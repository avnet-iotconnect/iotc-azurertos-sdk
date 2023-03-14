#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <tx_api.h>

#include <nx_api.h>
#include <nx_ip.h>
#define NX_DISABLE_ERROR_CHECKING
#include <nxd_dns.h>
#undef NX_DISABLE_ERROR_CHECKING
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


/* Bring in externs for caller checking code.  */
NX_CALLER_CHECKING_EXTERNS
UINT  _nxde_dns_host_by_name_get(NX_DNS *dns_ptr, UCHAR *host_name, NXD_ADDRESS *host_address_ptr,
                                 ULONG wait_option, UINT lookup_type)
{

UINT    status;

    /* Check for invalid pointer input.  */
    if ((dns_ptr == NX_NULL) || (host_name == NX_NULL) || (host_address_ptr == NX_NULL))
        return(NX_PTR_ERROR);

    /* Check for invalid non pointer input. */
    if ((dns_ptr -> nx_dns_id != NX_DNS_ID) || (lookup_type == 0))
    {
        return NX_DNS_PARAM_ERROR;
    }

    /* Check for appropriate caller.  */
    NX_THREADS_ONLY_CALLER_CHECKING

    /* Call actual DNS get host by name service.  */
    status =  _nxd_dns_host_by_name_get(dns_ptr, host_name, host_address_ptr, wait_option, lookup_type);

    /* Return status.  */
    return(status);
}
