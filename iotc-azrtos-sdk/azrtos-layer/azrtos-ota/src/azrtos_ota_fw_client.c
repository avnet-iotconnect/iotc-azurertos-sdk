//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/27/21.
//

#include "azrtos_ota_fw_client.h"

static IotConnectAduDriver adu_driver = NULL; // initialize so that first check works properly
static IotConnectDownloadHandler user_download_cb;
static NX_AZURE_IOT_ADU_AGENT_DRIVER adudr; // last driver request
static bool firmware_ready = false;

bool ota_fw_download_handler (IotConnectDownloadEvent* event) {
    bool ret = true;
    switch (event->type) {
    case IOTC_DL_STATUS:
        // this event is not useful as the return from download is better at handling it
        break;
    case IOTC_DL_FILE_SIZE:
        memset(&adudr,0, sizeof(adudr));
        adudr.nx_azure_iot_adu_agent_driver_command = NX_AZURE_IOT_ADU_AGENT_DRIVER_PREPROCESS;
        adudr.nx_azure_iot_adu_agent_driver_firmware_size = event->file_size;
        adu_driver(&adudr);
        if (adudr.nx_azure_iot_adu_agent_driver_status) {
            printf("OTA: Error: ADU driver failed to preprocess with code: 0x%x\r\n", adudr.nx_azure_iot_adu_agent_driver_status);
            // abort the download and let iotc_ota_fw_download return abort status;
            return false;
        }
        break;
    case IOTC_DL_DATA:
        memset(&adudr,0, sizeof(adudr));
        adudr.nx_azure_iot_adu_agent_driver_command = NX_AZURE_IOT_ADU_AGENT_DRIVER_WRITE;
        adudr.nx_azure_iot_adu_agent_driver_firmware_data_ptr = (UCHAR*) event->data.data_ptr;
        adudr.nx_azure_iot_adu_agent_driver_firmware_data_size = event->data.data_size;
        adudr.nx_azure_iot_adu_agent_driver_firmware_data_offset = event->data.offset;
        adu_driver(&adudr);
        if (adudr.nx_azure_iot_adu_agent_driver_status) {
            printf("OTA: Error: ADU driver failed to write with code: 0x%x\r\n", adudr.nx_azure_iot_adu_agent_driver_status);
            // abort the download and let iotc_ota_fw_download return abort status;
            return false;
        }
        break;
    default:
        printf("OTA: Warning: Unknown event type %d received from download client!\r\n", event->type);
        // we should not have to abort.. Perhaps code mismatch?
        // Probably also pass down to the user...
    }
//    tx_thread_preemption_change(tx_thread_identify(), old_threshold, &old_threshold);
    if (user_download_cb) {
        ret = user_download_cb(event);
    }
    return ret;
}


UINT iotc_ota_fw_download(
    IotConnectHttpRequest *r,
    IotConnectAduDriver ad,
    bool resume,
    IotConnectDownloadHandler user_dl_callback
) {
    IotConnectDownloadEvent evt;

    firmware_ready = false;

    evt.type = IOTC_DL_STATUS;
    if (firmware_ready) {
        printf("OTA: Error: Firmware is ready. Must call iotc_ota_fw_cancel() before attempting a new download!\r\n");
        evt.status = NX_INVALID_PARAMETERS;
        if (user_dl_callback) {
            user_dl_callback(&evt);
        }
        return evt.status;
    }
    if (!ad) {
        printf("OTA: Invalid arguments\r\n");
        evt.status = NX_INVALID_PARAMETERS;
        if (user_dl_callback) {
            user_dl_callback(&evt);
        }
        return evt.status;
    }
    //NOTE: Global assignment. Can only have one download running at the time.
    if (adu_driver) {
        printf("OTA: Error: A download is already in progress!\r\n");
        evt.status = NX_NOT_SUPPORTED; // unable to support multiple downloads
        if (user_dl_callback) {
            user_dl_callback(&evt);
        }
        return evt.status;
    }

    memset(&adudr,0, sizeof(adudr));
    adudr.nx_azure_iot_adu_agent_driver_command = NX_AZURE_IOT_ADU_AGENT_DRIVER_INITIALIZE;
    ad(&adudr);
    if (adudr.nx_azure_iot_adu_agent_driver_status) {
        printf("OTA: Error: Failed to initialize the driver. Error was: error: 0x%x\r\n", adudr.nx_azure_iot_adu_agent_driver_status);
        return adudr.nx_azure_iot_adu_agent_driver_status;
    }

    adu_driver = ad;
    user_download_cb = user_dl_callback;

    UINT status = iotc_download(r, ota_fw_download_handler, resume);
    if (NX_SUCCESS == status && adudr.nx_azure_iot_adu_agent_driver_status == NX_SUCCESS) {
        // don't reset the driver pointer. We will need to install()
        firmware_ready = true;
        return status;
    }
    adu_driver = NULL;
    user_download_cb = NULL;
    return status;
}

UINT iotc_ota_fw_apply() {
    if (!adu_driver) {
        printf("OTA: Error: Invalid usage of iotc_ota_fw_install().\r\n");
        return NX_INVALID_PARAMETERS;
    }
    if (!firmware_ready) {
        printf("OTA: Error: Invalid usage of iotc_ota_fw_install(). Firmware is not ready.\r\n");
        return NX_INVALID_PARAMETERS;
    }
    memset(&adudr,0, sizeof(adudr));
    adudr.nx_azure_iot_adu_agent_driver_command = NX_AZURE_IOT_ADU_AGENT_DRIVER_INSTALL;
    adu_driver(&adudr);
    adu_driver(&adudr);
    if (adudr.nx_azure_iot_adu_agent_driver_status) {
        printf("OTA: Error: Failed to install firmware. Error was: error: 0x%x\r\n", adudr.nx_azure_iot_adu_agent_driver_status);
        // This should not really happen. In effect the download needs to be cancelled
        adu_driver = NULL;
        firmware_ready = false;
    } else {
        memset(&adudr,0, sizeof(adudr));
        printf("OTA: Applying firmware. Resetting the board.\r\n");
        adudr.nx_azure_iot_adu_agent_driver_command = NX_AZURE_IOT_ADU_AGENT_DRIVER_APPLY;
        adu_driver(&adudr);
        if (adudr.nx_azure_iot_adu_agent_driver_status) {
            printf("OTA: Error: Failed to apply firmware. Error was: error: 0x%x\r\n", adudr.nx_azure_iot_adu_agent_driver_status);
        } else {
            // else we area already in reset!
            ;
        }
        adu_driver = NULL;
        firmware_ready = false;
    }
    return adudr.nx_azure_iot_adu_agent_driver_status;
}

UINT iotc_ota_fw_cancel() {
    if (!firmware_ready) {
        printf("OTA: Error: Invalid usage of iotc_ota_fw_cancel(). Firmware is not ready.\r\n");
        return NX_INVALID_PARAMETERS;
    }
    adu_driver = NULL;
    firmware_ready = false;
    return NX_SUCCESS;
}
