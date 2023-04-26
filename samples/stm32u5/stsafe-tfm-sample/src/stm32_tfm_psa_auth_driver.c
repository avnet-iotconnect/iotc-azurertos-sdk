/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/****************************************************************************************/
/*                                                                                      */
/* The following source code has been taken from 'sample_device_identity.c'             */
/* by Microsoft (https://github.com/azure-rtos) and modified by STMicroelectronics.     */
/* Main changes summary :                                                               */
/* - Modified to add an API instead of using extern variable declaration                */
/*                                                                                      */
/****************************************************************************************/


//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//


#include <stddef.h>
#include <stdint.h>

#include "tx_api.h"
#include "nx_api.h"
#include "app_azure_iot_config.h"
#include "nx_azure_iot_ciphersuites.h"
#include "device_identity.h"

#include "iotc_algorithms.h"
#include "azrtos_crypto_config.h"
#include "stm32_tfm_psa_auth_driver.h"


#if !defined(USE_TFM_X509_STORAGE) && !defined(USE_TFM_STSAFE_CERTIFICATE)
#error "USE_TFM_X509_STORAGE or USE_TFM_STSAFE_CERTIFICATE must be set to use stm32_tfm_psa_auth_driver!"
#endif

#ifndef NX_SECURE_X509_KEY_TYPE_HARDWARE
#error "Need NetX 6.1.7 or newer to compile stm32_tfm_psa_auth_driver!"
#endif


extern NX_CRYPTO_METHOD crypto_method_ecdsa_psa_crypto;

#define SR_BUFFER_SIZE TO_SIGNATURE_SIZE // two big integers (32 bytes each)
#define DC_MAGIC 0x4e

struct stm32_tfm_psa_driver_context {
    struct stm32_tfm_psa_driver_parameters driver_parameters;
    IotcAzccCryptoConfig crypto_config;
    const UCHAR *cert; // storage is in device identity
    UINT cert_size;
    const UCHAR *key;
    UINT key_size;
    char cn_buffer[IOTC_COMMON_NAME_MAX_LEN + 1];
    char magic;
};

static bool is_context_valid(IotcAuthInterfaceContext context) {
	struct stm32_tfm_psa_driver_context *to_context = (struct stm32_tfm_psa_driver_context*) context;
	bool ret = (DC_MAGIC == to_context->magic);
	if (!ret) {
		printf("TFM-PSA: Context is invalid\r\n");
	}
	return ret;
}

// Return a DER formatted certificate given the slot number
static int stm32_tfm_psa_get_cert(IotcAuthInterfaceContext context, uint8_t cert_slot, uint8_t **cert, size_t *cert_size) {
	*cert = NULL;
	*cert_size = 0;

	if (!is_context_valid(context)) return -1;
	struct stm32_tfm_psa_driver_context *stm32_tfm_psa_context = (struct stm32_tfm_psa_driver_context*) context;

    if (stm32_tfm_psa_context->cert) {
        // shortcut... skip loading again and avoid printing the cert twice
        *cert = (uint8_t*)stm32_tfm_psa_context->cert;
        *cert_size = stm32_tfm_psa_context->cert_size;
        return 0;
    }
	if (cert_slot != 0) {
		printf("TFM-PSA: get_cert: Slot must be 0 \r\n");
		return -2;
	}
	if (!cert || cert_size == NULL) {
		printf("TFM-PSA: get_cert: Invalid parameters for get_cert operation\r\n");
		return -3;
	}

	UINT status;
    if ((status = device_identity_retrieve_credentials(
    		&(stm32_tfm_psa_context->cert),
			&stm32_tfm_psa_context->cert_size,
			&(stm32_tfm_psa_context->key),
			&stm32_tfm_psa_context->key_size
			))) {
        printf("Failed to retrieve device identity: error code = 0x%08x\r\n", status);
        return(status);
    }

	*cert = (uint8_t*)stm32_tfm_psa_context->cert;
    *cert_size = stm32_tfm_psa_context->cert_size;

    return 0;
}


// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static int stm32_tfm_psa_get_operational_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
    return stm32_tfm_psa_get_cert(context, 0, cert, cert_size);
}
// Return a DER formatted certificate that will be used for dynamic identity
static int stm32_tfm_psa_get_bootstrap_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
    //TODO: Decide how to deal with multiple slots
	if (!is_context_valid(context)) return -1;
    return stm32_tfm_psa_get_cert(context, 0, cert, cert_size);
}

// In case of NX_SECURE_X509_KEY_TYPE_HARDWARE, the private key is ignored by AzureRTOS.
// However, the private key is passed in to to_ecdsa_operation, so that the context can be used in the callback
static int stm32_tfm_psa_get_private_key(IotcAuthInterfaceContext context, uint8_t** key, size_t* key_size) {
	if (!is_context_valid(context)) return -1;
	struct stm32_tfm_psa_driver_context *stm32_tfm_psa_context = (struct stm32_tfm_psa_driver_context*) context;
	*key = (uint8_t*)stm32_tfm_psa_context->key;
	*key_size = stm32_tfm_psa_context->key_size;
	return 0;
}

// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static unsigned int stm32_tfm_psa_get_azrtos_private_key_type(IotcAuthInterfaceContext context) {
	return NX_SECURE_X509_KEY_TYPE_HARDWARE;
}

static int stm32_tfm_psa_get_serial(IotcAuthInterfaceContext context, uint8_t *serial, size_t *size) {
    printf("TFM-PSA: stm32_tfm_psa_get_serial not implemented\r\n");
    return -1;
}

// Generate a CSR using the CSR slot defined in driver parameters and return the pointer to the CSR
// NOTE: The request will invalidate any cert or CSR previously stored in the cert buffer.
static int stm32_tfm_psa_generate_csr(IotcAuthInterfaceContext context, const char *cn, uint8_t **csr, size_t *len) {
    printf("TFM-PSA: stm32_tfm_psa_generate_csr not implemented\r\n");
	return -1;
}

static int stm32_tfm_psa_store_operational_cert(IotcAuthInterfaceContext context, uint8_t* cert, size_t cert_len) {
    printf("TFM-PSA: stm32_tfm_psa_store_operational_cert not implemented\r\n");
	return -1;
}

static int stm32_tfm_psa_sign_hash(IotcAuthInterfaceContext context, uint8_t* input_hash, uint8_t* output) {
    printf("TFM-PSA: stm32_tfm_psa_sign_hash not implemented\r\n");
	return -1;
}

static IotcAzccCryptoConfig* stm32_tfm_psa_get_crypto_config(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct stm32_tfm_psa_driver_context* stm32_tfm_psa_context = (struct stm32_tfm_psa_driver_context*) context;
	return &(stm32_tfm_psa_context->crypto_config);
}

static char* stm32_tfm_psa_extract_cn(IotcAuthInterfaceContext context, int cert_index) {
    
    (void)cert_index; //TODO: make use of index
	if (!is_context_valid(context)) return NULL;
	struct stm32_tfm_psa_driver_context* stm32_tfm_psa_context = (struct stm32_tfm_psa_driver_context*) context;

    NX_SECURE_X509_CERT dev_certificate;
    UINT nx_status;

    nx_status = nx_secure_x509_certificate_initialize(&dev_certificate,
                                (UCHAR *)stm32_tfm_psa_context->cert, (USHORT)stm32_tfm_psa_context->cert_size,
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
                stm32_tfm_psa_context->cn_buffer, 
                dev_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name, 
                cn_length);
        stm32_tfm_psa_context->cn_buffer[cn_length] = 0; // terminate the string
    } else {
        return NULL;
    }
    
	return stm32_tfm_psa_context->cn_buffer;
}

static char* stm32_tfm_psa_extract_operational_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	return stm32_tfm_psa_extract_cn(context, 0);
}

static char* stm32_tfm_psa_extract_botstrap_cn(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	return stm32_tfm_psa_extract_cn(context, 0);
}

int stm32_tfm_psa_create_auth_driver(IotcAuthInterface* driver_interface, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext* context, struct stm32_tfm_psa_driver_parameters *driver_parameters) {
	if (!driver_parameters) {
		printf("TFM-PSA: Driver parameters are required\r\n");
		return -1;
	}

	struct stm32_tfm_psa_driver_context *c = 
        (struct stm32_tfm_psa_driver_context*) malloc(sizeof(struct stm32_tfm_psa_driver_context));

	memset(c, 0, sizeof(struct stm32_tfm_psa_driver_context));

	c->magic = DC_MAGIC;
	if (!c) {
		printf("TFM-PSA: Unable to allocate context!\r\n");
		return -3;
	}
    memcpy(&(c->driver_parameters), driver_parameters, sizeof(struct stm32_tfm_psa_driver_parameters));

    if (!driver_interface || !context) {
    	printf("TFM-PSA: Incorrect parameters\r\n");
    	stm32_tfm_psa_release_auth_driver((IotcAuthInterfaceContext*)c);
    	return -4;
    }
    if (0 != iotcazcc_init_with_ec_curve_secp256(&c->crypto_config)) {
    	printf("TFM-PSA: failed to init the crypto subsystem\r\n");
    	stm32_tfm_psa_release_auth_driver((IotcAuthInterfaceContext*)c);
    	return -2;
    }

	c->crypto_config.custom_crypto_method_storage.nx_crypto_operation = crypto_method_ecdsa_psa_crypto.nx_crypto_operation;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_init = crypto_method_ecdsa_psa_crypto.nx_crypto_init;
	c->crypto_config.custom_crypto_method_storage.nx_crypto_cleanup = crypto_method_ecdsa_psa_crypto.nx_crypto_cleanup;

    *context = (IotcAuthInterfaceContext) c;

    driver_interface->get_serial = stm32_tfm_psa_get_serial;
    driver_interface->get_cert = stm32_tfm_psa_get_operational_cert;
    driver_interface->get_private_key = stm32_tfm_psa_get_private_key;
    driver_interface->get_crypto_config = stm32_tfm_psa_get_crypto_config;
    driver_interface->get_azrtos_private_key_type = stm32_tfm_psa_get_azrtos_private_key_type;

    if (ddim_interface) {
    	ddim_interface->get_bootstrap_cert = stm32_tfm_psa_get_bootstrap_cert;
        ddim_interface->generate_csr = stm32_tfm_psa_generate_csr;
        ddim_interface->sign_hash = stm32_tfm_psa_sign_hash;
        ddim_interface->extract_bootstrap_cn = stm32_tfm_psa_extract_botstrap_cn;
        ddim_interface->extract_operational_cn = stm32_tfm_psa_extract_operational_cn;
        ddim_interface->store_operational_cert = stm32_tfm_psa_store_operational_cert;
    }
	return 0;
}

int stm32_tfm_psa_release_auth_driver(IotcAuthInterfaceContext* context) {
	struct stm32_tfm_psa_driver_context* to_context = (struct stm32_tfm_psa_driver_context*) context;
	if (!is_context_valid(context)) {
		printf("TFM-PSA: invalid context\r\n");
		return -1; // not sure what to do here
	}
	to_context->magic = 0xdf; // to be able to detect a double free
	free(to_context);
	return 0;
}

