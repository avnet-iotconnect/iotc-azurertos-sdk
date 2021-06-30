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
#include "nx_api.h"
#include "tx_api.h"
#include "nxd_dns.h"
#include "nxd_sntp_client.h"
#include "azrtos_time.h"

#ifndef SAMPLE_SNTP_UPDATE_MAX
#define SAMPLE_SNTP_UPDATE_MAX            10
#endif /* SAMPLE_SNTP_UPDATE_MAX */

#ifndef SAMPLE_SNTP_UPDATE_INTERVAL
#define SAMPLE_SNTP_UPDATE_INTERVAL       (NX_IP_PERIODIC_RATE / 2)
#endif /* SAMPLE_SNTP_UPDATE_INTERVAL */

/* Seconds between Unix Epoch (1/1/1970) and NTP Epoch (1/1/1999) */
#define SAMPLE_UNIX_TO_NTP_EPOCH_SECOND   0x83AA7E80


static NX_SNTP_CLIENT   sntp_client;

/* System clock time offset for UTC.  */
static ULONG            unix_time_base;

/* Sync up the local time.  */
UINT sntp_time_sync(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, const char *sntp_server_name)
{

UINT    status;
UINT    server_status;
ULONG   sntp_server_address;
UINT    i;


    printf("SNTP Time Sync...\r\n");
    status = nx_dns_host_by_name_get(dns_ptr, (UCHAR *)sntp_server_name, &sntp_server_address, 5 * NX_IP_PERIODIC_RATE);

    /* Check status.  */
    if (status)
    {
        return(status);
    }

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
    for (i = 0; i < SAMPLE_SNTP_UPDATE_MAX; i++)
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
            unix_time_base = (sntp_seconds - (system_time_in_second + SAMPLE_UNIX_TO_NTP_EPOCH_SECOND));
            /* Time sync successfully.  */

            /* Stop and delete SNTP.  */
            nx_sntp_client_stop(&sntp_client);
            nx_sntp_client_delete(&sntp_client);

            return(NX_SUCCESS);
        }

        /* Sleep.  */
        tx_thread_sleep(SAMPLE_SNTP_UPDATE_INTERVAL);
    }

    /* Time sync failed.  */

    /* Stop and delete SNTP.  */
    nx_sntp_client_stop(&sntp_client);
    nx_sntp_client_delete(&sntp_client);

    /* Return success.  */
    return(NX_NOT_SUCCESSFUL);
}

UINT unix_time_get(ULONG *unix_time)
{

    /* Return number of seconds since Unix Epoch (1/1/1970 00:00:00).  */
    *unix_time =  unix_time_base + (tx_time_get() / TX_TIMER_TICKS_PER_SECOND);

    return(NX_SUCCESS);
}

#ifdef IOTC_NEEDS_C_TIME
time_t time(time_t *t) {
	ULONG time_now;
	if (NX_SUCCESS != unix_time_get(&time_now)) {
		return -1;
	}
	return (time_t) time_now;
}
#endif


#if defined(IOTC_NEEDS_GETTIMEOFDAY) || defined(IOTC_NEEDS_GETTIMEOFDAY_OU)
// _gettimeofday override provides access to time() function that's needed for the IoTConnect C library
#ifdef IOTC_NEEDS_GETTIMEOFDAY
int __gettimeofday(struct timeval *tv, void *tzvp) {
#else // IOTC_NEEDS_GETTIMEOFDAY_OU with one underscore
int _gettimeofday(struct timeval *tv, void *tzvp) {
#endif
    // if either no time, or someone is trying to use timezone offset, we cannot support
	ULONG time_now;
    if (NX_SUCCESS != unix_time_get(&time_now)) {
    	return -1;
    }
    tv->tv_sec = (time_t)time_now;
    tv->tv_usec = 0L;
    return 0;  // return non-zero for error
} // end _gettimeofday()
#endif

