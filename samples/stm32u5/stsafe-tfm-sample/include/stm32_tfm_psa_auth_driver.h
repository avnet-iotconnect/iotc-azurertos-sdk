//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 11/11/21.
//


#ifndef STM32_TFM_PSA_AUTH_DRIVER_H
#define STM32_TFM_PSA_AUTH_DRIVER_H

#ifdef _cplusplus
extern   "C" {
#endif

#include "iotc_auth_driver.h"

struct stm32_tfm_psa_driver_parameters {
	// TODO: No parameters for now
    int dummy;
};

int stm32_tfm_psa_create_auth_driver( //
		IotcAuthInterface* driver_interface, //
		IotcDdimInterface* ddim_interface, // Optional DDIM interface that can be used to dynamically obtain device certificates from IoTConnect
		IotcAuthInterfaceContext* context, // This context will be returned and can be used to
		struct stm32_tfm_psa_driver_parameters *driver_parameters //
		);
int stm32_tfm_psa_release_auth_driver(IotcAuthInterfaceContext* context);


#ifdef _cplusplus
}
#endif

#endif // MCHP_ATECC609_PKCS11_AUTH_DRIVER_H
