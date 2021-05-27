//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/27/21.
//

#ifndef AZRTOS_OTA_FW_CLIENT_H
#define AZRTOS_OTA_FW_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif


#include "azrtos_download_client.h"
#include "nx_azure_iot_adu_agent.h"

// Function that will handle download operations per Microsoft ADU driver specification
// As of May 2021, prototype ADU driver implementations
// are available at https://github.com/azure-rtos/samples/tree/PublicPreview/ADU
typedef void (*IotConnectAduDriver) (NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req);

// The download client will implement default event_callback for the download client
// and if the user passes a callback, it will route requests to the user's callback AFTER
// it is done with ADU driver operations.
// If this function returns NX_SUCCESS, the user must either iotc_ota_fw_apply() or iotc_ota_fw_cancel()
// before attempting a new OTA download

UINT iotc_ota_fw_download(
    IotConnectHttpRequest *r,
    IotConnectAduDriver adu_driver,
    bool resume,
    IotConnectDownloadHandler event_callback
);

// Apply the downloaded firmware and reboot.
// Call this only in case the previous iotc_ota_fw_download() returned NX_SUCCESS
UINT iotc_ota_fw_apply();

// Cancels firmware application and resets the client so that a new download can be tried,
// Call this only in case the previous iotc_ota_fw_download() returned NX_SUCCESS
UINT iotc_ota_fw_cancel();

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_OTA_FW_CLIENT_H
