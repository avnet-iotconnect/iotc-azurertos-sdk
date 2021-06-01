//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef AZRTOS_IOTHUB_CLIENT_H
#define AZRTOS_IOTHUB_CLIENT_H
#ifdef __cplusplus
extern   "C" {
#endif

#include "iotconnect.h"

typedef void (*IotConnectC2dCallback)(UCHAR* message, size_t message_len);

typedef struct {
    char *host;    // IoTHub host to connect the client to
    char *device_name;   // Name of the device - combined "cpid-duid"
    IotConnectAuth *auth; // Pointer to IoTConnect auth configuration
    IotConnectC2dCallback c2d_msg_cb; // callback for inbound messages
    IotConnectStatusCallback status_cb; // callback for connection status
} IotConnectIotHubConfig;

UINT iothub_client_init(IotConnectIotHubConfig *c, IotConnectAzrtosConfig* azrtos_config);

void iothub_client_disconnect();

bool iothub_client_is_connected();

// send a null terminated string to IoTHub
UINT iothub_send_message(const char *message);

/**
Receive message(s) from IoTHub when a message is received, status_cb is called.

loop_forever if you wish to call this function from own thread.
wait_time will be ignored in this case and set NX_WAIT_FOREVER.

If calling from a single thread (loop_forever = false)
that will be sending and receiving set wait time to the desired value as a multiple of NX_IP_PERIODIC_RATE.
 *
 */
UINT iothub_c2d_receive(bool loop_forever, ULONG wait_ticks);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_IOTHUB_CLIENT_H
