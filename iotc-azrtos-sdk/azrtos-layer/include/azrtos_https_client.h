//
// Copyright: Avnet 2021
// Created by Shu Liu <shu.liu@avnet.com> on 4/19/21.
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
// This HTTPS client implementation assumes application/json content type and returns it as
// null terminated string, unless a custom request handler is used.
// Custom headers can also be sent and received. By default Content-Type application/json is
// being sent.
// If one needs to send custom headers in the request, they should be specified in the request_headers
// and request_headers_size should be set to the number of headers in the array (up to IOTC_HTTP_CLIENT_MAX_HEADERS).
// If the user wants to receive response headers, the callback custom_response_header_cb
// should be set. Note that header field names and values will NOT be null terminated strings.

#ifndef AZRTOS_HTTPS_CLIENT_H
#define AZRTOS_HTTPS_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect.h"
#include "nx_web_http_client.h"

#ifndef IOTC_HTTP_CLIENT_MAX_HEADERS 
#define IOTC_HTTP_CLIENT_MAX_HEADERS 3 // Maximum number of headers in either request or response
#endif 
    
struct IotConnectHttpRequest;

typedef UINT (*IotConnectHttpCustomHandler) (struct IotConnectHttpRequest *req, NX_WEB_HTTP_CLIENT *http_client);
typedef VOID (*IotConnectHttpResponseHeaderHandler) (
    CHAR *field_name, UINT field_name_length,
    CHAR *field_value, UINT field_value_length
);

typedef struct IotConnectHttpHeader {
    char *name;
    char *value;
} IotConnectHttpHeader;


typedef struct IotConnectHttpRequest {
    IotConnectAzrtosConfig *azrtos_config;
    char *host_name;
    char *resource; // path of the resource to GET/PUT
    char *payload; // if payload is not null, a POST will be issued, rather than GET.
    IotConnectHttpHeader request_headers[IOTC_HTTP_CLIENT_MAX_HEADERS];
    size_t request_headers_size;
    char *response; // We will will provide a default buffer with default size. Response will be a null terminated string.
    size_t response_length; // We will will allocate a default buffer with default size. Response will be a null terminated string.
    IotConnectHttpHeader* response_headers[IOTC_HTTP_CLIENT_MAX_HEADERS + 1];// see header comments in this header file
    unsigned char *tls_cert; // provide an SSL certificate for your host (default ones provided in iotconnect_certs.h
    unsigned int tls_cert_len; // provide length of the certificate for your https host

    // If this callback is set, custom_handler_cb should not be set.
    // Use this callback to implement response header handling.
    // We will call back the user function as each header is being sent
    // by the server.
    // NOTE: There is no guarantee that the 
    IotConnectHttpResponseHeaderHandler custom_response_header_cb;

    // If this callback is set, custom_response_header_cb should not be set.
    // We will relay request handling to the callback function,
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

