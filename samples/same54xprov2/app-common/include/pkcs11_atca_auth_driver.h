//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 11/11/21.
//


#ifndef PKCS11_ATCA_AUTH_DRIVER_H
#define PKCS11_ATCA_AUTH_DRIVER_H

#ifdef __cplusplus
extern   "C" {
#endif

#include "iotc_auth_driver.h"

struct pkcs11_atca_driver_parameters {
	// TODO: No parameters for now
    int dummy;
};

int pkcs11_atca_create_auth_driver( //
		IotcAuthInterface* driver_interface, //
		IotcDdimInterface* ddim_interface, // Optional DDIM interface that can be used to dynamically obtain device certificates from IoTConnect
		IotcAuthInterfaceContext* context, // This context will be returned and can be used to
		struct pkcs11_atca_driver_parameters *driver_parameters //
		);
int pkcs11_atca_release_auth_driver(IotcAuthInterfaceContext* context);


#ifdef __cplusplus
}
#endif

#endif // MCHP_ATECC609_PKCS11_AUTH_DRIVER_H
