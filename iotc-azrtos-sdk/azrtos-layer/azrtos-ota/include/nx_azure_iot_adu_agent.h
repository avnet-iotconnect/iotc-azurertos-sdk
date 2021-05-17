/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/
// Copyright: Avnet 2021
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
// NOTE: This file is a port from ADU version of azure SDK and it is intended
// to provide temporary interface to shim the nx_azure_iot_adu_agent_stm32l4xx_driver.c
// until that interface is available in the official version.

#ifndef NX_AZURE_IOT_ADU_AGENT_H
#define NX_AZURE_IOT_ADU_AGENT_H

#ifdef __cplusplus
extern   "C" {
#endif

#include "tx_api.h"
#include "nx_azure_iot.h" // for NX_AZURE_IOT_SUCCESS and NX_AZURE_IOT_FAILURE so that the drver can port with same code


/* Define ADU driver constants.  */
#define NX_AZURE_IOT_ADU_AGENT_DRIVER_INITIALIZE                        0
#define NX_AZURE_IOT_ADU_AGENT_DRIVER_PREPROCESS                        1
#define NX_AZURE_IOT_ADU_AGENT_DRIVER_WRITE                             2
#define NX_AZURE_IOT_ADU_AGENT_DRIVER_INSTALL                           3
#define NX_AZURE_IOT_ADU_AGENT_DRIVER_APPLY                             4

/**
 * @brief ADU driver struct
 *
 */
typedef struct NX_AZURE_IOT_ADU_AGENT_DRIVER_STRUCT
{

    /* Define the driver command.  */
    UINT                                    nx_azure_iot_adu_agent_driver_command;

    /* Define the driver return status.  */
    UINT                                    nx_azure_iot_adu_agent_driver_status;

    /* Define the firmware size for the driver to preprocess.  */
    UINT                                    nx_azure_iot_adu_agent_driver_firmware_size;

    /* Define the firmware data for the driver to write.   */
    UINT                                    nx_azure_iot_adu_agent_driver_firmware_data_offset;
    UCHAR                                  *nx_azure_iot_adu_agent_driver_firmware_data_ptr;
    UINT                                    nx_azure_iot_adu_agent_driver_firmware_data_size;

    /* Define the return pointer for raw driver command requests.  */
    ULONG                                  *nx_azure_iot_adu_agent_driver_return_ptr;

} NX_AZURE_IOT_ADU_AGENT_DRIVER;

#ifdef __cplusplus
}
#endif
#endif /* NX_AZURE_IOT_ADU_AGENT_H */
