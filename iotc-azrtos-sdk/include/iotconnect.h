//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//

#ifndef IOTCONNECT_H
#define IOTCONNECT_H

#include <iotc_auth_driver.h>
#include <stddef.h>
#include <stdbool.h>
#include "nx_api.h"
#include "nxd_dns.h"
#include "iotcl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UNDEFINED,
    MQTT_CONNECTED,
    MQTT_DISCONNECTED,
} IotConnectConnectionStatus;

typedef enum {
    IOTC_KEY,		// Symmetric key
	IOTC_X509,		// Private key and ceritificate
} IotConnectAuthType;

typedef void (*IotConnectStatusCallback)(IotConnectConnectionStatus data);

typedef struct {
	NX_IP *ip_ptr;
	NX_PACKET_POOL *pool_ptr;
	NX_DNS *dns_ptr;
} IotConnectAzrtosConfig;

typedef struct {
    IotConnectAuthType type;
    union { // union because we may support different types of auth
        char *symmetric_key; //
        struct {
            IotcAuthInterface auth_interface; //
            IotcAuthInterfaceContext auth_interface_context;
        } x509;
    } data;
} IotConnectAuth;

typedef struct {
    char *env;    // Environment name. Contact your representative for details.
    char *cpid;   // Settings -> Company Profile.
    char *duid;   // Name of the device.
    IotConnectAuth auth;
    IotclOtaCallback ota_cb; // callback for OTA events.
    IotclCommandCallback cmd_cb; // callback for command events.
    IotConnectStatusCallback status_cb; // callback for connection status
    bool verbose; // If true, we will output extra info and sent and received MQTT json data to standard out
} IotConnectClientConfig;


void iotconnect_sdk_init_config(IotConnectClientConfig *c);

UINT iotconnect_sdk_init(IotConnectAzrtosConfig *ac, IotConnectClientConfig *c);

bool iotconnect_sdk_is_connected(void);

// Receive poll hook for for C2D messages.
// Either call this function, or IotConnectSdk_Receive()
// Set wait_time to a multiple of NX_IP_PERIODIC_RATE
void iotconnect_sdk_poll(UINT wait_time_ms);

void iotconnect_sdk_disconnect(void);

#ifdef __cplusplus
}
#endif

#endif
