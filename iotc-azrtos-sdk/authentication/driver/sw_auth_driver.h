//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/24.
//


#ifndef SW_AUTH_DRIVER_H
#define SW_AUTH_DRIVER_H

#include <iotc_auth_driver.h>
#include <stddef.h>

#ifdef __cplusplus
extern   "C" {
#endif


struct sw_der_driver_parameters {
	uint8_t * cert; // in DER format with storage that exists until release_*() is called - additional storage space will not be allocated
	size_t cert_size;
	uint8_t * key;  // in DER format with storage that exists until release_*() is called - additional storage space will not be allocated
	size_t key_size;
	// see nx_crypto_methods.c. You can use an appropriate extern and pass it to this function
	// For RSA keys, use crypto_method_rsa
	// For EC keys, use the appropriate curve method like crypto_method_ec_secp384
	// EC Curve can be found in your certificate, in the NIST CURVE field (for example NIST CURVE: P-384)
	NX_CRYPTO_METHOD* crypto_method;
};

int create_sw_der_auth_driver(IotcAuthInterface* driver_interface, IotcAuthInterfaceContext* context, struct sw_der_driver_parameters* driver_parameters);
int release_sw_der_auth_driver(IotcAuthInterfaceContext* context);

#ifdef __cplusplus
}
#endif

#endif // IOTC_AUTH_INTERFACE_H
