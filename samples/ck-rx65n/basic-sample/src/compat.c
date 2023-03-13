#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <tx_api.h>

#include <nx_api.h>
#include <nx_ip.h>
#include <nxd_dns.h>
#include <nx_system.h>

#include <r_wifi_sx_ulpgn_if.h>

/* Compatibility functions between NetX and the Wi-Fi driver. */


int32_t az_platform_clock_msec(int64_t* out_clock_msec)
{
    return 0u;
}


int32_t az_platform_sleep_msec(int32_t milliseconds)
{
    return 0u;
}

