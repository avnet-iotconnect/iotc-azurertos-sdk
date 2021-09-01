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
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
#include <errno.h>
#include "nx_api.h"
#include "wifi.h"
#include "nx_wifi.h"
#include "nxd_dns.h"
#include "nxd_sntp_client.h"
#include "nx_secure_tls_api.h"
#include "stm32l4xx_hal.h"  
#include "iotconnect.h"
#include "azrtos_time.h"
#include "app_config.h"

#include "sensors_data.h"
#include "stm32l4xx_hal.h"
#include "stm32l4s5i_iot01.h"
#include "stm32l475e_iot01_env_sensors.h"
#include "stm32l475e_iot01_motion_sensors.h"


/* Define the thread for running Azure sample on ThreadX (X-Ware IoT Platform).  */
#ifndef SAMPLE_THREAD_STACK_SIZE
#define SAMPLE_THREAD_STACK_SIZE        (6 * 1024)
#endif /* SAMPLE_THREAD_STACK_SIZE  */

#ifndef SAMPLE_THREAD_PRIORITY
#define SAMPLE_THREAD_PRIORITY          (4)
#endif /* SAMPLE_THREAD_PRIORITY  */

extern int _end; // heap end for _sbrk
/* Define the memory area for the sample thread.  */
static UCHAR sample_thread_stack[SAMPLE_THREAD_STACK_SIZE];

/* Define the prototypes for the sample thread.  */
static TX_THREAD sample_thread;

extern bool app_startup(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr);
extern CHAR* http_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, CHAR *host_name, CHAR *resource);
static void sample_thread_entry(ULONG parameter);

/* Define the default thread priority, stack size, etc. The user can override this 
 via -D command line option or via project settings.  */

#ifndef SAMPLE_PACKET_COUNT
#define SAMPLE_PACKET_COUNT             (20)
#endif /* SAMPLE_PACKET_COUNT  */

#ifndef SAMPLE_PACKET_SIZE
#define SAMPLE_PACKET_SIZE              (1200)  /* Set the default value to 1200 since WIFI payload size (ES_WIFI_PAYLOAD_SIZE) is 1200.  */
#endif /* SAMPLE_PACKET_SIZE  */

#define SAMPLE_POOL_SIZE                ((SAMPLE_PACKET_SIZE + sizeof(NX_PACKET)) * SAMPLE_PACKET_COUNT)

#ifndef SAMPLE_SNTP_SYNC_MAX
#define SAMPLE_SNTP_SYNC_MAX              3
#endif /* SAMPLE_SNTP_SYNC_MAX */

/* Define the stack/cache for ThreadX.  */
static UCHAR sample_pool_stack[SAMPLE_POOL_SIZE];

/* Define the prototypes for ThreadX.  */
static NX_IP ip_0;
static NX_PACKET_POOL pool_0;
static NX_DNS dns_client;

/* Using SNTP to get unix time.  */
/* Define the address of SNTP Server. If not defined, use DNS module to resolve the host name SAMPLE_SNTP_SERVER_NAME.  */
/*
 #define SAMPLE_SNTP_SERVER_ADDRESS     IP_ADDRESS(118, 190, 21, 209)
 */

#ifndef SAMPLE_SNTP_SERVER_NAME
#define SAMPLE_SNTP_SERVER_NAME           "0.pool.ntp.org"    /* SNTP Server.  */
#endif /* SAMPLE_SNTP_SERVER_NAME */

static UINT dns_create();

VOID board_setup(void);

/* Define main entry point.  */
int main(void) {
    /* Setup platform. */
    board_setup();

    init_sensors();

    // memory_test();

    /* Enter the ThreadX kernel.  */
    tx_kernel_enter();
}

/* Define what the initial system looks like.  */
void tx_application_define(void *first_unused_memory) {

    UINT status;

    /* Initialize the NetX system.  */
    nx_system_initialize();

    /* Create a packet pool.  */
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", SAMPLE_PACKET_SIZE, sample_pool_stack,
            SAMPLE_POOL_SIZE);

    /* Check for pool creation error.  */
    if (status) {
        printf("PACKET POOL CREATE FAIL.\r\n");
        return;
    }

    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", 0, 0, &pool_0, NULL, NULL, (LONG) NULL, 0);

    /* Check for IP create errors.  */
    if (status) {
        printf("IP CREATE FAIL.\r\n");
        return;
    }

    /* Initialize THREADX Wifi.  */
    status = nx_wifi_initialize(&ip_0, &pool_0);

    /* Check status.  */
    if (status) {
        printf("WIFI INITIALIZE FAIL.\r\n");
        return;
    }

    /* Initialize TLS.  */
    nx_secure_tls_initialize();

    /* Create Threadx Azure thread. */
    status = tx_thread_create(&sample_thread, "Sample Thread", sample_thread_entry, 0, sample_thread_stack,
            SAMPLE_THREAD_STACK_SIZE, SAMPLE_THREAD_PRIORITY, SAMPLE_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Check status.  */
    if (status) {
        printf("Azure sample Thread Create Failed.\r\n");
    }

}

/* Define sample helper thread entry.  */
void sample_thread_entry(ULONG parameter) {
    UINT status;

    /* Create DNS.  */
    status = dns_create();

    /* Check for DNS create errors.  */
    if (status) {
        printf("dns_create fail: %u\r\n", status);
        return;
    }

    /* Sync up time by SNTP at start up.  */
    for (UINT i = 0; i < SAMPLE_SNTP_SYNC_MAX; i++) {

        /* Start SNTP to sync the local time.  */
        status = sntp_time_sync(&ip_0, &pool_0, &dns_client, SAMPLE_SNTP_SERVER_NAME);

        /* Check status.  */
        if (status == NX_SUCCESS)
            break;
    }

    /* Check status.  */
    if (status) {
        printf("SNTP Time Sync failed. Stopping the application.\r\n");
        return;
    } else {
        printf("SNTP Time Sync successful.\r\n");
    }
    bool app_status = app_startup(&ip_0, &pool_0, &dns_client);
    printf("App exited with status %s.\r\n", app_status ? "true" : "false");
    while (true) {
        tx_thread_sleep(10 * NX_IP_PERIODIC_RATE);
    }
}

static UINT dns_create(ULONG dns_server_address) {

    UINT status;
    UCHAR dns_address_1[4];
    UCHAR dns_address_2[4];

    /* Create a DNS instance for the Client.  Note this function will create
     the DNS Client packet pool for creating DNS message packets intended
     for querying its DNS server. */
    status = nx_dns_create(&dns_client, &ip_0, (UCHAR*) "DNS Client");
    if (status) {
        return (status);
    }

    /* Is the DNS client configured for the host application to create the pecket pool? */
#ifdef NX_DNS_CLIENT_USER_CREATE_PACKET_POOL   

    /* Yes, use the packet pool created above which has appropriate payload size
     for DNS messages. */
    status = nx_dns_packet_pool_set(&dns_client, &pool_0);
    if (status) {
        return (status);
    }
#endif /* NX_DNS_CLIENT_USER_CREATE_PACKET_POOL */  

    /* Get the DNS address.  */
    if (WIFI_GetDNS_Address(dns_address_1, dns_address_2) != WIFI_STATUS_OK) {
        nx_dns_delete(&dns_client);
        return (NX_NOT_SUCCESSFUL);
    }

    /* Add an IPv4 server address to the Client list. */
    status = nx_dns_server_add(&dns_client,
            IP_ADDRESS(dns_address_1[0], dns_address_1[1], dns_address_1[2], dns_address_1[3]));
    if (status) {
        nx_dns_delete(&dns_client);
        return (status);
    }

    return (NX_SUCCESS);
}


