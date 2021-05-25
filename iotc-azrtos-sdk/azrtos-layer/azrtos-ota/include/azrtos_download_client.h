//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/24/21.
//

#ifndef AZRTOS_DOWNLOAD_CLIENT_H
#define AZRTOS_DOWNLOAD_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "tx_api.h"
#include "azrtos_https_client.h"

UINT iotc_download(IotConnectHttpRequest *req);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_DOWNLOAD_CLIENT_H

