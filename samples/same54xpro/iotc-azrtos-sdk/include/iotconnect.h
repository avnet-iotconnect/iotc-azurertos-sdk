//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//

#ifndef IOTCONNECT_H
#define IOTCONNECT_H

#include <stddef.h>
#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect_event.h"
#include "iotconnect_telemetry.h"
#include "iotconnect_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UNDEFINED,
    MQTT_CONNECTED,
    MQTT_DISCONNECTED,
    // MQTT_FAILED, this status is not applicable to AzureRTOS implementation
    // TODO: Sync statuses etc.
} IotConnectConnectionStatus;

typedef enum {
    IOTC_KEY,
	IOTC_X509_RSA, // RSA key and cert in DER format
	IOTC_X509_ECC  // ECC key and cert in DER format (not supported with AzreRTOS as of yet)
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
        struct identity { // for both IOTC_IDENTITY_RSA and IOTC_IDENTITY_ECC
            UCHAR *client_private_key; // DER format (binary to C array)
            size_t client_private_key_len;
            UCHAR *client_certificate; // DER format (binary to C array)
            size_t client_certificate_len;
        } identity;
        char *symmetric_key; //
    } data;
} IotConnectAuth;

typedef struct {
    char *env;    // Environment name. Contact your representative for details.
    char *cpid;   // Settings -> Company Profile.
    char *duid;   // Name of the device.
    IotConnectAuth auth;
    IotclOtaCallback ota_cb; // callback for OTA events.
    IotclCommandCallback cmd_cb; // callback for command events.
    IotclMessageCallback msg_cb; // callback for ALL messages, including the specific ones like cmd or ota callback.
    IotConnectStatusCallback status_cb; // callback for connection status
} IotConnectClientConfig;


IotConnectClientConfig *iotconnect_sdk_init_and_get_config();

UINT iotconnect_sdk_init(IotConnectAzrtosConfig *config);

bool iotconnect_sdk_is_connected();

IotclConfig *iotconnect_sdk_get_lib_config();

void iotconnect_sdk_send_packet(const char *data);

// Receive loop hook forever-blocking for for C2D messages.
// Either call this function, or IoTConnectSdk_Poll()
void iotconnect_sdk_receive();

// Receive poll hook for for C2D messages.
// Either call this function, or IotConnectSdk_Receive()
// Set wait_time to a multiple of NX_IP_PERIODIC_RATE
void iotconnect_sdk_poll(UINT wait_time_ms);

void iotconnect_sdk_disconnect();

#ifdef __cplusplus
}
#endif

#endif
