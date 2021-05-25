/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/
//
// Copyright: Avnet 2021
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
#include <stdio.h>

#include "nx_api.h"
#include "nx_azure_iot_hub_client.h"
#include "nx_azure_iot_ciphersuites.h"
#include "azrtos_time.h"
#include "iotconnect_certs.h"
#include "iotconnect.h"
#include "azrtos_iothub_client.h"

/* Define the Azure RTOS IOT thread stack and priority.  */
#ifndef NX_AZURE_IOT_STACK_SIZE
#define NX_AZURE_IOT_STACK_SIZE                     (4096)
#endif /* NX_AZURE_IOT_STACK_SIZE */

#ifndef NX_AZURE_IOT_THREAD_PRIORITY
#define NX_AZURE_IOT_THREAD_PRIORITY                (4)
#endif /* NX_AZURE_IOT_THREAD_PRIORITY */

/* Define Azure RTOS TLS info.  */
static NX_SECURE_X509_CERT root_ca_cert;
static UCHAR nx_azure_iot_tls_metadata_buffer[NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE];
static ULONG nx_azure_iot_thread_stack[NX_AZURE_IOT_STACK_SIZE / sizeof(ULONG)];

/* Define the prototypes for AZ IoT.  */
static NX_AZURE_IOT nx_azure_iot;

static NX_AZURE_IOT_HUB_CLIENT iothub_client;

/* When Using X509 certificate authentication to connect to IoT Hub,
 this struct stores the data of the cert, once it's initialized  */
static NX_SECURE_X509_CERT device_certificate;

/* Define sample threads. */

static IotConnectIotHubConfig config;

static bool is_connected = false;
static bool is_disconnect_requested = false;


static VOID connection_status_callback(NX_AZURE_IOT_HUB_CLIENT *hub_client_ptr, UINT status) {
    NX_PARAMETER_NOT_USED(hub_client_ptr);
    if (status) {
        is_connected = false;
        if (is_disconnect_requested) {
            is_disconnect_requested = false;
        } else {
            nx_azure_iot_hub_client_deinitialize(&iothub_client);
            nx_azure_iot_delete(&nx_azure_iot);
            printf("Received a disconnect!\r\n");

        }
        if (status != NX_AZURE_IOT_DISCONNECTED) {
            printf("Disconnected from IoTHub!: error code = 0x%08x\r\n", status);
        }
        if (config.status_cb) {
            config.status_cb(MQTT_DISCONNECTED);
        }
    } else {
        printf("Connected to IoTHub.\r\n");
        is_connected = true;
        if (config.status_cb) {
            config.status_cb(MQTT_CONNECTED);
        }
    }
}

static UINT initialize_iothub(NX_AZURE_IOT_HUB_CLIENT *iothub_client_ptr) {
    UINT status;

    /* Initialize IoTHub client. */
    if ((status = nx_azure_iot_hub_client_initialize(iothub_client_ptr, &nx_azure_iot, //
            (UCHAR*) config.host, strlen(config.host), //
            (UCHAR*) config.device_name, strlen(config.device_name), //
            (UCHAR*) "", 0, // modules are not supported
            _nx_azure_iot_tls_supported_crypto, //
            _nx_azure_iot_tls_supported_crypto_size, //
            _nx_azure_iot_tls_ciphersuite_map, //
            _nx_azure_iot_tls_ciphersuite_map_size, //
            nx_azure_iot_tls_metadata_buffer, //
            sizeof(nx_azure_iot_tls_metadata_buffer), //
            &root_ca_cert))) {
        printf("Failed on nx_azure_iot_hub_client_initialize!: error code = 0x%08x\r\n", status);
        return (status);
    }
    if (config.auth->type != IOTC_KEY) {
        printf("Using x509 authentication.\r\n");
        const UINT key_type = (config.auth->type == IOTC_X509_RSA ? //
        NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER : //
        NX_SECURE_X509_KEY_TYPE_EC_DER //
                );
        /* Initialize the device certificate.  */
        if ((status = nx_secure_x509_certificate_initialize(&device_certificate, //
                config.auth->data.identity.client_certificate, //
                config.auth->data.identity.client_certificate_len, //
                NX_NULL, 0, //
                config.auth->data.identity.client_private_key, //
                config.auth->data.identity.client_private_key_len, //
                key_type))) {
            printf("Failed on nx_secure_x509_certificate_initialize!: error code = 0x%08x\r\n", status);
            return status;
        }

        /* Set device certificate.  */
        if ((status = nx_azure_iot_hub_client_device_cert_set(iothub_client_ptr, &device_certificate))) {
            printf("Failed on nx_azure_iot_hub_client_device_cert_set!: error code = 0x%08x\r\n", status);
            return status;
        }
    } else {
        printf("Using key based authentication....\r\n");
        /* Set symmetric key.  */
        if ((status = nx_azure_iot_hub_client_symmetric_key_set(iothub_client_ptr, //
                (UCHAR*) config.auth->data.symmetric_key, //
                strlen(config.auth->data.symmetric_key)))) {
            printf("Failed on nx_azure_iot_hub_client_symmetric_key_set!\r\n");
            return status;
        }
    }
    /* Set connection status callback. */
    if ((status = nx_azure_iot_hub_client_connection_status_callback_set(iothub_client_ptr, connection_status_callback))) {
        printf("Failed on connection_status_callback!\r\n");
        return status;
    }
    if ((status = nx_azure_iot_hub_client_cloud_message_enable(iothub_client_ptr))) {
        printf("C2D receive enable failed!: error code = 0x%08x\r\n", status);
        return status;
    }

    if (status) {
        nx_azure_iot_hub_client_deinitialize(iothub_client_ptr);
        return status;
    }

    return (status);
}

static void log_callback(az_log_classification classification, UCHAR *msg, UINT msg_len) {
    if (classification == AZ_LOG_IOT_AZURERTOS) {
        printf("%.*s", msg_len, (CHAR*) msg);
    }
}
UINT iothub_client_init(IotConnectIotHubConfig *c, IotConnectAzrtosConfig *azrtos_config) {
    UINT status = 0;

    is_connected = false;

    memset(&config, 0, sizeof(config));
    memcpy(&config, c, sizeof(config));

    nx_azure_iot_log_init(log_callback);

    /* Create Azure IoT handler.  */
    if ((status = nx_azure_iot_create(&nx_azure_iot, (UCHAR*) "Azure IoT", //
            azrtos_config->ip_ptr, azrtos_config->pool_ptr, azrtos_config->dns_ptr, //
            nx_azure_iot_thread_stack, sizeof(nx_azure_iot_thread_stack), //
            NX_AZURE_IOT_THREAD_PRIORITY, &unix_time_get))) {
        printf("Failed on nx_azure_iot_create!: error code = 0x%08x\r\n", status);
        return status;
    }

    printf("Initializing server certificates...\r\n");
    /* Initialize CA certificate. */
    if ((status = nx_secure_x509_certificate_initialize(&root_ca_cert, //
            (UCHAR*)IOTCONNECT_IOTHUB_ROOT_CERT,
            IOTCONNECT_IOTHUB_ROOT_CERT_SIZE,
            NX_NULL, 0, NULL, 0, NX_SECURE_X509_KEY_TYPE_NONE))) {
        printf("Failed to initialize ROOT CA certificate!: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return status;
    }

    printf("Initializing iothub...\r\n");
    if ((status = initialize_iothub(&iothub_client))) {
        printf("Failed to initialize iothub client: error code = 0x%08x\r\n", status);
        nx_azure_iot_delete(&nx_azure_iot);
        return status;
    }

    if (config.auth->type == IOTC_X509_RSA) {
        printf("Using RSA device keys. Key exchange may take over 40 seconds.\r\n");
    }
    printf("Connecting...\r\n");
    if (nx_azure_iot_hub_client_connect(&iothub_client, NX_TRUE, NX_WAIT_FOREVER)) {
        printf("Failed on nx_azure_iot_hub_client_connect!\r\n");
        nx_azure_iot_hub_client_deinitialize(&iothub_client);
        nx_azure_iot_delete(&nx_azure_iot);
        return status;
    }

    /* Create Telemetry sample thread. */
    is_connected = true;
    return NX_AZURE_IOT_SUCCESS;
}

void iothub_client_disconnect() {
    if (is_connected) {
        is_connected = false;
        is_disconnect_requested = true; // don't deinitialize in the callback
        nx_azure_iot_hub_client_disconnect(&iothub_client);
        nx_azure_iot_hub_client_deinitialize(&iothub_client);
        nx_azure_iot_delete(&nx_azure_iot);
        if (config.status_cb) {
            printf("Disconnected from IoTHub.");
            config.status_cb(MQTT_DISCONNECTED);
        }
    }
}

bool iothub_client_is_connected() {
    return is_connected;
}


UINT iothub_send_message(const char *message) {
    UINT status = 0;
    NX_PACKET *packet_ptr;

    /* Create a telemetry message packet. */
    if ((status = nx_azure_iot_hub_client_telemetry_message_create(&iothub_client, &packet_ptr, NX_WAIT_FOREVER))) {
        printf("Telemetry message create failed!: error code = 0x%08x\r\n", status);
        return status;
    }

    if (nx_azure_iot_hub_client_telemetry_send(&iothub_client, packet_ptr, (UCHAR*) message, strlen(message),
    NX_WAIT_FOREVER)) {
        printf("Telemetry message send failed!: error code = 0x%08x\r\n", status);
        nx_azure_iot_hub_client_telemetry_message_delete(packet_ptr);
        return status;
    }
    //printf("Telemetry message sent: %s.\r\n", message);
    return NX_SUCCESS;
}

UINT iothub_c2d_receive(bool loop_forever, ULONG wait_ticks) {
    NX_PACKET *packet_ptr;
    UINT status = 0;
    if (loop_forever) {
        wait_ticks = NX_WAIT_FOREVER;
    }

    /* Loop to receive c2d message.  */
    do {
    	packet_ptr = NULL;
        status = nx_azure_iot_hub_client_cloud_message_receive(&iothub_client, &packet_ptr, wait_ticks);

        if ((NX_AZURE_IOT_NO_PACKET != status && NX_SUCCESS != status)) {
            printf("C2D receive failed!: error code = 0x%08x\r\n", status);
            break;
        }

        if (NX_SUCCESS == status) {
            config.c2d_msg_cb( //
                    packet_ptr->nx_packet_prepend_ptr, //
                    packet_ptr->nx_packet_append_ptr - packet_ptr->nx_packet_prepend_ptr //
                            );
#if 0
            printf("Received message:");
            printf_packet(packet_ptr);
            printf("\r\n");
#endif
        }
        if (packet_ptr) {
            nx_packet_release(packet_ptr);
        }
    } while (loop_forever && is_connected);
    return status;
}

