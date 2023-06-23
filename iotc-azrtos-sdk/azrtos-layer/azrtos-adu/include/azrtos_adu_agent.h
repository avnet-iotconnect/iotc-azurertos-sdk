//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 2/7/23.
//

#ifndef AZRTOS_ADU_H
#define AZRTOS_ADU_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "tx_api.h"

#define IOTC_ADU_MICROCHIP "MICROCHIP"
#define IOTC_ADU_SAME54 "SAME54"
#define IOTC_ADU_STM "STMicroelectronics"
#define IOTC_ADU_STM32L4S5 "STM32L4S5"

// in case of U5 we can use the board name compatible with X-Cube-Azures's board name defined in the project
#define IOTC_ADU_STM32U5 "B-U585I-IOT02A"

UINT iothub_start_device_agent(
    const char *manufacturer,
    const char *model,
    const char *provider, // provider and name are ignored for NetX 6.2.0 and newer
    const char *name, // provider and name are ignored for NetX 6.2.0 and newer
    const char *version
    );


UINT iothub_stop_device_agent(void);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_ADU_H
