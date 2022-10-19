//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//

#ifndef AZRTOS_CRYPTO_CONFIG_H
#define AZRTOS_CRYPTO_CONFIG_H


#include <nx_crypto.h>

#ifdef __cplusplus
extern   "C" {
#endif

#ifndef IOTCAZCC_MAX_CRYPTO_METHODS
#define IOTCAZCC_MAX_CRYPTO_METHODS 12
#endif

#ifndef IOTCAZCC_MAX_CIPHERSUITES
#define IOTCAZCC_MAX_CIPHERSUITES 4
#endif

#define IOTCAZCC_ERR_INVALID_ARGUMENT   0x3400
#define IOTCAZCC_ERR_OUT_OF_BOUNDS 	    0x3401
#define IOTCAZCC_ERR_OVERFLOW           0x3403


/* Define some supported crypto methods. */
extern NX_CRYPTO_METHOD crypto_method_hmac;
extern NX_CRYPTO_METHOD crypto_method_hmac_sha256;
extern NX_CRYPTO_METHOD crypto_method_tls_prf_sha256;
extern NX_CRYPTO_METHOD crypto_method_sha256;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_128;
extern NX_CRYPTO_METHOD crypto_method_rsa;
extern NX_CRYPTO_METHOD crypto_method_ecdhe;
extern NX_CRYPTO_METHOD crypto_method_ecdsa;
extern NX_CRYPTO_METHOD crypto_method_ec_secp256;
extern NX_CRYPTO_METHOD crypto_method_ec_secp384;

typedef struct {
	const NX_CRYPTO_METHOD* crypto_methods[IOTCAZCC_MAX_CRYPTO_METHODS];
	size_t crypto_methods_length;
	const NX_CRYPTO_CIPHERSUITE* tls_ciphersuites[IOTCAZCC_MAX_CIPHERSUITES];
	size_t tls_ciphersuites_length;
	// A driver can intercept these functions
	// instead of having to deal have their own storage
	NX_CRYPTO_METHOD custom_crypto_method_storage;
} IotcAzccCryptoConfig;

int iotcazcc_crypto_init(IotcAzccCryptoConfig *crypto, UINT  key_type);
int iotcazcc_crypto_init_with_rsa_defaults(IotcAzccCryptoConfig *crypto);
int iotcazcc_crypto_init_with_ec_defaults(IotcAzccCryptoConfig *crypto);

int iotcazcc_register_crypto_method(IotcAzccCryptoConfig* crypto, NX_CRYPTO_METHOD* method);

// HELPERS / EXAMPLES:
int iotcazcc_init_with_ec_curve_secp256(IotcAzccCryptoConfig* crypto);
int iotcazcc_init_with_ec_curve_secp384(IotcAzccCryptoConfig* crypto);
//


#ifdef __cplusplus
}
#endif

#endif // AZRTOS_CRYPTO_CONFIG_H
