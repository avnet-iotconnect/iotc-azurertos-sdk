#include "nx_api.h"
#include "wifi.h"

VOID sample_network_configure(NX_IP *ip_ptr, ULONG *dns_address)
{
ULONG   ip_address = 0;
ULONG   network_mask = 0;
ULONG   gateway_address = 0;
UINT    status;
ULONG   dns_address_second;

    WIFI_GetIP_Address((UCHAR *)&ip_address);
    WIFI_GetIP_Mask((UCHAR *)&network_mask);
    WIFI_GetGateway_Address((UCHAR *)&gateway_address);
    NX_CHANGE_ULONG_ENDIAN(ip_address);
    NX_CHANGE_ULONG_ENDIAN(network_mask);
    NX_CHANGE_ULONG_ENDIAN(gateway_address);

    if (dns_address)
    {
        WIFI_GetDNS_Address((UCHAR *)dns_address, (UCHAR *)&dns_address_second);
        NX_CHANGE_ULONG_ENDIAN(*dns_address);
    }

    status = nx_ip_address_set(ip_ptr, ip_address, network_mask);

    /* Check for IP address set errors.  */
    if (status)
    {
        printf("IP ADDRESS SET FAIL.\r\n");
        return;
    }

    status = nx_ip_gateway_address_set(ip_ptr, gateway_address);

    /* Check for gateway address set errors.  */
    if (status)
    {
        printf("IP GATEWAY ADDRESS SET FAIL.\r\n");
        return;
    }
}