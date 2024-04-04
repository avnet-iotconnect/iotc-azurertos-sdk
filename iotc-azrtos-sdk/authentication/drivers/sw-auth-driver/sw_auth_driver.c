//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//

#include "iotc_auth_driver.h"
#include <stddef.h>
#include <stdbool.h>
#include "nx_secure_x509.h"
#include "sw_auth_driver.h"
#include "azrtos_crypto_config.h"

#define DC_MAGIC 0x5f

struct sw_der_driver_context {
    struct sw_der_driver_parameters driver_parameters;
    IotcAzccCryptoConfig crypto_config;
    char magic;
};

static bool is_context_valid(IotcAuthInterfaceContext context) {
	struct sw_der_driver_context *c = (struct sw_der_driver_context*) context;
	bool ret = (DC_MAGIC == c->magic);
	if (!ret) {
		printf("SW Auth Driver: Context is invalid\r\n");
	}
	return ret;
}



// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static int sw_get_cert(IotcAuthInterfaceContext context, uint8_t **cert, size_t *cert_size) {
	if (!is_context_valid(context)) return -1;
	struct sw_der_driver_context* c = (struct sw_der_driver_context*) context;
	*cert = c->driver_parameters.cert;
	*cert_size = c->driver_parameters.cert_size;
	return 0;
}

// In case of NX_SECURE_X509_KEY_TYPE_HARDWARE, the private key is ignored by AzureRTOS.
// However, the private key is passed in to to_ecdsa_operation, so that the context can be used in the callback
static int sw_get_private_key(IotcAuthInterfaceContext context, uint8_t** key, size_t* key_size) {
	if (!is_context_valid(context)) return -1;
	struct sw_der_driver_context* c = (struct sw_der_driver_context*) context;
	*key = c->driver_parameters.key;
	*key_size = c->driver_parameters.key_size;
	return 0;
}

// Return a DER formatted certificate that will be used to authenticate the IoTConnect connection
static unsigned int sw_get_azrtos_private_key_type(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NX_SECURE_X509_KEY_TYPE_NONE;
	struct sw_der_driver_context* c = (struct sw_der_driver_context*) context;

	return c->driver_parameters.crypto_method->nx_crypto_algorithm ==  NX_CRYPTO_KEY_EXCHANGE_RSA ?
			NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER : NX_SECURE_X509_KEY_TYPE_EC_DER;
}

static int get_serial_not_implemented(IotcAuthInterfaceContext context, uint8_t *serial, size_t *size) {
	(void) context; // unused
	(void) serial; // unused
	(void) size; // unused
	return -1;
}

static IotcAzccCryptoConfig* sw_get_crypto_config(IotcAuthInterfaceContext context) {
	if (!is_context_valid(context)) return NULL;
	struct sw_der_driver_context* c = (struct sw_der_driver_context*) context;
	return &(c->crypto_config);
}

int create_sw_der_auth_driver(IotcAuthInterface* driver_interface, IotcAuthInterfaceContext* context, struct sw_der_driver_parameters* driver_parameters) {
	struct sw_der_driver_context *c = (struct sw_der_driver_context*) malloc(sizeof(struct sw_der_driver_context));
	if (!c) {
		printf("create_sw_der_auth_driver: Unable to allocate context!\r\n");
		return -3;
	}
	c->magic = DC_MAGIC;
    memcpy(&(c->driver_parameters), driver_parameters, sizeof(struct sw_der_driver_parameters));

    if (driver_parameters->crypto_method->nx_crypto_algorithm ==  NX_CRYPTO_KEY_EXCHANGE_RSA) {
		if (0 != iotcazcc_crypto_init_with_rsa_defaults(&c->crypto_config)) {
			printf("create_sw_der_auth_driver failed to init the crypto subsystem with RSA defaults\r\n");
			release_sw_der_auth_driver((IotcAuthInterfaceContext) c);
			return -2;
		}
    } else  {
		if (0 != iotcazcc_crypto_init_with_ec_defaults(&c->crypto_config)) {
			printf("create_sw_der_auth_driver failed to init the crypto subsystem with eC defaults\r\n");
			release_sw_der_auth_driver((IotcAuthInterfaceContext) c);
			return -2;
		}
		if (0 != iotcazcc_register_crypto_method(&c->crypto_config, driver_parameters->crypto_method)) {
			printf("create_sw_der_auth_driver failed to register EC curve\r\n");
			release_sw_der_auth_driver((IotcAuthInterfaceContext) c);
			return -3;
		}

    }

    *context = (IotcAuthInterfaceContext) c;

    driver_interface->get_serial = get_serial_not_implemented;
    driver_interface->get_cert = sw_get_cert;
    driver_interface->get_private_key = sw_get_private_key;
    driver_interface->get_crypto_config = sw_get_crypto_config;
    driver_interface->get_azrtos_private_key_type = sw_get_azrtos_private_key_type;
    return 0;
}

int release_sw_der_auth_driver(IotcAuthInterfaceContext* context) {
	if (!is_context_valid(context)) return -1;
	struct sw_der_driver_context *c = (struct sw_der_driver_context *) context;
	c->magic = (char) 0xdf; // to be able to detect a double free
	free(c);
	return 0;
}
