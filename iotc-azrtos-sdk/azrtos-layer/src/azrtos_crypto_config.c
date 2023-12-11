//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <azrtos_crypto_config.h>

#ifdef IOTC_USE_PSA_CIPHERS
    #include "nx_crypto_aes_psa.h"
    #include "nx_crypto_rsa_psa.h"
    #include "nx_crypto_ecdsa_psa_crypto.h"
#endif /* ENABLE_PSA_CRYPTO_CIPHERSUITES */

/* TLS ciphersuites. */
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_tls_ecdhe_rsa_with_aes_128_cbc_sha256;
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_tls_rsa_with_aes_128_cbc_sha256;

/* X.509 ciphersuites.  */
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_rsa_sha_256;
extern const NX_CRYPTO_CIPHERSUITE nx_crypto_x509_ecdsa_sha_256;


#ifdef IOTC_USE_PSA_CIPHERS
extern NX_CRYPTO_METHOD crypto_method_ecdsa_psa_crypto;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_128_psa;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_192_psa;
extern NX_CRYPTO_METHOD crypto_method_aes_cbc_256_psa;
extern NX_CRYPTO_METHOD crypto_method_sha256_psa;
extern NX_CRYPTO_METHOD crypto_method_rsa_psa;
#endif /* ENABLE_PSA_CRYPTO_CIPHERSUITES */


// Always add these crypto methods
const NX_CRYPTO_METHOD* default_nx_crypto_methods[] =
{
    &crypto_method_hmac,
    &crypto_method_hmac_sha256,
    &crypto_method_tls_prf_sha256,
#ifdef IOTC_USE_PSA_CIPHERS
    &crypto_method_sha256,
#else
    &crypto_method_sha256,
    &crypto_method_aes_cbc_128,
#endif /* IOTC_USE_PSA_CIPHERS */
    &crypto_method_rsa
};

int iotcazcc_register_crypto_method(IotcAzccCryptoConfig* crypto, NX_CRYPTO_METHOD* method) {
	if (NULL == crypto || NULL == method) {
		return IOTCAZCC_ERR_INVALID_ARGUMENT;
	}
	if (crypto->crypto_methods_length >= IOTCAZCC_MAX_CRYPTO_METHODS) {
		return IOTCAZCC_ERR_OVERFLOW;
	}
	crypto->crypto_methods[crypto->crypto_methods_length] = method;
	crypto->crypto_methods_length++;
	return 0;
}

int iotcazcc_crypto_init(IotcAzccCryptoConfig *crypto, UINT key_type) {
        (void) key_type; // unused
	memset(crypto, 0, sizeof(IotcAzccCryptoConfig));
	return 0;
}

int iotcazcc_crypto_init_with_rsa_defaults(IotcAzccCryptoConfig *crypto) {
	memcpy(crypto->crypto_methods, default_nx_crypto_methods, sizeof(default_nx_crypto_methods));
	crypto->crypto_methods_length = sizeof(default_nx_crypto_methods) /  sizeof(NX_CRYPTO_METHOD *);
	crypto->tls_ciphersuites[0] = &nx_crypto_tls_rsa_with_aes_128_cbc_sha256;
	crypto->tls_ciphersuites[1] = &nx_crypto_x509_rsa_sha_256;
	crypto->tls_ciphersuites_length = 2;
	memset(&crypto->custom_crypto_method_storage, 0, sizeof(NX_CRYPTO_METHOD));
	return 0;

}

int iotcazcc_crypto_init_with_ec_defaults(IotcAzccCryptoConfig *crypto){
	memcpy(crypto->crypto_methods, default_nx_crypto_methods, sizeof(default_nx_crypto_methods));
	crypto->crypto_methods_length = sizeof(default_nx_crypto_methods) /  sizeof(NX_CRYPTO_METHOD *);
	crypto->tls_ciphersuites[0] = &nx_crypto_tls_ecdhe_rsa_with_aes_128_cbc_sha256; // key signing
	crypto->tls_ciphersuites[1] = &nx_crypto_x509_rsa_sha_256; //  verifiation remote certificate
	crypto->tls_ciphersuites[2] = &nx_crypto_x509_ecdsa_sha_256; // identity certific
	crypto->tls_ciphersuites_length = 3;
	iotcazcc_register_crypto_method(crypto, &crypto_method_ecdhe);

	// pretend that NX_CRYPTO_METHOD is const, but it is not. Hardware secure element should override ecdsa_crypto methods
	// const during IoTHub Client operation
#ifdef IOTC_USE_PSA_CIPHERS
	memcpy(&(crypto->custom_crypto_method_storage), &crypto_method_ecdsa_psa_crypto, sizeof(NX_CRYPTO_METHOD));
#else
	memcpy(&(crypto->custom_crypto_method_storage), &crypto_method_ecdsa, sizeof(NX_CRYPTO_METHOD));
#endif /* IOTC_USE_PSA_CIPHERS */
	iotcazcc_register_crypto_method(crypto, &(crypto->custom_crypto_method_storage));
	return 0;

}

// Registers a custom ECC cuvrve or generic crypto method.
// see nx_crypto_methods.c in NetX Duo.
// For your custom method, declare it as extern and call this function

int iotcazcc_register_crypto_sw_method_rsa(IotcAzccCryptoConfig* crypto) {
	// NOOP:  return iotcazcc_register_crypto_method(crypto, &crypto_method_ec_secp256);
        (void) crypto; // unused
	return 0;
}

int iotcazcc_init_with_ec_curve_secp256(IotcAzccCryptoConfig* crypto) {
	int ret = iotcazcc_crypto_init_with_ec_defaults(crypto);
	if (0 != ret) {
		return ret;
	}
	return iotcazcc_register_crypto_method(crypto, &crypto_method_ec_secp256);
}

int iotcazcc_init_with_ec_curve_secp384(IotcAzccCryptoConfig* crypto) {
	int ret = iotcazcc_crypto_init_with_ec_defaults(crypto);\
	if (0 != ret) {
		return ret;
	}
	return iotcazcc_register_crypto_method(crypto, &crypto_method_ec_secp384);
}

