//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//


#ifndef IOTC_AUTH_DRIVER_H
#define IOTC_AUTH_DRIVER_H

#include <stdint.h>
#include <stddef.h>
#include "azrtos_crypto_config.h"

#ifdef __cplusplus
extern   "C" {
#endif

// useful defines:
#define IOTC_COMMON_NAME_MAX_LEN        64
#define IOTC_256_BIT_KEY_SIZE           32 // size of a 256-bit private key in bytes
#define IOTC_256_BIT_SIGNATURE_SIZE     (32 * 2) // two 256-bit intergers
#define IOTC_SHA256_HASH_SIZE           32

typedef void * IotcAuthInterfaceContext;

typedef IotcAzccCryptoConfig* (*IotcDriverGetCryptoConfig)(IotcAuthInterfaceContext);

// Pass this cert to nx_secure_x509_certificate_initialize or use it for further processing when using the auth interface.
// The same signature can be used to get bootstrap certificate for DDIM
// Returns a DER formatted certificate that will be used to authenticate the IoTConnect connection
typedef int (*IotcDriverGetCert)( IotcAuthInterfaceContext context, uint8_t** cert, size_t* cert_size);

// Pass this key to nx_secure_x509_certificate_initialize
// For software auth drivers: Returns a DER formatted private that will be used to authenticate the IoTConnect connection.
// For hardware auth drivers: Returns arbitrary data the will be passed in as "private key" to AzureRTOS and later used in callbacks.
typedef int (*IotcDriverGetPrivateKey)( IotcAuthInterfaceContext context, uint8_t** key, size_t* key_size);

// Return serial number of the MCU in case of software authentication or secure element serial in case of hardware authentication.
typedef int (*IotcDriverGetSerial)(IotcAuthInterfaceContext context, uint8_t* serial, size_t* size);

/* See nx_secure_x509.h
#define NX_SECURE_X509_KEY_TYPE_NONE                              0x00000000 Default value for no key.
#define NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER                     0x00000001 DER-encoded PKCS-1 RSA private key.
#define NX_SECURE_X509_KEY_TYPE_EC_DER                            0x00000002 DER-encoded EC private key.
#define NX_SECURE_X509_KEY_TYPE_HARDWARE                          0x00000003 Hardware private key
 */
typedef unsigned int (*IotcDriverGetKeyType)(IotcAuthInterfaceContext context);


typedef char *(*IotcDriverExtractCN)(IotcAuthInterfaceContext context);

// Generate a CSR an internally specified driver-specified bootstrap identity slot
// NOTE: The request may invalidate any previous cert or CSR requested.
typedef int (*IotcDriverGenerateCsr)(IotcAuthInterfaceContext context, const char *cn, uint8_t **csr, size_t *csr_len);

// Sign the given hash with bootstrap certificate's private key
typedef int (*IotcDriverSignHash)(IotcAuthInterfaceContext context, uint8_t* sha256_hash, uint8_t* output);

// Store a new operational certificate
typedef int (*IotcDriverStoreOperationalCert)(IotcAuthInterfaceContext context, uint8_t* cert, size_t cert_len);

// Dynamic Device Identity Management driver interface - special set of function that may not be supported by software drivers.
typedef struct  {
	IotcDriverGetCert get_bootstrap_cert;
	IotcDriverGenerateCsr generate_csr;
	IotcDriverSignHash sign_hash;
	IotcDriverExtractCN extract_operational_cn;
	IotcDriverExtractCN extract_bootstrap_cn;
	IotcDriverStoreOperationalCert store_operational_cert;
} IotcDdimInterface;

typedef struct  {
	IotcDriverGetCryptoConfig get_crypto_config;
	IotcDriverGetSerial get_serial;
	IotcDriverGetCert get_cert;
	IotcDriverGetPrivateKey get_private_key;
	IotcDriverGetKeyType get_azrtos_private_key_type;
} IotcAuthInterface;

#ifdef __cplusplus
}
#endif

#endif // IOTC_AUTH_DRIVER_H

