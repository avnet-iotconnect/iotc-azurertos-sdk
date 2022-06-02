//
// Copyright: Avnet, Softweb Inc. 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/15/22.
//

#ifndef IOTCONNECT_DI_H
#define IOTCONNECT_DI_H

#include <stddef.h>
#include "iotconnect.h"
#include "iotc_auth_driver.h"
#include "iotconnect_device_identity.h"

#ifdef __cplusplus
extern "C" {
#endif

int iotcdi_obtain_operational_identity(IotConnectAzrtosConfig* azrtos_config, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext auth_interface_context, const char* env);

#ifdef __cplusplus
}
#endif

#endif
