//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//

#include <stddef.h>
#include <stdint.h>
#include "TO.h"
#include "TOSE_helper_certs.h"
#include "nx_secure_x509.h"
#include "nx_crypto_ecdsa.h"
#include "iotc_algorithms.h"
#include "azrtos_crypto_config.h"
#include "iotc_auth_driver.h"
#include "to_auth_driver.h"


#ifdef NX_SECURE_X509_KEY_TYPE_HARDWARE // Need NetX 6.1.7 or newer to compile this

#define SR_BUFFER_SIZE TO_SIGNATURE_SIZE // two big integers (32 bytes each)
#define DC_MAGIC 0x3c

struct to_driver_context {
    struct to_driver_parameters driver_parameters;
    IotcAzccCryptoConfig crypto_config;
    TOSE_ctx_t* to_ctx;
    uint8_t cert_buffer[TO_CERT_X509_MAXSIZE];
    char cn_buffer[TO_CERT_SUBJECT_CN_MAXSIZE + 1];
    char magic;
};

#define ASN1_TAG_SET 0x31
#define ASN1_TAG_LEN_SIZE 2
#define ASN1_TAG_SEQ 0x30
#define ASN1_TAG_STR 0x0C
static const uint8_t ASN1_OID_CN[] = {0x06, 0x03, 0x55, 0x04, 0x03};

static TO_ret_t build_dn_from_cn(uint8_t *dn, const char* cn, uint16_t *size)
{
    if ((dn == NULL) || (cn == NULL) || (size == NULL))
    {
        return TO_INVALID_PARAM;
    }
    uint32_t cn_len = strlen(cn);
    if (cn_len > TO_CERT_SUBJECT_CN_MAXSIZE)
    {
        return TO_INVALID_PARAM;
    }
    uint8_t total_len = (uint8_t)(ASN1_TAG_LEN_SIZE + sizeof(ASN1_OID_CN) + ASN1_TAG_LEN_SIZE + cn_len);
    uint8_t i = 0;
    dn[i++] = ASN1_TAG_SET;
    dn[i++] = total_len;
    dn[i++] = ASN1_TAG_SEQ;
    dn[i] = total_len - i + 1;
    i++;
    memcpy(&dn[i], ASN1_OID_CN, sizeof(ASN1_OID_CN));
    i += sizeof(ASN1_OID_CN);
    dn[i++] = ASN1_TAG_STR;
    dn[i] = total_len - i + 1;
    i++;
    snprintf((char*)&dn[i], cn_len + 1, "%s", cn);
    *size = ASN1_TAG_LEN_SIZE + total_len;

    return TO_OK;
}

static bool is_context_valid(IotcAuthInterfaceContext context) {
	struct to_driver_context *to_context = (struct to_driver_context*) context;
	bool ret = DC_MAGIC == to_context->magic;
	if (!ret) {
		printf("ADTO: Context is invalid\r\n");
	}
	return ret;
}

static NX_CRYPTO_KEEP UINT  to_ecdsa_operation(UINT op,      /* Encrypt, Decrypt, Authenticate */
        VOID *handle, /* Crypto handler */
        struct NX_CRYPTO_METHOD_STRUCT *method,
        UCHAR *key, // our context will be stored in the key
        NX_CRYPTO_KEY_SIZE key_size_in_bits,
        UCHAR *input,
        ULONG input_length_in_byte,
        UCHAR *iv_ptr,
        UCHAR *output,
        ULONG output_length_in_byte,
        VOID *crypto_metadata,
        ULONG crypto_metadata_size,
        VOID *packet_ptr,
        VOID (*nx_crypto_hw_process_callback)(VOID *, UINT))
{
	if (op == NX_CRYPTO_EC_CURVE_SET) {
		return _nx_crypto_method_ecdsa_operation(op, handle, method, key, key_size_in_bits, input, input_length_in_byte,
					iv_ptr, output, output_length_in_byte, crypto_metadata, crypto_metadata_size, packet_ptr,
					nx_crypto_hw_process_callback);

	} else if (op == NX_CRYPTO_AUTHENTICATE) {
		if (!is_context_valid(key)) return NX_CRYPTO_NOT_SUCCESSFUL;
		struct to_driver_context* to_context = (struct to_driver_context*) key;
		static uint8_t s_r_buffer[SR_BUFFER_SIZE];
		TO_ret_t ret = TOSE_sign_hash(to_context->to_ctx, to_context->driver_parameters.operational_identity_slot, input, s_r_buffer);
		if (TORSP_SUCCESS != ret) {
	    	printf("ADTO: TOSE_sign error 0x%04X\r\n", ret);
			return NX_CRYPTO_NOT_SUCCESSFUL;
		}

		// Uncommenting the the section will help debugging but is a security risk:
#if 0
		printf("ADTO: ECDSA(");
		for (int i=0; input && i <input_length_in_byte; i++) {
							printf("%02x", input[i]);
			}
		printf(")=");
#endif

		NX_CRYPTO_EXTENDED_OUTPUT* eo = (NX_CRYPTO_EXTENDED_OUTPUT *)output;
		size_t signature_size;
		int sig_ret = sig_to_asn1_der(
				eo->nx_crypto_extended_output_data,
				eo->nx_crypto_extended_output_length_in_byte,
				&signature_size,
				&s_r_buffer[0], &s_r_buffer[SR_BUFFER_SIZE/2], TO_ECC_PRIV_KEYSIZE * 8
				);
		eo->nx_crypto_extended_output_actual_size = (ULONG)signature_size; // ULONG SHLONG BONG.. these guys love not using standard
		if (sig_ret) {
			return NX_CRYPTO_NOT_SUCCESSFUL;
		}
		UCHAR* t = eo->nx_crypto_extended_output_data;
		for (int i=0; t && i < eo->nx_crypto_extended_output_actual_size; i++) {
			printf("%02x", t[i]);
		}
		printf("\r\n");
		return NX_CRYPTO_SUCCESS;
	} else {
		printf("ADTO: Unknown ECDSA operation invoked %u\r\n", op);
		return NX_CRYPTO_NOT_SUCCESSFUL;
	}
}

// Return a DER formatted certificate given the slot number
static int to_get_cert(IotcAuthInterfaceContext context, uint8_t cert_slot, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
	struct to_driver_context *to_context = (struct to_driver_context*) context;

	if (cert_slot < 0 || cert_slot > 2) {
		printf("ADTO: get_cert: Slot must be 0, 1 or 2 \r\n");
		return -2;
	}
	if (!cert || cert_size == NULL) {
		printf("ADTO: get_cert: Invalid parameters for get_cert operation\r\n");
		return -3;
	}

	*cert_size = 0;

	uint16_t cs;
	TO_ret_t ret = TOSE_get_certificate_x509( //
			to_context->to_ctx, //
			cert_slot, //
			to_context->cert_buffer, //
			&cs //
			);
	if (TORSP_SUCCESS != ret) {
		printf("ADTO: get_to_cert: Get certificate error 0x%04x\r\n", ret);
		return -2;
	}
	*cert = to_context->cert_buffer;
	*cert_size = cs;
	return 0;
}


// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static int to_get_operational_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
	struct to_driver_context *to_context = (struct to_driver_context*) context;
    return to_get_cert(context, to_context->driver_parameters.operational_identity_slot, cert, cert_size);
}
// Return a DER formatted certificate that will be used for dynamic identity
static int to_get_bootstrap_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
	struct to_driver_context *to_context = (struct to_driver_context*) context;
    return to_get_cert(context, to_context->driver_parameters.bootstrap_identity_slot, cert, cert_size);
}

// In case of NX_SECURE_X509_KEY_TYPE_HARDWARE, the private key is ignored by AzureRTOS.
// However, the private key is passed in to to_ecdsa_operation, so that the context can be used in the callback
static int to_get_private_key(IotcAuthInterfaceContext context, uint8_t** key, size_t* key_size) {
	if (!is_context_valid(context)) return -1;
	*key = context;
	*key_size = sizeof(IotcAuthInterfaceContext);
	return 0;
}

// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static unsigned int to_get_azrtos_private_key_type(IotcAuthInterfaceContext context) {
	return NX_SECURE_X509_KEY_TYPE_HARDWARE;
}

static int to_get_serial(IotcAuthInterfaceContext context, uint8_t *serial, size_t *size) {
	int ret;
	struct to_driver_context *to_context = (struct to_driver_context*) context;

	if (TOSE_get_serial_number(to_context->to_ctx, serial) != TORSP_SUCCESS) {
		printf("Unable to get Secure Element serial number\r\n");
		ret = -2;
		goto err;
	}
	*size = TO_SN_SIZE;

	ret = 0;

err:
		return ret;
}

// Generate a CSR using the CSR slot defined in driver parameters and return the pointer to the CSR
// NOTE: The request will invalidate any cert or CSR previously stored in the cert buffer.
static int to_generate_csr(IotcAuthInterfaceContext context, const char *cn, uint8_t **csr, size_t *len) {
	if (!is_context_valid(context)) return -1;
	struct to_driver_context *to_context = (struct to_driver_context*) context;
	TO_ret_t ret;
	uint8_t dn[TO_CERT_DN_MAXSIZE];

	*len = 0;

	if ((to_context == NULL) || (to_context->to_ctx == NULL)) {
		return TO_ERROR;
	}

	struct to_driver_parameters* p = &to_context->driver_parameters;
	if (p->operational_csr_slot <= 0 || p->operational_csr_slot > 2) {
		printf("ADTO: csr: Slot must be 1 or 2 \r\n");
		return -2;
	}
	if (!csr || len == NULL) {
		printf("ADTO: csr: Invalid parameters for the CSR operation\r\n");
		return -3;
	}

	uint16_t dn_size;
	ret = build_dn_from_cn(dn, cn, &dn_size);
	if (dn_size > TO_CERT_DN_MAXSIZE) {
		printf("ADTO: error: dn_size exceeded TO_CERT_DN_MAXSIZE!\r\n");
		return ret;
	}
	if (ret) {
		printf("ADTO: Failed to build DN from requested CN\r\n");
		return ret;
	}

	ret = TOSE_set_certificate_signing_request_dn( //
			to_context->to_ctx, //
			to_context->driver_parameters.operational_csr_slot, //
			dn, //
			dn_size //
			);
	if (ret != TORSP_SUCCESS) {
		printf("TOSE_set_certificate_signing_request_dn <error %04X>\r\n", ret);
	} else {
		ret = 0; // correction for non-zero TORSP_SUCCESS

		uint16_t cert_size;
		ret = TOSE_get_certificate_signing_request( //
				to_context->to_ctx, //
				to_context->driver_parameters.operational_csr_slot, //
				to_context->cert_buffer, //
				&cert_size //
				);
		if (ret != TORSP_SUCCESS) {
			printf("TOSE_get_certificate_signing_request <error %04X>\r\n", ret);
		} else {
			ret = 0; // correction for non-zero TORSP_SUCCESS
			*csr=to_context->cert_buffer;
			*len = cert_size;
		}
	}
	return (int) ret;
}

static int to_store_operational_cert(IotcAuthInterfaceContext context, uint8_t* cert, size_t cert_len) {
	if (!is_context_valid(context)) return -1;
	struct to_driver_context *to_context = (struct to_driver_context*) context;
	TO_ret_t ret;

	struct to_driver_parameters* p = &to_context->driver_parameters;
	if (p->operational_csr_slot <= 0 || p->operational_csr_slot > 2) {
		printf("ADTO: cert store: CSR slot must be 1 or 2 \r\n");
		return -2;
	}
	if (!cert || cert_len == 0) {
		printf("ADTO: cert store: Invalid parameters for cert store operation\r\n");
		return -3;
	}

	ret = TOSE_helper_set_certificate_x509( //
			to_context->to_ctx, //
			to_context->driver_parameters.operational_csr_slot, //
			cert, cert_len);
	if (ret != TO_OK) {
		printf("ADTO: cert store: failed to store cert error: 0x%04X>\r\n", ret);
		return -4;
	}
	return 0;
}

static int to_sign_hash(IotcAuthInterfaceContext context, uint8_t* input_hash, uint8_t* output) {
	if (!is_context_valid(context)) return -1;
	struct to_driver_context* to_context = (struct to_driver_context*) context;

	TO_ret_t ret = TOSE_sign_hash(to_context->to_ctx, to_context->driver_parameters.bootstrap_identity_slot, input_hash, output);
	if (TORSP_SUCCESS != ret) {
    	printf("ADTO: TOSE_sign error 0x%04X\r\n", ret);
		return -2;
	}
	return 0;
}

static IotcAzccCryptoConfig* to_get_crypto_config(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct to_driver_context* to_context = (struct to_driver_context*) context;
	return &(to_context->crypto_config);
}

static char* to_extract_cn(IotcAuthInterfaceContext context, int cert_index) {
	if (!is_context_valid(context)) return NULL;
	struct to_driver_context* to_context = (struct to_driver_context*) context;
	TO_ret_t ret = TOSE_get_certificate_subject_cn(to_context->to_ctx, (uint8_t) cert_index, to_context->cn_buffer);
	if (ret != TORSP_SUCCESS) {
		printf("ADTO: failed to extract certificate CN: 0x%04X\r\n", ret);
		return NULL;
	}
	return to_context->cn_buffer;
}

static char* to_extract_operational_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct to_driver_context* to_context = (struct to_driver_context*) context;
	return to_extract_cn(context, to_context->driver_parameters.operational_identity_slot);
}

static char* to_extract_botstrap_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct to_driver_context* to_context = (struct to_driver_context*) context;
	return to_extract_cn(context, to_context->driver_parameters.bootstrap_identity_slot);
}

int to_create_auth_driver(IotcAuthInterface* driver_interface, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext* context, struct to_driver_parameters *driver_parameters) {
	if (!driver_parameters) {
		printf("ADTO: Driver parameters are required\r\n");
		return -1;
	}

	struct to_driver_context *c = (struct to_driver_context*) malloc(sizeof(struct to_driver_context));
	c->magic = DC_MAGIC;
	if (!c) {
		printf("ADTO: Unable to allocate context!\r\n");
		return -3;
	}
    memcpy(&(c->driver_parameters), driver_parameters, sizeof(struct to_driver_parameters));

    if (!driver_interface || !context) {
    	printf("ADTO: Incorrect parameters\r\n");
    	to_release_auth_driver((IotcAuthInterfaceContext*)c);
    	return -4;
    }
    c->to_ctx = TODRV_HSE_get_ctx();

    // A driver can decide to power up the chip for every operation or not
    // It is a good idea to at least check operation before registering the driver
    TO_ret_t ret = TOSE_init(c->to_ctx);
    if (TO_OK != ret) {
    	printf("ADTO: TOSE_init <error %04X>\r\n", ret);
    	to_release_auth_driver((IotcAuthInterfaceContext*)c);
		return -1;
	}

    if (0 != iotcazcc_init_with_ec_curve_secp256(&c->crypto_config)) {
    	printf("ADTO: failed to init the crypto subsystem\r\n");
    	to_release_auth_driver((IotcAuthInterfaceContext*)c);
    	return -2;
    }

	c->crypto_config.custom_crypto_method_storage.nx_crypto_operation = to_ecdsa_operation;
    // TODO: Our own init/cleanup?
	c->crypto_config.custom_crypto_method_storage.nx_crypto_init = _nx_crypto_method_ecdsa_init;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_cleanup = _nx_crypto_method_ecdsa_cleanup;

    *context = (IotcAuthInterfaceContext) c;

    driver_interface->get_serial = to_get_serial;
    driver_interface->get_cert = to_get_operational_cert;
    driver_interface->get_private_key = to_get_private_key;
    driver_interface->get_crypto_config = to_get_crypto_config;
    driver_interface->get_azrtos_private_key_type = to_get_azrtos_private_key_type;

    if (ddim_interface) {
    	ddim_interface->get_bootstrap_cert = to_get_bootstrap_cert;
        ddim_interface->generate_csr = to_generate_csr;
        ddim_interface->sign_hash = to_sign_hash;
        ddim_interface->extract_bootstrap_cn = to_extract_botstrap_cn;
        ddim_interface->extract_operational_cn = to_extract_operational_cn;
        ddim_interface->store_operational_cert = to_store_operational_cert;
    }

	/* TODO: Consider delaying in TO_init or here if we use GPIO power */
	return 0;
}

#else
int to_create_auth_driver(IotcAuthInterface* driver_interface, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext* context, struct to_driver_parameters *driver_parameters) {
	printf("ADTO: NetX 6.1.7 or newer is required\r\n");
	return -1;

}
#endif

int to_release_auth_driver(IotcAuthInterfaceContext* context) {
	struct to_driver_context* to_context = (struct to_driver_context*) context;

	if (!is_context_valid(context)) {
		printf("ADTO: invalid context\r\n");
		return -1; // not sure what to do here
	}
	TO_lib_ret_t ret = TOSE_fini(to_context->to_ctx);
	to_context->magic = 0xdf; // to be able to detect a double free
	free(to_context);
	return ret;
}
