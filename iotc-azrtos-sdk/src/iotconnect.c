//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "cJSON.h"
#include "iotcl.h"
#include "iotcl_dra_discovery.h"
#include "iotcl_dra_identity.h"
#include "azrtos_iothub_client.h"
#include "azrtos_download_client.h"
#include "iotconnect_certs.h"
#include "azrtos_https_client.h"
#include "iotconnect.h"

static IotConnectClientConfig config = { 0 };
static IotConnectAzrtosConfig azrtos_config = { 0 };

static void dump_response(const char *message, const char* what, IotConnectHttpRequest *response) {
    printf("IOTC %s API: %s\r\n", what, message);
    if (response->response) {
        printf(" Response was:\r\n----\r\n%s\r\n----\r\n", response->response);
    } else {
        printf(" Response was empty\r\n");
    }
}

static int validate_http_response(const char* what, IotConnectHttpRequest *req) {
    if (NULL == req->response || 0 == strlen(req->response)) {
        dump_response("Unable to obtain HTTP response.", what, req);
        return IOTCL_ERR_PARSING_ERROR;
    }

    char *json_start = strstr(req->response, "{");
    if (NULL == json_start) {
        dump_response("No json response from server.", what, req);
        return IOTCL_ERR_PARSING_ERROR;
    }
    if (json_start != req->response) {
        dump_response("Expected JSON to start immediately in the returned data.", what, req);
    }
    return 0;
}
static int run_http_identity(const char* duid, const char *cpid, const char *env) {
    IotConnectHttpRequest req = { 0 };
    IotclDraUrlContext discovery_url = {0};
    IotclDraUrlContext identity_url = {0};
    UINT http_status;

    int status = iotcl_dra_discovery_init_url_azure(&discovery_url, cpid, env);
    if (status) {
        return status; // called function will print the error
    }

    req.azrtos_config = &azrtos_config;
    req.host_name = (char *)iotcl_dra_url_get_hostname(&discovery_url);
    req.resource = (char *)iotcl_dra_url_get_resource(&discovery_url);
    req.tls_cert = (unsigned char*) IOTCONNECT_GODADDY_G2_ROOT_CERT;
    req.tls_cert_len = IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;

    http_status = iotconnect_https_request(&req);
    if (http_status != NX_SUCCESS) {
        printf("IOTC: Discovery: iotconnect_https_request() error code: %x data: %s\r\n", status, req.response);
        status = (int)http_status;
        goto cleanup;
    }

    iotcl_dra_url_deinit(&discovery_url);

    status = validate_http_response("Discovery", &req);
    if (status) {
    	goto cleanup; // called function will print the error
    }

    status = iotcl_dra_discovery_parse(&identity_url, 0, req.response);
    if (status) {
        dump_response("Unable to parse Discovery response", "Discovery", &req);
        goto cleanup;
    }

    status = iotcl_dra_identity_build_url(&identity_url, duid);
    if (status) {
    	goto cleanup; // called function will print the error
    }


    memset(&req, 0, sizeof(IotConnectHttpRequest));

    req.azrtos_config = &azrtos_config;
    req.host_name = (char *)iotcl_dra_url_get_hostname(&identity_url);
    req.resource = (char *)iotcl_dra_url_get_resource(&identity_url);
    req.tls_cert = (unsigned char*) IOTCONNECT_GODADDY_G2_ROOT_CERT;
    req.tls_cert_len = IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;

    http_status = iotconnect_https_request(&req);
    if (http_status != NX_SUCCESS) {
        printf("IOTC: Discovery: iotconnect_https_request() error code: %x data: %s\r\n", status, req.response);
        status = (int)http_status;
        goto cleanup;
    }

    status = validate_http_response("Identity", &req);
    if (status) {
    	goto cleanup; // called function will print the error
    }

    status = iotcl_dra_identity_configure_library_mqtt(req.response);
	if (status) {
		dump_response("Unable to configure the library from identity response", "Identity", &req);
		goto cleanup;
	}

cleanup:
	iotcl_dra_url_deinit(&discovery_url);
	iotcl_dra_url_deinit(&identity_url);
    return status;
}

// this function will Give you Device CallBack payload
static void on_iothub_data(UCHAR *data, size_t len) {
    if (config.verbose) {
        printf("IOTC: event>>> %*s\r\n", len, (char *)data);
    }
    iotcl_c2d_process_event_with_length(data, len); // the called function will print errors
}

static void on_iotconnect_status(IotConnectConnectionStatus status) {
    if (config.status_cb) {
        config.status_cb(status);
    }
}
///////////////////////////////////////////////////////////////////////////////////
// Get All twin property from C2D
void iotconnect_sdk_disconnect() {
    printf("IOTC: Disconnecting...\r\n");
    iothub_client_disconnect();
}

void iotconnect_sdk_mqtt_send_cb(const char *topic, const char *json_str) {
    if (config.verbose) {
        printf(">: %s\r\n",  json_str);
    }
    UINT status = iothub_send_message(topic, json_str);
    if (status) {
    	printf("Error encountered while sending the message. Error code was: %x\r\n", (unsigned int) status);
    }
}

void iotconnect_sdk_poll(UINT wait_time_ms) {
    iothub_c2d_receive(false, wait_time_ms * NX_IP_PERIODIC_RATE / 1000);
}

void iotconnect_sdk_init_config(IotConnectClientConfig *config) {
    memset(&config, 0, sizeof(config));
}

bool iotconnect_sdk_is_connected() {
    return iothub_client_is_connected();
}

///////////////////////////////////////////////////////////////////////////////////
// this the Initialization os IoTConnect SDK
UINT iotconnect_sdk_init(IotConnectAzrtosConfig *ac, IotConnectClientConfig* c) {
	UINT status;
	IotConnectIotHubConfig iic;
	IotclClientConfig iotcl_cfg = { 0 };

	memcpy(&azrtos_config, ac, sizeof(azrtos_config));
    memset(&iic, 0, sizeof(iic));

    iic.c2d_msg_cb = on_iothub_data;

    iic.auth = &config.auth;
    iic.status_cb = on_iotconnect_status;

    iotcl_cfg.device.cpid = c->cpid;
    iotcl_cfg.device.duid = c->duid;
    iotcl_cfg.device.instance_type = IOTCL_DCT_CUSTOM;
    iotcl_cfg.mqtt_send_cb = iotconnect_sdk_mqtt_send_cb;
    iotcl_cfg.events.cmd_cb = config.cmd_cb;
    iotcl_cfg.events.ota_cb = config.ota_cb;


    if (c->verbose) {
        status = (UINT)iotcl_init_and_print_config(&iotcl_cfg);
    } else {
        status = (UINT)iotcl_init(&iotcl_cfg);
    }

    if (status) {
        printf("IOTC: Failed to initialize the IoTConnect Lib\r\n");
        return status;
    }

    status = run_http_identity(c->duid, c->cpid, c->env);
    if (status) {
        iotcl_deinit();
        return status; // called function will print errors
    }

    printf("IOTC: Connecting to IoTHub.\r\n");
    status = iothub_client_init(&iic, &azrtos_config);
    if (status) {
        printf("IOTC: Failed to connect!\r\n");
        iotcl_deinit();
    	return status;
    }

    return status;
}
