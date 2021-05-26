//
// Copyright: Avnet 2021
// Created by Shu Liu <shu.liu@avnet.com> on 4/19/21.
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
// This HTTPS client implementation assumes application/json content type and returns it as
// null terminated string, unless a custom request handler is used.

#ifndef AZRTOS_HTTPS_CLIENT_H
#define AZRTOS_HTTPS_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect.h"
#include "nx_web_http_client.h"

struct IotConnectHttpRequest;

typedef UINT (*IotConnectHttpCustomHandler) (struct IotConnectHttpRequest *req, NX_WEB_HTTP_CLIENT *http_client);


typedef struct IotConnectHttpRequest {
    IotConnectAzrtosConfig *azrtos_config;
    char *host_name;
    char *resource; // path of the resource to GET/PUT
    char *payload; // if payload is not null, a POST will be issued, rather than GET.
    char *response; // We will will provide a default buffer with default size. Response will be a null terminated string.
    size_t response_length; // We will will allocate a default buffer with default size. Response will be a null terminated string.
    unsigned char *tls_cert; // provide an SSL certificate for your host (default ones provided in iotconnect_certs.h
    unsigned int tls_cert_len; // provide length of the certificate for your https host

    // If this callback is set, we will relay request handling to the callback function,
    // once the connection has been established. This is generally intended for the download client,
    // but can be used for any functionality not provided by the default handler
    IotConnectHttpCustomHandler custom_handler_cb;

} IotConnectHttpRequest;

// supports get and post
// if post_data is NULL, a get is executed
UINT iotconnect_https_request(IotConnectHttpRequest *request);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_HTTPS_CLIENT_H

