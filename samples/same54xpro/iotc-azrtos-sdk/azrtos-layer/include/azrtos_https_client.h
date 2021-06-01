//
// Copyright: Avnet 2021
// Created by Shu Liu <shu.liu@avnet.com> on 4/19/21.
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef AZRTOS_HTTPS_CLIENT_H
#define AZRTOS_HTTPS_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "nx_api.h"
#include "nxd_dns.h"


typedef struct {
    char *response; // only response for now, but may include transient data that can be freed up in the future
} IotConnectHttpResponse;

// supports get and post
// if post_data is NULL, a get is executed
UINT iotconnect_https_request(IotConnectAzrtosConfig *azrtos_config,
        IotConnectHttpResponse *r,
        char *host_name,
        char *resource,
        char *post_data
        );
void iotconnect_free_https_response(IotConnectHttpResponse *response);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_HTTPS_CLIENT_H

