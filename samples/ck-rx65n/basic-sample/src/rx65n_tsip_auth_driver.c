//
// Copyright: Avnet 2023
// Created by Nick Beck <nbeck@witekio.com>
//
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "nx_secure_x509.h"
#include "nx_crypto_rsa.h"

#include "iotc_algorithms.h"
#include "azrtos_crypto_config.h"
#include "rx65n_tsip_auth_driver.h"
#include "r_tsip_rx_if.h"
#include "secure_boot.h"
#include "key_data.h"

#ifdef NX_SECURE_X509_KEY_TYPE_HARDWARE // Need NetX 6.1.7 or newer to compile this

// Uncomment below macro to enable some debug logging
//#define ENABLE_TSIP_AUTH_DRIVER_DEBUG

#define DC_MAGIC 0x3d

#define CERT_BUF_MAX_SIZE           1024

struct rx65n_tsip_driver_context {
    struct rx65n_tsip_driver_parameters driver_parameters;
    IotcAzccCryptoConfig crypto_config;
    uint8_t cert[CERT_BUF_MAX_SIZE];
    size_t cert_size;
    char cn_buffer[IOTC_COMMON_NAME_MAX_LEN + 1];
    char magic;
};


static NX_CRYPTO_KEEP UINT  rx65n_tsip_rsa_operation(UINT op,      /* Encrypt, Decrypt, Authenticate */
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
        VOID (*nx_crypto_hw_process_callback)(VOID *, UINT));

/* TLS ciphersuites. */
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_tls_ecdhe_rsa_with_aes_128_cbc_sha256;
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_tls_rsa_with_aes_128_cbc_sha256;

/* X.509 ciphersuites.  */
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_rsa_sha_256;
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_ecdsa_sha_256;

// Always add these crypto methods
const NX_CRYPTO_METHOD* rx65n_tsip_default_nx_crypto_methods[] =
{
    &crypto_method_hmac,
    &crypto_method_hmac_sha256,
    &crypto_method_tls_prf_sha256,
    &crypto_method_sha256,
    &crypto_method_aes_cbc_128,
};

static int rx65n_tsip_crypto_init_with_rsa_defaults(IotcAzccCryptoConfig *crypto) {
	memcpy(crypto->crypto_methods, rx65n_tsip_default_nx_crypto_methods, sizeof(rx65n_tsip_default_nx_crypto_methods));
	crypto->crypto_methods_length = sizeof(rx65n_tsip_default_nx_crypto_methods) /  sizeof(NX_CRYPTO_METHOD *);
	crypto->tls_ciphersuites[0] = &nx_crypto_tls_rsa_with_aes_128_cbc_sha256;
	crypto->tls_ciphersuites[1] = &nx_crypto_x509_rsa_sha_256;
	crypto->tls_ciphersuites_length = 2;
	memset(&crypto->custom_crypto_method_storage, 0, sizeof(NX_CRYPTO_METHOD));
	return 0;

}

static bool is_context_valid(IotcAuthInterfaceContext context) {
	struct rx65n_tsip_driver_context *auth_context = (struct rx65n_tsip_driver_context*) context;
	bool ret = DC_MAGIC == auth_context->magic;
	if (!ret) {
		printf("TSIP: Context is invalid\r\n");
	}
	return ret;
}

// Return a DER formatted certificate given the slot number
static int rx65n_tsip_get_cert(IotcAuthInterfaceContext context, uint8_t cert_slot, uint8_t **cert, size_t *cert_size) {
	iotc_auth_err_t ret = IOTC_AUTH_SUCCESS;

	if (!is_context_valid(context)) return IOTC_AUTH_BAD_CONTEXT;
	struct rx65n_tsip_driver_context *rx65n_tsip_context = (struct rx65n_tsip_driver_context*) context;

	if (cert_slot != 0) {
		printf("TSIP: get_cert: Slot must be 0 \r\n");
		return IOTC_AUTH_INVALID_PARAMETER;
	}
	if (!cert || cert_size == NULL) {
		printf("TSIP: get_cert: Invalid parameters for get_cert operation\r\n");
		return IOTC_AUTH_INVALID_PARAMETER;
	}

	*cert_size = 0;
	// Obtain the X.509 device cert that is stored in flash
	// TODO - Currently we store the device cert in an array in sample_device_identity.c
	extern unsigned char sample_device_cert_ptr[];
	extern unsigned int sample_device_cert_len;
	if (sample_device_cert_len > sizeof(rx65n_tsip_context->cert))
	{
		printf("TSIP: get_cert: Certificate too large error\r\n");
		return IOTC_AUTH_ERR_FAIL;
	}
	memcpy(rx65n_tsip_context->cert, sample_device_cert_ptr, sample_device_cert_len);
	rx65n_tsip_context->cert_size = sample_device_cert_len;
	*cert = rx65n_tsip_context->cert;
	*cert_size = sample_device_cert_len;

    return ret;
}


// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static int rx65n_tsip_get_operational_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return IOTC_AUTH_BAD_CONTEXT;
    return rx65n_tsip_get_cert(context, 0, cert, cert_size);
}
// Return a DER formatted certificate that will be used for dynamic identity
static int rx65n_tsip_get_bootstrap_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
    //TODO: Decide how to deal with multiple slots. Will need to use the UpdateKey keyindex and associated UpdateRSA*PrivateKeyIndex() functions when DDIM support added
	if (!is_context_valid(context)) return IOTC_AUTH_BAD_CONTEXT;
    return rx65n_tsip_get_cert(context, 0, cert, cert_size);
}

// In case of NX_SECURE_X509_KEY_TYPE_HARDWARE_RX_TSIP, the private key is ignored by AzureRTOS
// However, the context is passed in as the private key so that it can be used in the to rx65n_tsip_rsa_operation
static int rx65n_tsip_get_private_key(IotcAuthInterfaceContext context, uint8_t** key, size_t* key_size) {
	if (!is_context_valid(context)) return IOTC_AUTH_BAD_CONTEXT;
	// It is deliberatly not possible to extract the private key from the TSIP core and this is
	//  ignored by AzureRTOS anyway as the private key_type is set to NX_SECURE_X509_KEY_TYPE_HARDWARE_RX_TSIP.
	// However, the context is passed in as the private key so that it can be used in the to rx65n_tsip_rsa_operation
	// If a NULL key with zero length is returned then the certificate will not be added to the payload
	//  of the TLS Client Certificate message as AzureRTOS treats the cert as not being an identity certificate.
	//TODO - Use this context being passed in as the "key" to rx65n_tsip_rsa_operation() handler to pass info like keysize and tsip keyindexes
	*key = context;
	*key_size = sizeof(IotcAuthInterfaceContext);
	return IOTC_AUTH_SUCCESS;
}

static unsigned int rx65n_tsip_get_azrtos_private_key_type(IotcAuthInterfaceContext context) {
// The macro NX_SECURE_X509_KEY_TYPE_USER_DEFINED_MASK is the start of the range for custom key types
// and we add an arbitrary offset/value to this for our RX TSIP custom h/w key handler
#define NX_SECURE_X509_KEY_TYPE_HARDWARE_RX_TSIP (NX_SECURE_X509_KEY_TYPE_USER_DEFINED_MASK + 0x1000)

	return NX_SECURE_X509_KEY_TYPE_HARDWARE_RX_TSIP;
}

static int rx65n_tsip_get_serial(IotcAuthInterfaceContext context, uint8_t *serial, size_t *size) {
    printf("TSIP: rx65n_tsip_get_serial not implemented\r\n");
    return IOTC_AUTH_NOT_IMPLEMENTED;
}

// Generate a CSR using the CSR slot defined in driver parameters and return the pointer to the CSR
// NOTE: The request will invalidate any cert or CSR previously stored in the cert buffer.
static int rx65n_tsip_generate_csr(IotcAuthInterfaceContext context, const char *cn, uint8_t **csr, size_t *len) {
    printf("TSIP: rx65n_tsip_generate_csr not implemented\r\n");
	return IOTC_AUTH_NOT_IMPLEMENTED;
}

static int rx65n_tsip_store_operational_cert(IotcAuthInterfaceContext context, uint8_t* cert, size_t cert_len) {
    printf("TSIP: rx65n_tsip_store_operational_cert not implemented\r\n");
	return IOTC_AUTH_NOT_IMPLEMENTED;
}

static int rx65n_tsip_sign_hash(IotcAuthInterfaceContext context, uint8_t* input_hash, uint8_t* output) {
    printf("TSIP: rx65n_tsip_sign_hash not implemented\r\n");
	return IOTC_AUTH_NOT_IMPLEMENTED;
}

static IotcAzccCryptoConfig* rx65n_tsip_get_crypto_config(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct rx65n_tsip_driver_context* rx65n_tsip_context = (struct rx65n_tsip_driver_context*) context;
	return &(rx65n_tsip_context->crypto_config);
}

static char* rx65n_tsip_extract_cn(IotcAuthInterfaceContext context, int cert_index) {

    (void)cert_index; //TODO: make use of index
	if (!is_context_valid(context)) return NULL;
	struct rx65n_tsip_driver_context* rx65n_tsip_context = (struct rx65n_tsip_driver_context*) context;

    NX_SECURE_X509_CERT dev_certificate;
    UINT nx_status;

    nx_status = nx_secure_x509_certificate_initialize(&dev_certificate,
                                (UCHAR *)rx65n_tsip_context->cert, (USHORT)rx65n_tsip_context->cert_size,
                                NX_NULL, 0,
                                (UCHAR *)"0", 1,
                                NX_SECURE_X509_KEY_TYPE_HARDWARE);
    if (nx_status) {
        printf("nx_secure_x509_certificate_initialize failed with status %d\r\n", nx_status);
        return NULL;
    }

    USHORT cn_length = dev_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name_length;
    if (cn_length <= IOTC_COMMON_NAME_MAX_LEN) {
        NX_CRYPTO_MEMCPY(
                rx65n_tsip_context->cn_buffer,
                dev_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name,
                cn_length);
        rx65n_tsip_context->cn_buffer[cn_length] = 0; // terminate the string
    } else {
        return NULL;
    }

	return rx65n_tsip_context->cn_buffer;
}

static char* rx65n_tsip_extract_operational_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	return rx65n_tsip_extract_cn(context, 0);
}

static char* rx65n_tsip_extract_bootstrap_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	return rx65n_tsip_extract_cn(context, 0);
}

int rx65n_tsip_create_auth_driver(IotcAuthInterface* driver_interface, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext* context, struct rx65n_tsip_driver_parameters *driver_parameters) {
	int ret = IOTC_AUTH_SUCCESS;
	struct rx65n_tsip_driver_context *c = NULL;

	if (!driver_parameters) {
		printf("TSIP: Driver parameters are required\r\n");
		ret = IOTC_AUTH_INVALID_PARAMETER;
		goto create_end;
	}

	c = (struct rx65n_tsip_driver_context*) malloc(sizeof(struct rx65n_tsip_driver_context));
	memset(c, 0, sizeof(struct rx65n_tsip_driver_context));

	if (!c) {
		printf("TSIP: Unable to allocate context!\r\n");
		ret = IOTC_AUTH_BAD_CONTEXT;
		goto create_end;
	}
	c->magic = DC_MAGIC;
    memcpy(&(c->driver_parameters), driver_parameters, sizeof(struct rx65n_tsip_driver_parameters));

    if (!driver_interface || !context) {
    	printf("TSIP: Incorrect parameters\r\n");
    	rx65n_tsip_release_auth_driver((IotcAuthInterfaceContext*)c);
        ret = IOTC_AUTH_INVALID_PARAMETER;
        goto create_end;
    }

    // Set the default crypto methods for RSA
    ret = rx65n_tsip_crypto_init_with_rsa_defaults(&c->crypto_config);
	if (0 != ret) {
		ret = IOTC_AUTH_ERR_FAIL;
		goto create_end;
	}
	// Add the custom crypto method that uses the RX TSIP crypto core to the array of methods
	memcpy(&(c->crypto_config.custom_crypto_method_storage), &crypto_method_rsa, sizeof(NX_CRYPTO_METHOD));
	c->crypto_config.custom_crypto_method_storage.nx_crypto_operation = rx65n_tsip_rsa_operation;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_init = _nx_crypto_method_rsa_init;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_cleanup = _nx_crypto_method_rsa_cleanup;
	iotcazcc_register_crypto_method(&c->crypto_config, &(c->crypto_config.custom_crypto_method_storage));

    *context = (IotcAuthInterfaceContext) c;

    // Setup function pointers for rx tsip auth driver
    driver_interface->get_serial = rx65n_tsip_get_serial;
    driver_interface->get_cert = rx65n_tsip_get_operational_cert;
    driver_interface->get_private_key = rx65n_tsip_get_private_key;
    driver_interface->get_crypto_config = rx65n_tsip_get_crypto_config;
    driver_interface->get_azrtos_private_key_type = rx65n_tsip_get_azrtos_private_key_type;

    if (ddim_interface) {
    	ddim_interface->get_bootstrap_cert = rx65n_tsip_get_bootstrap_cert;
        ddim_interface->generate_csr = rx65n_tsip_generate_csr;
        ddim_interface->sign_hash = rx65n_tsip_sign_hash;
        ddim_interface->extract_bootstrap_cn = rx65n_tsip_extract_bootstrap_cn;
        ddim_interface->extract_operational_cn = rx65n_tsip_extract_operational_cn;
        ddim_interface->store_operational_cert = rx65n_tsip_store_operational_cert;
    }
	ret = IOTC_AUTH_SUCCESS;
	return ret;
create_end:
	// Free the context if it had been allocated
	if (c)
	{
		free(c);
		c = NULL;
	}
	return ret;
}


int rx65n_tsip_release_auth_driver(IotcAuthInterfaceContext* context) {
	struct rx65n_tsip_driver_context* rx65n_tsip_context = (struct rx65n_tsip_driver_context*) context;
	if (!is_context_valid(context)) {
		printf("TSIP: invalid context\r\n");
		return IOTC_AUTH_BAD_CONTEXT; // not sure what to do here
	}
	rx65n_tsip_context->magic = 0xdf; // to be able to detect a double free
	free(rx65n_tsip_context);
	rx65n_tsip_context = NULL;
	return IOTC_AUTH_SUCCESS;
}

#ifdef ENABLE_TSIP_AUTH_DRIVER_DEBUG
static void tsip_print_buffer_as_hex(char *title, uint8_t *buf, int buf_len)
{
	int i;

	printf("%s:\r\n", title);
	for (i = 0; i < buf_len; i++)
	{
		printf("0x%02x ", buf[i]);
	}
	printf("\r\n\r\n");
}

#define tsip_debug_printf(...) printf(__VA_ARGS__)

#else //ENABLE_TSIP_AUTH_DRIVER_DEBUG
#define tsip_print_buffer_as_hex(title, buf, buf_len) do {} while(0)
#define tsip_debug_printf(...) do {} while(0)
#endif //ENABLE_TSIP_AUTH_DRIVER_DEBUG

static UINT convert_tsip_to_nx_crypto_error(e_tsip_err_t tsip_err)
{
	tsip_err = TSIP_SUCCESS;
	UINT ret;

	switch(tsip_err)
	{
		case TSIP_ERR_FAIL:
			ret = NX_CRYPTO_NOT_SUCCESSFUL;
			break;
		case TSIP_ERR_RESOURCE_CONFLICT:
			ret = NX_CRYPTO_NO_INSTANCE;
			break;
		case TSIP_ERR_RETRY:
			ret = NX_CRYPTO_NO_INSTANCE;
			break;
		case TSIP_ERR_KEY_SET:
			ret = NX_CRYPTO_INVALID_KEY;
			break;
		case TSIP_ERR_AUTHENTICATION:
			ret = NX_CRYPTO_AUTHENTICATION_FAILED;
			break;
		case TSIP_ERR_CALLBACK_UNREGIST:
			ret = NX_CRYPTO_INVALID_LIBRARY;
			break;
		case TSIP_ERR_PARAMETER:
			ret = NX_CRYPTO_INVALID_PARAMETER;
			break;
		case TSIP_ERR_PROHIBIT_FUNCTION:
			ret = NX_CRYPTO_INVALID_ALGORITHM;
			break;
		case TSIP_RESUME_FIRMWARE_GENERATE_MAC:
			ret = NX_CRYPTO_METHOD_INITIALIZATION_FAILURE;
			break;
		case TSIP_ERR_VERIFICATION_FAIL:
			ret = NX_CRYPTO_NOT_SUCCESSFUL;
			break;
		default:
			ret = NX_CRYPTO_NOT_SUCCESSFUL;
			break;
	}
	return ret;
}

static NX_CRYPTO_KEEP UINT  rx65n_tsip_rsa_operation(UINT op,      /* Encrypt, Decrypt, Authenticate */
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
	UINT ret = 0;
	tsip_rsa_byte_data_t message_hash_data = {0};
	tsip_rsa_byte_data_t signature_data = {0};
	e_tsip_err_t tsip_err = TSIP_SUCCESS;
	// TODO - Store and obtain the private key and keysize etc from RX TSIP auth driver context passed in `key` parameter of this function
	extern st_key_block_data_t g_key_block_image;
	tsip_rsa1024_private_key_index_t *rsa1024_private_keyindex = &g_key_block_image.key_data.user_rsa1024_nd_key_index;

	// Debug print of values passed into function
	tsip_debug_printf("%s: values passed into function - op=%d, keysize=%dbits, input-size=%d, output-size=%d\r\n",
			__func__, op, key_size_in_bits, input_length_in_byte, output_length_in_byte);

	if (op == NX_SECURE_X509_KEY_TYPE_HARDWARE_RX_TSIP)
	{
		tsip_debug_printf("\r\n Using custom hardware key handler for NX_SECURE_X509_KEY_TYPE_HARDWARE_RX_TSIP\r\n\r\n");
		// Adjust the values to be valid for this custom RX TSIP h/w key operation
		// TODO - Only supports 1024-bit RSA currently - obtain the `key_size_in_bits` and `input_length_in_byte` from the keysize passed in the context
		key_size_in_bits = 1024;
		input_length_in_byte = 128;
		tsip_debug_printf("%s: correctly adjusted values op=%d, keysize=%dbits, input-size=%d, output-size=%d\r\n",
				__func__, op, key_size_in_bits, input_length_in_byte, output_length_in_byte);

		tsip_print_buffer_as_hex("Key", key, (key_size_in_bits / 8));
		tsip_print_buffer_as_hex("Input", input, input_length_in_byte);
		if ((tsip_err = R_TSIP_Open(NULL, NULL)) != TSIP_SUCCESS)
		{
			printf("Failed to initialise TSIP: %u\r\n", tsip_err);
			R_TSIP_Close();
			return convert_tsip_to_nx_crypto_error(tsip_err);
		}
		// The `input` message digest includes RSA PKCS1 v1.5 padding for the keysize
		// However the TSIP signature generation functions only want the raw SHA256 hash value
		// which is the last 256bits/32bytes of the input buffer
#define RX_TSIP_RSA_SIGNATURE_HASH_OFFSET_IN_RSA_PKCS_SHA256_PADDED_MESSAGE_DIGEST (128-32)
#define RX_TSIP_RSA_SIGNATURE_GENERATE_HASH_FROM_INPUT_DATA (0)
#define RX_TSIP_RSA_SIGNATURE_INPUT_DATA_IS_A_HASH (1)

		message_hash_data.pdata = input + RX_TSIP_RSA_SIGNATURE_HASH_OFFSET_IN_RSA_PKCS_SHA256_PADDED_MESSAGE_DIGEST;
		// Create a hash of the message pointed to by message_hash_data.pdata
		message_hash_data.data_type = RX_TSIP_RSA_SIGNATURE_INPUT_DATA_IS_A_HASH; // 0 = message, 1 = hash
		// We only need to set the data length if the data_type == 0 (i.e. a message not a hash) so don't bother to set it

		signature_data.pdata = output;
		signature_data.data_length = output_length_in_byte;
		memset(signature_data.pdata, 0, signature_data.data_length);
		tsip_err = R_TSIP_RsassaPkcs1024SignatureGenerate(&message_hash_data,
														  &signature_data,
														  rsa1024_private_keyindex,
														  R_TSIP_RSA_HASH_SHA256);
		if (tsip_err != TSIP_SUCCESS)
		{
			printf("%s: Error performing R_TSIP_RsassaPkcs1024SignatureGenerate() (%d)\r\n", __func__, tsip_err);
			R_TSIP_Close();
			return convert_tsip_to_nx_crypto_error(tsip_err);
		}
		// Only print the signature, which could be smaller than the full output buffer size.
		tsip_print_buffer_as_hex("Output", output, signature_data.data_length);
		R_TSIP_Close();
		ret = NX_CRYPTO_SUCCESS;
	}
	else
	{
		tsip_print_buffer_as_hex("Key", key, (key_size_in_bits / 8));
		tsip_print_buffer_as_hex("Input", input, input_length_in_byte);
		ret = _nx_crypto_method_rsa_operation(op, handle, method, key, key_size_in_bits, input, input_length_in_byte,
				iv_ptr, output, output_length_in_byte, crypto_metadata, crypto_metadata_size, packet_ptr,
				nx_crypto_hw_process_callback);
		if (ret == NX_CRYPTO_SUCCESS)
		{
			tsip_print_buffer_as_hex("Output", output, output_length_in_byte);
		}
		else
		{
			printf("%s: Error running _nx_crypto_method_rsa_operation() (%d)\r\n", __func__, ret);
		}
	}
	return ret;
}

#else // NX_SECURE_X509_KEY_TYPE_HARDWARE

#error "TSIP: NetX 6.1.7 or newer is required"

#endif // NX_SECURE_X509_KEY_TYPE_HARDWARE
