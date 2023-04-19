//
// Copyright: Avnet 2023
// Created by Nick Beck <nbeck@witekio.com>
//


#ifndef RX65N_TSIP_AUTH_DRIVER_H
#define RX65N_TSIP_AUTH_DRIVER_H

#ifdef __cplusplus
extern   "C" {
#endif

#include "iotc_auth_driver.h"

typedef enum {
    IOTC_AUTH_SUCCESS = 0,
	IOTC_AUTH_ERR_FAIL = -1,
	IOTC_AUTH_BAD_CONTEXT = -2,
	IOTC_AUTH_INVALID_PARAMETER = -3,
	IOTC_AUTH_NOT_IMPLEMENTED = -3,
} iotc_auth_err_t;

struct rx65n_tsip_driver_parameters {
	// No parameters for now
    int dummy;
};

int rx65n_tsip_create_auth_driver( //
		IotcAuthInterface* driver_interface, //
		IotcDdimInterface* ddim_interface, // Optional DDIM interface that can be used to dynamically obtain device certificates from IoTConnect
		IotcAuthInterfaceContext* context, // This context will be returned and can be used to access auth driver specific private data
		struct rx65n_tsip_driver_parameters *driver_parameters //
		);
int rx65n_tsip_release_auth_driver(IotcAuthInterfaceContext* context);


#ifdef __cplusplus
}
#endif

#endif // RX65N_TSIP_AUTH_DRIVER_H
