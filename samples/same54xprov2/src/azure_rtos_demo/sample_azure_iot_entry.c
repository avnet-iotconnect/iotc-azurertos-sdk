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

#include "nx_api.h"
#include "nxd_dns.h"
#include "nxd_sntp_client.h"

#include "azrtos_time.h"
#include "app_config.h"

/* Include the sample.  */
extern VOID sample_entry(NX_IP* ip_ptr, NX_PACKET_POOL* pool_ptr, NX_DNS* dns_ptr, UINT (*unix_time_callback)(ULONG *unix_time));

#ifndef DEMO_SNTP_SYNC_MAX
#define DEMO_SNTP_SYNC_MAX            30
#endif /* DEMO_SNTP_SYNC_MAX */

#ifndef DEMO_SNTP_UPDATE_MAX
#define DEMO_SNTP_UPDATE_MAX          10
#endif /* DEMO_SNTP_UPDATE_MAX */

#ifndef DEMO_SNTP_UPDATE_INTERVAL
#define DEMO_SNTP_UPDATE_INTERVAL     (NX_IP_PERIODIC_RATE / 2)
#endif /* DEMO_SNTP_UPDATE_INTERVAL */

/* Default time. GMT: Friday, Jan 1, 2022 12:00:00 AM. Epoch timestamp: 1640995200.  */
#ifndef DEMO_SYSTEM_TIME 
#define DEMO_SYSTEM_TIME              1640995200
#endif /* DEMO_SYSTEM_TIME  */

/* Seconds between Unix Epoch (1/1/1970) and NTP Epoch (1/1/1999) */
#define DEMO_UNIX_TO_NTP_EPOCH_SECOND 0x83AA7E80

#if 0
static NX_SNTP_CLIENT   sntp_client;
#endif
/* System clock time for UTC.  */
static ULONG            unix_time_base;

#ifndef DEMO_SNTP_SERVER_NAME
static const CHAR *sntp_servers[] =
{
    "0.pool.ntp.org",
    "1.pool.ntp.org",
    "2.pool.ntp.org",
    "3.pool.ntp.org",
};
static UINT sntp_server_index;
#endif /* DEMO_SNTP_SERVER_NAME */

#if 0
static UINT sntp_time_sync(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr);
static UINT unix_time_get(ULONG *unix_time);
#endif
#include <stdbool.h>
extern bool iotconnect_sample_app(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr);

/* Define Azure IoT sample entry.  */
void sample_azure_iot_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr)
{
UINT    status;
    
    /* Sync up time by SNTP at start up.  */
    for (UINT i = 0; i < DEMO_SNTP_SYNC_MAX; i++)
    {

        /* Start SNTP to sync the local time.  */
        status = sntp_time_sync(ip_ptr, pool_ptr, dns_ptr, SAMPLE_SNTP_SERVER_NAME);

        /* Check status.  */
        if(status == NX_SUCCESS)
            break;

#ifndef DEMO_SNTP_SERVER_NAME

        /* Switch SNTP server every time.  */
        sntp_server_index = (sntp_server_index + 1) % (sizeof(sntp_servers) / sizeof(sntp_servers[0]));
#endif /* DEMO_SNTP_SERVER_NAME */
    }

    /* Check status.  */
    if (status)
    {
        printf("SNTP Time Sync failed.\r\n");
        printf("Set Time to default value: DEMO_SYSTEM_TIME.");
        unix_time_base = DEMO_SYSTEM_TIME;
    }
    else
    {
        printf("SNTP Time Sync successfully.\r\n");
    }

    /* Start sample.  */
    iotconnect_sample_app(ip_ptr, pool_ptr, dns_ptr);
}

#if 0
/* Sync up the local time.  */
static UINT sntp_time_sync(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr)
{

UINT    status;
UINT    server_status;
ULONG   sntp_server_address;
UINT    i;

#ifndef DEMO_SNTP_SERVER_NAME
    printf("SNTP Time Sync...%s\r\n", sntp_servers[sntp_server_index]);

    /* Look up SNTP Server address. */
    status = nx_dns_host_by_name_get(dns_ptr, (UCHAR *)sntp_servers[sntp_server_index], &sntp_server_address, 5 * NX_IP_PERIODIC_RATE);

    /* Check status.  */
    if (status)
    {
        return(status);
    }
#else /* !DEMO_SNTP_SERVER_NAME */
    printf("SNTP Time Sync...%s\r\n", DEMO_SNTP_SERVER_NAME);

    /* Look up SNTP Server address. */
    status = nx_dns_host_by_name_get(dns_ptr, (UCHAR *)DEMO_SNTP_SERVER_NAME, &sntp_server_address, 5 * NX_IP_PERIODIC_RATE);

    /* Check status.  */
    if (status)
    {
        return(status);
    }
#endif /* DEMO_SNTP_SERVER_NAME */

    /* Create the SNTP Client to run in broadcast mode.. */
    status =  nx_sntp_client_create(&sntp_client, ip_ptr, 0, pool_ptr,  
                                    NX_NULL,
                                    NX_NULL,
                                    NX_NULL /* no random_number_generator callback */);

    /* Check status.  */
    if (status)
    {
        return(status);
    }

    /* Use the IPv4 service to initialize the Client and set the IPv4 SNTP server. */
    status = nx_sntp_client_initialize_unicast(&sntp_client, sntp_server_address);

    /* Check status.  */
    if (status)
    {
        nx_sntp_client_delete(&sntp_client);
        return(status);
    }

    /* Set local time to 0 */
    status = nx_sntp_client_set_local_time(&sntp_client, 0, 0);

    /* Check status.  */
    if (status)
    {
        nx_sntp_client_delete(&sntp_client);
        return(status);
    }

    /* Run Unicast client */
    status = nx_sntp_client_run_unicast(&sntp_client);

    /* Check status.  */
    if (status)
    {
        nx_sntp_client_stop(&sntp_client);
        nx_sntp_client_delete(&sntp_client);
        return(status);
    }

    /* Wait till updates are received */
    for (i = 0; i < DEMO_SNTP_UPDATE_MAX; i++)
    {

        /* First verify we have a valid SNTP service running. */
        status = nx_sntp_client_receiving_updates(&sntp_client, &server_status);

        /* Check status.  */
        if ((status == NX_SUCCESS) && (server_status == NX_TRUE))
        {

            /* Server status is good. Now get the Client local time. */
            ULONG sntp_seconds, sntp_fraction;
            ULONG system_time_in_second;

            /* Get the local time.  */
            status = nx_sntp_client_get_local_time(&sntp_client, &sntp_seconds, &sntp_fraction, NX_NULL);

            /* Check status.  */
            if (status != NX_SUCCESS)
            {
                continue;
            }

            /* Get the system time in second.  */
            system_time_in_second = tx_time_get() / TX_TIMER_TICKS_PER_SECOND;

            /* Convert to Unix epoch and minus the current system time.  */
            unix_time_base = (sntp_seconds - (system_time_in_second + DEMO_UNIX_TO_NTP_EPOCH_SECOND));

            /* Time sync successfully.  */

            /* Stop and delete SNTP.  */
            nx_sntp_client_stop(&sntp_client);
            nx_sntp_client_delete(&sntp_client);

            return(NX_SUCCESS);
        }

        /* Sleep.  */
        tx_thread_sleep(DEMO_SNTP_UPDATE_INTERVAL);
    }

    /* Time sync failed.  */

    /* Stop and delete SNTP.  */
    nx_sntp_client_stop(&sntp_client);
    nx_sntp_client_delete(&sntp_client);

    /* Return success.  */
    return(NX_NOT_SUCCESSFUL);
}

static UINT unix_time_get(ULONG *unix_time)
{

    /* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).  */
    *unix_time =  unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

    return(NX_SUCCESS);
}
#endif
