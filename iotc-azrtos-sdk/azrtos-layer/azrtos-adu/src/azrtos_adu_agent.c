/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************///
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 2/7/23.
//

#ifdef IOTC_ENABLE_ADU_SUPPORT

#include <string.h>
#include <stdio.h>
#include "tx_api.h"
#include "nx_api.h"

#include "azrtos_iothub_client.h"
#include "azrtos_adu_agent.h"
#include "nx_azure_iot_adu_agent.h"

// starting with NetXDuo 6.2.0 the ADU interface has changed
#define NETXDUO_VERCALC (NETXDUO_MAJOR_VERSION * 10000 + NETXDUO_MINOR_VERSION * 100 + NETXDUO_PATCH_VERSION)
#if (NETXDUO_VERCALC >= 60200)
#ifndef IOTC_USE_ADU_API_620
#define IOTC_USE_ADU_API_620 1
#endif
#endif

static NX_AZURE_IOT_ADU_AGENT adu_agent;
static bool is_adu_agent_started = false;

extern void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr);

static void adu_agent_update_notify(NX_AZURE_IOT_ADU_AGENT *adu_agent_ptr,
                                    UINT update_state,
                                    UCHAR *provider, UINT provider_length,
                                    UCHAR *name, UINT name_length,
                                    UCHAR *version, UINT version_length) {
    if (update_state == NX_AZURE_IOT_ADU_AGENT_UPDATE_RECEIVED) {
        printf("Received Device Update: Provider: %.*s; Name: %.*s, Version: %.*s\r\n",
               provider_length, provider, name_length, name, version_length, version);
        /* Start to download and install update immediately for testing.  */
#if ( IOTC_USE_ADU_API_620 == 1)
        nx_azure_iot_adu_agent_update_download_and_install(adu_agent_ptr);
#else
        nx_azure_iot_adu_agent_update_download_install(adu_agent_ptr);
#endif
    } else if(update_state == NX_AZURE_IOT_ADU_AGENT_UPDATE_INSTALLED) {
        /* Start to apply update immediately for testing.  */
        nx_azure_iot_adu_agent_update_apply(adu_agent_ptr);
    }
}

UINT iothub_start_device_agent(
    const char *manufacturer,
    const char *model,
    const char *provider,
    const char *name,
    const char *version
    ) {
    UINT status;
    NX_AZURE_IOT_HUB_CLIENT* iothub_client = iothub_client_internal_get_iothub_instance();

    if (!manufacturer || !model || !provider || !name || !version) {
        printf("Error: iothub_start_device_agent missing required argument!\r\n");
        return 1;
    }

    if (!is_adu_agent_started) {
    	UINT status;
#if ( IOTC_USE_ADU_API_620 == 1)
    	status = nx_azure_iot_adu_agent_start(&adu_agent,
    	            iothub_client,
    	            (const UCHAR *)manufacturer, strlen(manufacturer),
    	            (const UCHAR *)model, strlen(model),
    	            (const UCHAR *)version, strlen(version),
    	             adu_agent_update_notify,
    	             nx_azure_iot_adu_agent_driver
    	);
    	(void) provider;
    	(void) name;
#else
    	status = nx_azure_iot_adu_agent_start(&adu_agent,
    	            iothub_client,
    	            (const UCHAR *)manufacturer, strlen(manufacturer),
    	            (const UCHAR *)model, strlen(model),
    	            (const UCHAR *)provider, strlen(provider),
    	            (const UCHAR *)name, strlen(name),
    	            (const UCHAR *)version, strlen(version),
    	             adu_agent_update_notify,
    	             nx_azure_iot_adu_agent_driver
    	);
#endif
        if (status) {
                printf("Failed on nx_azure_iot_adu_agent_start! Error: 0x%x\r\n", status);
                return 2;
        }
        is_adu_agent_started = true;
    }
    /* Request all properties so ADU agent can do the work */
    if ((status = nx_azure_iot_hub_client_properties_request(iothub_client, NX_WAIT_FOREVER))) {
        printf("Properties request failed!: error code = 0x%08x\r\n", status);
        return 3;
    } else {
        printf("Sent properties request.\r\n");
    }
    return 0;
}

UINT iothub_stop_device_agent(void)
{
    UINT status = nx_azure_iot_adu_agent_stop(&adu_agent);
    return status;
}
    
#endif // IOTC_ENABLE_ADU_SUPPORT
