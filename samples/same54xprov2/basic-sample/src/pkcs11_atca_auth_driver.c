//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//
#include "cryptoki.h"
#include <stddef.h>
#include <stdint.h>

#include "cryptoauthlib.h"

#include "nx_secure_x509.h"
#include "nx_crypto_ecdsa.h"
#include "nx_crypto_ecdsa_pkcs11_atca.h"

#include "iotc_algorithms.h"
#include "azrtos_crypto_config.h"
#include "pkcs11_atca_auth_driver.h"


#ifdef NX_SECURE_X509_KEY_TYPE_HARDWARE // Need NetX 6.1.7 or newer to compile this

#define SR_BUFFER_SIZE TO_SIGNATURE_SIZE // two big integers (32 bytes each)
#define DC_MAGIC 0x3d

#define CERT_BUF_MAX_SIZE           1024

static const char* pkcs11_trust_device_label = "device";

struct pkcs11_atca_driver_context {
    struct pkcs11_atca_driver_parameters driver_parameters;
    IotcAzccCryptoConfig crypto_config;
    uint8_t cert[CERT_BUF_MAX_SIZE];
    size_t cert_size;
    char cn_buffer[IOTC_COMMON_NAME_MAX_LEN + 1];
    char magic;
};

extern NX_CRYPTO_METHOD crypto_method_ecdsa_pkcs11_atca;

CK_RV nx_crypto_find_pkcs11_object(CK_SESSION_HANDLE session_handle, CK_ATTRIBUTE_PTR template,
                CK_OBJECT_HANDLE_PTR obj_handle_ptr);


static bool is_context_valid(IotcAuthInterfaceContext context) {
	struct pkcs11_atca_driver_context *to_context = (struct pkcs11_atca_driver_context*) context;
	bool ret = DC_MAGIC == to_context->magic;
	if (!ret) {
		printf("ATECC608: Context is invalid\r\n");
	}
	return ret;
}

// Return a DER formatted certificate given the slot number
static int pkcs11_atca_get_cert(IotcAuthInterfaceContext context, uint8_t cert_slot, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
	struct pkcs11_atca_driver_context *pkcs11_atca_context = (struct pkcs11_atca_driver_context*) context;

	if (cert_slot != 0) {
		printf("ATECC608: get_cert: Slot must be 0 \r\n");
		return -2;
	}
	if (!cert || cert_size == NULL) {
		printf("ATECC608: get_cert: Invalid parameters for get_cert operation\r\n");
		return -3;
	}

	*cert_size = 0;
    
    CK_C_INITIALIZE_ARGS  cinit_args = {0};
    CK_RV rv;
    CK_OBJECT_HANDLE device_cert;

    cinit_args.flags = CKF_OS_LOCKING_OK;

    rv = C_Initialize( &cinit_args );
    if (rv != CKR_OK && rv != CKR_CRYPTOKI_ALREADY_INITIALIZED) {
        printf("C_Initialize: rv = 0x%.8lX\r\n", rv);
        return(rv);
    }

    /* There is only one device slot-id 0 on atecc608b*/
    CK_SLOT_ID slotID = 0;

    /* Open a session on the slot found */
    CK_SESSION_HANDLE hSession;

	rv = C_OpenSession(slotID, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR,
	    &hSession);

	if (rv != CKR_OK) {
		printf("C_OpenSession: rv = 0x%.8lX\r\n", rv);
		goto exit_program;
	}
  
	/* create a privateKey handle for slot 0 */
    CK_OBJECT_CLASS class_device_cert_val = CKO_CERTIFICATE;
    


    CK_ATTRIBUTE device_cert_template[] = {
            {CKA_CLASS,     &class_device_cert_val, sizeof(CK_OBJECT_CLASS)},
            {CKA_LABEL, (CK_VOID_PTR*)pkcs11_trust_device_label, strlen(pkcs11_trust_device_label)}
    };

    rv = nx_crypto_find_pkcs11_object(hSession, device_cert_template, &device_cert);
    if (rv != CKR_OK) {
		printf("find_object for device_cert: rv = 0x%.8lX\r\n", rv);
		goto exit_session;
	}
    
    CK_ATTRIBUTE device_cert_template_1[] = {
            {CKA_VALUE, pkcs11_atca_context->cert, CERT_BUF_MAX_SIZE}
    };

    rv = C_GetAttributeValue(hSession, device_cert, device_cert_template_1, 1);
    if (rv != CKR_OK) {
		printf("C_GetAttributeValue for device_cert: rv = 0x%.8lX\r\n", rv);
		goto exit_session;
	}
    
    
	*cert = pkcs11_atca_context->cert;
    *cert_size = pkcs11_atca_context->cert_size = (size_t) device_cert_template_1[0].ulValueLen;

#ifdef ENABLE_PRINT_CERTIFICATE
    char displaystr[1024];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert((*cert), *cert_size, displaystr, &displaylen);
    printf("CLIENT: Rebuilt Device Certificate: \r\n%s\r\n", displaystr);
#endif

exit_session:
	(void) C_CloseSession(hSession);

exit_program:
    //(void) C_Finalize(NULL_PTR);
    return(rv);
}


// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static int pkcs11_atca_get_operational_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
    return pkcs11_atca_get_cert(context, 0, cert, cert_size);
}
// Return a DER formatted certificate that will be used for dynamic identity
static int pkcs11_atca_get_bootstrap_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
    //TODO: Decide how to deal with multiple slots
	if (!is_context_valid(context)) return -1;
    return pkcs11_atca_get_cert(context, 0, cert, cert_size);
}

// In case of NX_SECURE_X509_KEY_TYPE_HARDWARE, the private key is ignored by AzureRTOS.
// However, the private key is passed in to to_ecdsa_operation, so that the context can be used in the callback
static int pkcs11_atca_get_private_key(IotcAuthInterfaceContext context, uint8_t** key, size_t* key_size) {
	if (!is_context_valid(context)) return -1;
	*key = context;
	*key_size = sizeof(IotcAuthInterfaceContext);
	return 0;
}

// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static unsigned int pkcs11_atca_get_azrtos_private_key_type(IotcAuthInterfaceContext context) {
	return NX_SECURE_X509_KEY_TYPE_HARDWARE;
}

static int pkcs11_atca_get_serial(IotcAuthInterfaceContext context, uint8_t *serial, size_t *size) {
    printf("ATECC608: pkcs11_atca_get_serial not implemented\r\n");
    return -1;
}

// Generate a CSR using the CSR slot defined in driver parameters and return the pointer to the CSR
// NOTE: The request will invalidate any cert or CSR previously stored in the cert buffer.
static int pkcs11_atca_generate_csr(IotcAuthInterfaceContext context, const char *cn, uint8_t **csr, size_t *len) {
    printf("ATECC608: pkcs11_atca_generate_csr not implemented\r\n");
	return -1;
}

static int pkcs11_atca_store_operational_cert(IotcAuthInterfaceContext context, uint8_t* cert, size_t cert_len) {
    printf("ATECC608: pkcs11_atca_store_operational_cert not implemented\r\n");
	return -1;
}

static int pkcs11_atca_sign_hash(IotcAuthInterfaceContext context, uint8_t* input_hash, uint8_t* output) {
    printf("ATECC608: pkcs11_atca_sign_hash not implemented\r\n");
	return -1;
}

static IotcAzccCryptoConfig* pkcs11_atca_get_crypto_config(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct pkcs11_atca_driver_context* pkcs11_atca_context = (struct pkcs11_atca_driver_context*) context;
	return &(pkcs11_atca_context->crypto_config);
}

static char* pkcs11_atca_extract_cn(IotcAuthInterfaceContext context, int cert_index) {
    
    (void)cert_index; //TODO: make use of index
	if (!is_context_valid(context)) return NULL;
	struct pkcs11_atca_driver_context* pkcs11_atca_context = (struct pkcs11_atca_driver_context*) context;

    NX_SECURE_X509_CERT dev_certificate;
    UINT nx_status;

    nx_status = nx_secure_x509_certificate_initialize(&dev_certificate,
                                (UCHAR *)pkcs11_atca_context->cert, (USHORT)pkcs11_atca_context->cert_size,
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
                pkcs11_atca_context->cn_buffer, 
                dev_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name, 
                cn_length);
        pkcs11_atca_context->cn_buffer[cn_length] = 0; // terminate the string
    } else {
        return NULL;
    }
    
	return pkcs11_atca_context->cn_buffer;
}

static char* pkcs11_atca_extract_operational_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	return pkcs11_atca_extract_cn(context, 0);
}

static char* pkcs11_atca_extract_botstrap_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	return pkcs11_atca_extract_cn(context, 0);
}

int pkcs11_atca_create_auth_driver(IotcAuthInterface* driver_interface, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext* context, struct pkcs11_atca_driver_parameters *driver_parameters) {
	if (!driver_parameters) {
		printf("ATECC608: Driver parameters are required\r\n");
		return -1;
	}

	struct pkcs11_atca_driver_context *c = 
        (struct pkcs11_atca_driver_context*) malloc(sizeof(struct pkcs11_atca_driver_context));
    
	c->magic = DC_MAGIC;
	if (!c) {
		printf("ATECC608: Unable to allocate context!\r\n");
		return -3;
	}
    memcpy(&(c->driver_parameters), driver_parameters, sizeof(struct pkcs11_atca_driver_parameters));

    if (!driver_interface || !context) {
    	printf("ATECC608: Incorrect parameters\r\n");
    	pkcs11_atca_release_auth_driver((IotcAuthInterfaceContext*)c);
    	return -4;
    }
    if (0 != iotcazcc_init_with_ec_curve_secp256(&c->crypto_config)) {
    	printf("ATECC608: failed to init the crypto subsystem\r\n");
    	pkcs11_atca_release_auth_driver((IotcAuthInterfaceContext*)c);
    	return -2;
    }

	c->crypto_config.custom_crypto_method_storage.nx_crypto_operation = crypto_method_ecdsa_pkcs11_atca.nx_crypto_operation;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_init = crypto_method_ecdsa_pkcs11_atca.nx_crypto_init;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_cleanup = crypto_method_ecdsa_pkcs11_atca.nx_crypto_cleanup;

    *context = (IotcAuthInterfaceContext) c;

    driver_interface->get_serial = pkcs11_atca_get_serial;
    driver_interface->get_cert = pkcs11_atca_get_operational_cert;
    driver_interface->get_private_key = pkcs11_atca_get_private_key;
    driver_interface->get_crypto_config = pkcs11_atca_get_crypto_config;
    driver_interface->get_azrtos_private_key_type = pkcs11_atca_get_azrtos_private_key_type;

    if (ddim_interface) {
    	ddim_interface->get_bootstrap_cert = pkcs11_atca_get_bootstrap_cert;
        ddim_interface->generate_csr = pkcs11_atca_generate_csr;
        ddim_interface->sign_hash = pkcs11_atca_sign_hash;
        ddim_interface->extract_bootstrap_cn = pkcs11_atca_extract_botstrap_cn;
        ddim_interface->extract_operational_cn = pkcs11_atca_extract_operational_cn;
        ddim_interface->store_operational_cert = pkcs11_atca_store_operational_cert;
    }
	return 0;
}

#else
int pkcs11_atca_create_auth_driver(IotcAuthInterface* driver_interface, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext* context, struct pkcs11_atca_driver_parameters *driver_parameters) {
	printf("ATECC608: NetX 6.1.7 or newer is required\r\n");
	return -1;
}
#endif

int pkcs11_atca_release_auth_driver(IotcAuthInterfaceContext* context) {
	struct pkcs11_atca_driver_context* to_context = (struct pkcs11_atca_driver_context*) context;
	if (!is_context_valid(context)) {
		printf("ATECC608: invalid context\r\n");
		return -1; // not sure what to do here
	}
	to_context->magic = 0xdf; // to be able to detect a double free
	free(to_context);
	return 0;
}
