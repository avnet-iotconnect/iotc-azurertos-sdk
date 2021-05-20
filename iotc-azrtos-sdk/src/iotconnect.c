//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <cJSON.h>

// This defines enables prototype integration with iotc-c-lib v3.0.0
//#define PROTOCOL_V2_PROTOTYPE


#include "iotconnect_discovery.h"
#include "iotconnect_event.h"
#include "azrtos_iothub_client.h"
#include "azrtos_https_client.h"
#include "iotconnect.h"

#ifdef PROTOCOL_V2_PROTOTYPE
#include "iotconnect_request.h"
#endif

#define RESOURCE_PATH_DSICOVERY "/api/sdk/cpid/%s/lang/M_C/ver/2.0/env/%s"
#define RESOURCE_PATH_SYNC "%ssync"

static IotclDiscoveryResponse *discovery_response = NULL;
static IotclSyncResponse *sync_response = NULL;

static IotConnectClientConfig config = { 0 };
static IotclConfig lib_config = { 0 };

static IotConnectAzrtosConfig azrtos_config = { 0 };
#ifdef PROTOCOL_V2_PROTOTYPE
static char * hello_response_dtg = NULL;
#endif

static void dump_response(const char *message, IotConnectHttpResponse *response) {
    printf("%s", message);
    if (response->response) {
        printf(" Response was:\r\n----\r\n%s\r\n----\r\n", response->response);
    } else {
        printf(" Response was empty\r\n");
    }
}

static void report_sync_error(IotclSyncResponse *response, const char *sync_response_str) {
    if (NULL == response) {
        printf("Failed to obtain sync response?\r\n");
        return;
    }
    switch (response->ds) {
    case IOTCL_SR_DEVICE_NOT_REGISTERED:
        printf("IOTC_SyncResponse error: Not registered\r\n");
        break;
    case IOTCL_SR_AUTO_REGISTER:
        printf("IOTC_SyncResponse error: Auto Register\r\n");
        break;
    case IOTCL_SR_DEVICE_NOT_FOUND:
        printf("IOTC_SyncResponse error: Device not found\r\n");
        break;
    case IOTCL_SR_DEVICE_INACTIVE:
        printf("IOTC_SyncResponse error: Device inactive\r\n");
        break;
    case IOTCL_SR_DEVICE_MOVED:
        printf("IOTC_SyncResponse error: Device moved\r\n");
        break;
    case IOTCL_SR_CPID_NOT_FOUND:
        printf("IOTC_SyncResponse error: CPID not found\r\n");
        break;
    case IOTCL_SR_UNKNOWN_DEVICE_STATUS:
        printf("IOTC_SyncResponse error: Unknown device status error from server\r\n");
        break;
    case IOTCL_SR_ALLOCATION_ERROR:
        printf("IOTC_SyncResponse internal error: Allocation Error\r\n");
        break;
    case IOTCL_SR_PARSING_ERROR:
        printf("IOTC_SyncResponse internal error: Parsing error. Please check parameters passed to the request.\r\n");
        break;
    default:
        printf("WARN: report_sync_error called, but no error returned?\r\n");
        break;
    }
    printf("Raw server response was:\r\n--------------\r\n%s\r\n--------------\r\n", sync_response_str);
}

static IotclDiscoveryResponse* run_http_discovery(const char *cpid, const char *env) {
    IotclDiscoveryResponse *ret = NULL;
    char resource_str_buff [ sizeof(RESOURCE_PATH_DSICOVERY) + CONFIG_IOTCONNECT_CPID_MAX_LEN + CONFIG_IOTCONNECT_ENV_MAX_LEN + 10 /* slack */ ];
    sprintf(resource_str_buff, RESOURCE_PATH_DSICOVERY, cpid, env);
    IotConnectHttpResponse hr;
    UINT status = iotconnect_https_request(&azrtos_config,
            &hr,
            IOTCONNECT_DISCOVERY_HOSTNAME,
            resource_str_buff,
            NULL);
    if (status != NX_SUCCESS) {
        printf("Discovery: iotconnect_https_request() error code: %x data: %s\r\n", status, hr.response);
        goto cleanup;
    }
    if (NULL == hr.response || 0 == strlen(hr.response)) {
        dump_response("Discovery: Unable to obtain HTTP response,", &hr);
        goto cleanup;
    }

    char *json_start = strstr(hr.response, "{");
    if (NULL == json_start) {
        dump_response("Discovery: No json response from server.", &hr);
        goto cleanup;
    }
    if (json_start != hr.response) {
        dump_response("WARN: Expected JSON to start immediately in the returned data.", &hr);
    }

    ret = iotcl_discovery_parse_discovery_response(json_start);
    if (!ret) {
        dump_response("Discovery: Unable to parse HTTP response,", &hr);
    }

    cleanup:
    // fall through
    iotconnect_free_https_response(&hr);
    return ret;
}

static IotclSyncResponse* run_http_sync(const char *cpid, const char *uniqueid) {
    IotclSyncResponse *ret = NULL;
    IotConnectHttpResponse hr;
    char post_data[IOTCONNECT_DISCOVERY_PROTOCOL_POST_DATA_MAX_LEN + 1] = {0};
    char sync_path [strlen(discovery_response->path) + strlen("sync?") + 1];

    sprintf(sync_path, RESOURCE_PATH_SYNC, discovery_response->path);
    snprintf(post_data,
             IOTCONNECT_DISCOVERY_PROTOCOL_POST_DATA_MAX_LEN, /*total length should not exceed MTU size*/
             IOTCONNECT_DISCOVERY_PROTOCOL_POST_DATA_TEMPLATE,
             cpid,
             uniqueid
    );
    UINT status = iotconnect_https_request(&azrtos_config,
            &hr,
            discovery_response->host,
            sync_path,
            post_data);
    if (status != NX_SUCCESS) {
        printf("Sync: iotconnect_https_request() error code: %x data: %s\r\n", status, hr.response);
        goto cleanup;
    }

    if (NULL == hr.response || 0 == strlen(hr.response)) {
        dump_response("Sync: Unable to obtain HTTP response,", &hr);
        goto cleanup;
    }

    char *json_start = strstr(hr.response, "{");
    if (NULL == json_start) {
        dump_response("Sync: No json response from server.", &hr);
        goto cleanup;
    }
    if (json_start != hr.response) {
        dump_response("WARN: Expected JSON to start immediately in the returned data.", &hr);
    }

    ret = iotcl_discovery_parse_sync_response(json_start);
    if (!ret) {
        dump_response("Sync: Unable to parse HTTP response,", &hr);
    }

    if (!ret || ret->ds != IOTCL_SR_OK) {
        report_sync_error(ret, hr.response);
        iotcl_discovery_free_sync_response(ret);
        ret = NULL;
    }

    cleanup:
    // fall through
    iotconnect_free_https_response(&hr);
    return ret;

}

// this function will Give you Device CallBack payload
static void on_iothub_data(UCHAR *data, size_t len) {
    char *str = malloc(len + 1);
    memcpy(str, data, len);
    str[len] = 0;
    printf("event>>> %s\r\n", str);
    if (!iotcl_process_event(str)) {
        printf("Error encountered while processing %s\r\n", str);
    }
    free(str);
}

static void on_iotconnect_status(IotConnectConnectionStatus status) {
    if (config.status_cb) {
        config.status_cb(status);
    }
}
///////////////////////////////////////////////////////////////////////////////////
// Get All twin property from C2D
void iotconnect_sdk_disconnect() {
    printf("Disconnecting...\r\n");
    iothub_client_disconnect();
}

void iotconnect_sdk_send_packet(const char *data) {
    if (iothub_send_message(data)) {
        printf("Failed to send message %s\r\n", data);
    }
}

#ifdef PROTOCOL_V2_PROTOTYPE
static void on_message_intercept(IotclEventData data, IotclEventType type) {
#else PROTOCOL_V2_PROTOTYPE
    static void on_message_intercept(IotclEventData data, IotConnectEventType type) {
#endif
    switch (type) {
    case ON_FORCE_SYNC:
        iotconnect_sdk_disconnect();
        iotcl_discovery_free_discovery_response(discovery_response);
        iotcl_discovery_free_sync_response(sync_response);
        sync_response = NULL;
        discovery_response = run_http_discovery(config.cpid, config.env);
        if (NULL == discovery_response) {
            printf("Unable to run HTTP discovery on ON_FORCE_SYNC \r\n");
            return;
        }
        sync_response = run_http_sync(config.cpid, config.duid);
        if (NULL == sync_response) {
            printf("Unable to run HTTP sync on ON_FORCE_SYNC \r\n");
            return;
        }
        printf("Got ON_FORCE_SYNC. Disconnecting.\r\n");
        iotconnect_sdk_disconnect(); // client will get notification that we disconnected and will reinit

    case ON_CLOSE:
        printf("Got a disconnect request. Closing the mqtt connection. Device restart is required.\r\n");
        iotconnect_sdk_disconnect();
    default:
        break; // not handling nay other messages
    }

    if (NULL != config.msg_cb) {
        config.msg_cb(data, type);
    }
}

void IotConnectSdk_Loop() {
    iothub_c2d_receive(true, NX_WAIT_FOREVER);
}

void iotconnect_sdk_poll(UINT wait_time_ms) {
    iothub_c2d_receive(false, wait_time_ms * NX_IP_PERIODIC_RATE / 1000);
}

IotclConfig* iotconnect_sdk_get_lib_config() {
    return iotcl_get_config();
}

IotConnectClientConfig* iotconnect_sdk_init_and_get_config() {
    memset(&config, 0, sizeof(config));
    return &config;
}

bool iotconnect_sdk_is_connected() {
    // TODO: get connected status from iothub layer
    return iothub_client_is_connected();
}

#ifdef PROTOCOL_V2_PROTOTYPE
static void hello_response_callback(IotclEventData data, IotclEventType type) {
	switch (type) {
	case REQ_HELLO:
        hello_response_dtg = iotcl_clone_response_dtg(data);
        if (NULL != hello_response_dtg) {
            printf("Hello response DTG is: %s\r\n", hello_response_dtg);
        } else {
        	printf("Error from hello response. DTG is null.\r\n");
        }
        break;
     default:
    	 printf("Warning: Received an unknown hello response type %d\r\n", type);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////////
// this the Initialization os IoTConnect SDK
UINT iotconnect_sdk_init(IotConnectAzrtosConfig *ac) {
	UINT ret;
	IotConnectIotHubConfig iic;

	memcpy(&azrtos_config, ac, sizeof(azrtos_config));
    memset(&iic, 0, sizeof(iic));


#ifndef PROTOCOL_V2_PROTOTYPE

    // TODO: ALLOW CACHING! ---------------------------------------------------------------------------------
	iotcl_discovery_free_discovery_response(discovery_response);
	iotcl_discovery_free_sync_response(sync_response);
	discovery_response = NULL;
	sync_response = NULL;
    //if (!discovery_response) {
        discovery_response = run_http_discovery(config.cpid, config.env);
        if (NULL == discovery_response) {
            // get_base_url will print the error
            return -1;
        }
        printf("Discovery response parsing successful.\r\n");
    //}

    //if (!sync_response) {
        sync_response = run_http_sync(config.cpid, config.duid);
        if (NULL == sync_response) {
            // Sync_call will print the error
            return -2;
        }
        printf("Sync response parsing successful.\r\n");
    //}
    // We want to print only first 4 characters of cpid. %.4s doesn't seem to work with prink
    char cpid_buff[5];
    strncpy(cpid_buff, sync_response->cpid, 4);
    cpid_buff[4] = 0;
    printf("CPID: %s***\r\n", cpid_buff);
    printf("ENV:  %s\r\n", config.env);

    iic.c2d_msg_cb = on_iothub_data;

    iic.device_name = sync_response->broker.client_id;
    iic.host = sync_response->broker.host;
    iic.auth = &config.auth;
    iic.status_cb = on_iotconnect_status;


#else
    iic.c2d_msg_cb = on_iothub_data;
    char * client_id = NULL;
    client_id = malloc(strlen(config.cpid) + strlen(config.duid) + 2 /* dash and null */);
    sprintf(client_id, "%s-%s", config.cpid, config.duid);

    iic.device_name = client_id; // sync_response->broker.client_id;
    iic.host = "poc-iotconnect-iothub-eu.azure-devices.net"; // sync_response->broker.host;
    iic.auth = &config.auth;
    iic.status_cb = on_iotconnect_status;
#endif

    lib_config.device.env = config.env;
    lib_config.device.cpid = config.cpid;
    lib_config.device.duid = config.duid;

    lib_config.event_functions.ota_cb = config.ota_cb;
    lib_config.event_functions.cmd_cb = config.cmd_cb;
    lib_config.event_functions.msg_cb = on_message_intercept;


    // intercept internal processing and forward to client
    lib_config.event_functions.msg_cb = on_message_intercept;

#ifndef PROTOCOL_V2_PROTOTYPE
    lib_config.telemetry.dtg = sync_response->dtg;
#else
    lib_config.event_functions.response_cb = hello_response_callback;
    // TODO: deal with workaround for telemetry config
    lib_config.telemetry.dtg = "unused";
    lib_config.request.sid = "Yjg5MmMzNThlMzc1NGNjMzg4NDEzMmUyNzFlMjYxNTE=UDI6MTI6MDMuOTA=";
#endif

    if (!iotcl_init(&lib_config)) {
        printf("Failed to initialize the IoTConnect Lib\r\n");
#ifdef PROTOCOL_V2_PROTOTYPE
        free (client_id);
#endif
        return NX_FALSE;
    }


    ret = iothub_client_init(&iic, &azrtos_config);
    if (ret) {
        printf("Failed to connect!\r\n");
        #ifdef PROTOCOL_V2_PROTOTYPE
                free (client_id);
        #endif
    	return ret;
    }

#ifdef PROTOCOL_V2_PROTOTYPE
    char * hello_request = iotcl_request_create_hello();
    iothub_send_message(hello_request);
    free(hello_request);
    ret = iothub_c2d_receive(false, 5 * NX_IP_PERIODIC_RATE);
    if (ret) {
        printf("Timed out while receiving hello response\r\n");
    } else {
        printf("Received response...!\r\n");
    }

    if (!hello_response_dtg) {
        printf("Failed to obtain DTG from hello request!\r\n");
        iothub_client_disconnect();
        return NX_FALSE;
    }
    lib_config.telemetry.dtg = hello_response_dtg;
    // get dtg and reconfig with telemetry configuration
    if (!iotcl_init(&lib_config)) {
        printf("Failed to initialize the IoTConnect Lib\r\n");
        iothub_client_disconnect();
        return NX_FALSE;
    }
#endif
    return ret;

}
