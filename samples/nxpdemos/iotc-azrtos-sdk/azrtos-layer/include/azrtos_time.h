//
// Copyright: Avnet, Softweb Inc. 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef AZRTOS_TIME_H
#define AZRTOS_TIME_H
#ifdef __cplusplus
extern   "C" {
#endif

#include <time.h>
#include "tx_api.h"
#include "nx_api.h"
#include "nxd_dns.h"

UINT sntp_time_sync(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr, const char *sntp_server_name);

// required function for azure RTOS
UINT unix_time_get(ULONG *unix_time);

#ifdef __cplusplus
}
#endif

#endif // AZRTOS_TIME_H

