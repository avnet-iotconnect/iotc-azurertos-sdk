/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

/*
 * This file contains functions that aid in developing SDKs for specific platforms or help implement custom approaches
 * for to IoTConnect discovery HTTP API.
 */

#ifndef IOTCONNECT_DISCOVERY_H
#define IOTCONNECT_DISCOVERY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iotconnect_lib_config.h"

#ifndef IOTCONNECT_DISCOVERY_HOSTNAME
#define IOTCONNECT_DISCOVERY_HOSTNAME "discovery.iotconnect.io"
#endif

///////////////////////////////////////////////////////////////////////////////////
// you need to pass cpid , env and the HOST at GET_TEMPLATE
// This templates can be used for raw HTTP headers in case that the platform doesn't GET/POST functionality
#define IOTCONNECT_DISCOVERY_HEADER_TEMPLATE \
    "GET /api/sdk/cpid/%s/lang/M_C/ver/2.0/env/%s HTTP/1.1\r\n" \
    "Host: " IOTCONNECT_DISCOVERY_HOSTNAME "\r\n" \
    "Connection: close\r\n" \
    "\r\n"

///////////////////////////////////////////////////////////////////////////////
// This templates can be used for raw HTTP headers in case that the platform doesn't GET/POST functionality
// you need to pass URL returned from discovery host ,host form discovery host, post_data_lan and post_data
#define IOTCONNECT_SYNC_HEADER_TEMPLATE \
    "POST %s/sync? HTTP/1.1\r\n" \
    "Host: %s\r\n" \
    "Content-Type: application/json; charset=utf-8\r\n" \
    "Connection: close\r\n" \
    "Content-length: %d\r\n" \
    "\r\n" \
    "%s"

// You will typically use this JSON post data to get mqtt client information
#define IOTCONNECT_DISCOVERY_PROTOCOL_POST_DATA_TEMPLATE "{\"cpId\":\"%s\",\"uniqueId\":\"%s\",\"option\":{\"attribute\":false,\"setting\":false,\"protocol\":true,\"device\":false,\"sdkConfig\":false,\"rule\":false}}"

// add 1 for string terminator
#define IOTCONNECT_DISCOVERY_PROTOCOL_POST_DATA_MAX_LEN (\
    sizeof(IOTCONNECT_DISCOVERY_PROTOCOL_POST_DATA_TEMPLATE) + \
    CONFIG_IOTCONNECT_DUID_MAX_LEN + CONFIG_IOTCONNECT_CPID_MAX_LEN \
    )

typedef enum {
    IOTCL_SR_OK = 0,
    IOTCL_SR_DEVICE_NOT_REGISTERED = 1,
    IOTCL_SR_AUTO_REGISTER,
    IOTCL_SR_DEVICE_NOT_FOUND,
    IOTCL_SR_DEVICE_INACTIVE,
    IOTCL_SR_DEVICE_MOVED,
    IOTCL_SR_CPID_NOT_FOUND,
    IOTCL_SR_UNKNOWN_DEVICE_STATUS = 20,
    IOTCL_SR_ALLOCATION_ERROR,
    IOTCL_SR_PARSING_ERROR,
} IotclSyncResult;

//NOTE: Append "sync?" to the base url or path in order to to do a sync POST
typedef struct IotclDiscoveryResponse {
    char *url;
    char *host; // parsed out host from the url
    char *path; // parsed out base ULR request path
} IotclDiscoveryResponse;

typedef struct IotclSyncResponse {
    IotclSyncResult ds;
    char *cpid; // validated CPID from the cloud
    char *dtg;
    int ee; // reserved for future use
    int rc; // reserved for future use
    int at; // reserved for future use
    struct protocol {
        char *client_id;
        char *name;
        char *host;
        char *user_name;
        char *pass;
        char *pub_topic;
        char *sub_topic;
    } broker;
} IotclSyncResponse;

// You must free the response when done
// Returned NULL means that there was a memory allocation or a parsing error
IotclDiscoveryResponse *iotcl_discovery_parse_discovery_response(const char *response_data);

void iotcl_discovery_free_discovery_response(IotclDiscoveryResponse *response);

// This function returns NULL in case of allocation failure
// The user mast check the ds value for "OK". Corresponding error should be handled/repoted and the response should be freed
IotclSyncResponse *iotcl_discovery_parse_sync_response(const char *response_data);

void iotcl_discovery_free_sync_response(IotclSyncResponse *response);


#ifdef __cplusplus
}
#endif

#endif //IOTCONNECT_DISCOVERY_H
