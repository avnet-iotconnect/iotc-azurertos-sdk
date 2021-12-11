//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/24/21.
//
// This download client can be used to download large files or just any binary data with range requests.
//

#ifndef AZRTOS_DOWNLOAD_CLIENT_H
#define AZRTOS_DOWNLOAD_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "tx_api.h"
#include "azrtos_https_client.h"

// This status will be returned when the user wants to abort the download
#define NX_DOWNLOAD_ABORTED_BY_USER 0x900F9

typedef enum {
    IOTC_DL_UNKNOWN = 0,
    IOTC_DL_STATUS,
    IOTC_DL_FILE_SIZE,
    IOTC_DL_DATA,
} IotConnectDownloadEventType;

typedef struct {
    IotConnectDownloadEventType type;
    union {
        struct data {
            unsigned char* data_ptr;
            size_t data_size;
            size_t offset;
            size_t file_size; // for convenience, from IOTC_DL_FILE_SIZE event
        } data;             // when IOTC_DL_DATA
        size_t file_size;   // when IOTC_DL_FILE_SIZE
        UINT status;        // when IOTC_DL_STATUS
    };
} IotConnectDownloadEvent;

// if false is returned and download is in progress, download will abort and report NX_DOWNLOAD_ABORTED_BY_USER
typedef bool (*IotConnectDownloadHandler) (IotConnectDownloadEvent* event);

// This download client download chunks of data from the server using the HTTP Range requests
// and call the user back via event_callback with data chunks
// If the download finishes successful, the user will be called back with NX_SUCCESS.
// The client will retry downloading individual chunks, but it won't try reconnecting or resuming automatically
// It is up to he user to interpret the returned error code and re-attempt with resume=true
// If resume=true, the client will notify again of the file size, but resume from where download left off
UINT iotc_download(IotConnectHttpRequest *r, IotConnectDownloadHandler event_callback, bool resume);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_DOWNLOAD_CLIENT_H

