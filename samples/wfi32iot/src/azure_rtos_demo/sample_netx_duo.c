/* This is a small demo of the high-performance NetX Duo TCP/IP stack.  */

#include   "tx_api.h"
#include   "nx_api.h"
#if (NX_DEMO_ENABLE_DHCP != 0)
#include   "nxd_dhcp_client.h"
#endif  // (NX_DEMO_ENABLE_DHCP != 0)
#if (NX_DEMO_ENABLE_DNS != 0)
#include   "nxd_dns.h"
#endif  // (NX_DEMO_ENABLE_DNS != 0)

/* Define demo stack size.   */

#ifndef DEMO_STACK_SIZE
#define DEMO_STACK_SIZE         8196
#endif

#ifndef DEMO_THREAD_PRIORITY
#define DEMO_THREAD_PRIORITY    4
#endif


/* Define the ThreadX and NetX object control blocks...  */
TX_THREAD               thread_0;
NX_PACKET_POOL          pool_0;
NX_IP                   ip_0;
#if (NX_DEMO_ENABLE_DHCP != 0)
NX_DHCP                 dhcp_0;
#endif  // (NX_DEMO_ENABLE_DHCP != 0)
#if (NX_DEMO_ENABLE_DNS != 0)
NX_DNS                  dns_0;
#endif  // (NX_DEMO_ENABLE_DNS != 0)
UCHAR                   *http_stack;
UCHAR                   *iperf_stack;
#if (NX_DEMO_DISABLE_IPV6 == 0)
NXD_ADDRESS             ipv6_address;
#endif  // (NX_DEMO_DISABLE_IPV6 == 0)

/* Define the stack/cache for ThreadX.  */
ULONG demo_thread_stack[DEMO_STACK_SIZE / sizeof(ULONG)];
ULONG demo_ip_stack[NX_DEMO_IP_STACK_SIZE / sizeof(ULONG)];
ULONG demo_pool_stack[NX_DEMO_PACKET_POOL_SIZE / sizeof(ULONG)];
ULONG demo_arp_cache_area[NX_DEMO_ARP_CACHE_SIZE / sizeof(ULONG)];

/* Define the counters used in the demo application...  */
ULONG                   error_counter;

/* Define thread prototypes.  */
VOID    thread_0_entry(ULONG thread_input);
extern  void sample_azure_iot_entry(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr);
              
/***** Substitute your ethernet driver entry function here *********/
extern  VOID nx_driver_harmony(NX_IP_DRIVER*); 

#if (NX_DEMO_ENABLE_DHCP != 0)
static void dhcp_wait();
#endif  // (NX_DEMO_ENABLE_DHCP != 0)

#if (NX_DEMO_ENABLE_DNS != 0)
static UINT dns_create();
#endif  // (NX_DEMO_ENABLE_DNS != 0)


/* Define what the initial system looks like.  */
void nx_azure_init(void)
{

UINT    status;

    /* Initialize the NetX system.  */
    nx_system_initialize();
    
    /* Create a packet pool.  */ 
    status = nx_packet_pool_create(&pool_0, "NetX Main Packet Pool", 
                                   NX_DEMO_PACKET_SIZE,
                                   demo_pool_stack, sizeof(demo_pool_stack)); 
                
    /* Check for packet pool create errors.  */
    if (status)
        error_counter++;     
    
    /* Create an IP instance.  */
    status = nx_ip_create(&ip_0, "NetX IP Instance 0", 
                          NX_DEMO_IPV4_ADDRESS, 
                          NX_DEMO_IPV4_MASK, 
                          &pool_0, 
                          nx_driver_harmony,
                          (UCHAR *)demo_ip_stack, 
                          sizeof(demo_ip_stack), 
                          NX_DEMO_IP_THREAD_PRIORITY);
          
    /* Check for IP create errors.  */
    if (status)
        error_counter++;

#if (NX_DEMO_ENABLE_DHCP != 0)
    status = nx_ip_gateway_address_set(&ip_0, NX_DEMO_GATEWAY_ADDRESS);
    if (status)
        error_counter++;
    
#endif  // (NX_DEMO_ENABLE_DHCP != 0)


    /* Enable ARP and supply ARP cache memory for IP Instance 0.  */
    status =  nx_arp_enable(&ip_0, (void *)demo_arp_cache_area, sizeof(demo_arp_cache_area));

    /* Check for ARP enable errors.  */
    if (status)
        error_counter++;

    /* Enable ICMP */
    status = nx_icmp_enable(&ip_0); 

    /* Check for ICMP enable errors.  */
    if(status)
        error_counter++;

#if (NX_DEMO_ENABLE_UDP != 0)    
    /* Enable UDP traffic.  */
    status =  nx_udp_enable(&ip_0);

    /* Check for UDP enable errors.  */
    if (status)
        error_counter++;
#endif  // (NX_DEMO_ENABLE_UDP != 0)    

#if (NX_DEMO_ENABLE_TCP != 0)
    /* Enable TCP traffic.  */
    status =  nx_tcp_enable(&ip_0); 

    /* Check for TCP enable errors.  */
    if (status)
        error_counter++;
#endif  // (NX_DEMO_ENABLE_TCP != 0)

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,  
                     demo_thread_stack, sizeof(demo_thread_stack), 
                     DEMO_THREAD_PRIORITY, DEMO_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
    
#if (NX_DEMO_DISABLE_IPV6 == 0)
    /* Set up the IPv6 address here. */
    ipv6_address.nxd_ip_address.v6[3] = 0x3;
    ipv6_address.nxd_ip_address.v6[2] = 0x0;
    ipv6_address.nxd_ip_address.v6[1] = 0x0;
    ipv6_address.nxd_ip_address.v6[0] = 0xfe800000;
    ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;

    /* Enable ICMPv6 services. */
    status = nxd_icmp_enable(&ip_0);
    if (status)
        error_counter++;

    /* Enable IPv6 services. */
    status = nxd_ipv6_enable(&ip_0);
    if (status)
        error_counter++;

    status = nxd_ipv6_address_set(&ip_0, 0, &ipv6_address, 10, NX_NULL);
    if (status)
        error_counter++;
#endif  // (NX_DEMO_DISABLE_IPV6 == 0)
}

/* Define the test threads.  */
void    thread_0_entry(ULONG thread_input)
{

#if (NX_DEMO_ENABLE_DNS != 0)
    UINT    status;
#endif  // (NX_DEMO_ENABLE_DNS != 0)
    ULONG   ip_address = 0;
    ULONG   network_mask = 0;
    ULONG   gateway_address = 0;
    
#if (NX_DEMO_ENABLE_DHCP != 0)
    dhcp_wait();
#endif  // (NX_DEMO_ENABLE_DHCP != 0)

    /* Get IP address and gateway address. */
    nx_ip_address_get(&ip_0, &ip_address, &network_mask);
    nx_ip_gateway_address_get(&ip_0, &gateway_address);

    /* Output IP address and gateway address. */
    printf("IP address: %lu.%lu.%lu.%lu\r\n",
           (ip_address >> 24),
           (ip_address >> 16 & 0xFF),
           (ip_address >> 8 & 0xFF),
           (ip_address & 0xFF));
    printf("Mask: %lu.%lu.%lu.%lu\r\n",
           (network_mask >> 24),
           (network_mask >> 16 & 0xFF),
           (network_mask >> 8 & 0xFF),
           (network_mask & 0xFF));
    printf("Gateway: %lu.%lu.%lu.%lu\r\n",
           (gateway_address >> 24),
           (gateway_address >> 16 & 0xFF),
           (gateway_address >> 8 & 0xFF),
           (gateway_address & 0xFF));


#if (NX_DEMO_ENABLE_DNS != 0)
    status = dns_create();
    if (status)
        error_counter++;
#endif  // (NX_DEMO_ENABLE_DNS != 0)

#if (NX_DEMO_DISABLE_IPV6 == 0)
    tx_thread_sleep(NX_IP_PERIODIC_RATE);
#endif  // (NX_DEMO_DISABLE_IPV6 == 0)

    /* Start the Azure IoT demo */
    sample_azure_iot_entry(&ip_0, &pool_0, &dns_0);
}

#if (NX_DEMO_ENABLE_DHCP != 0)
static void dhcp_wait()
{
ULONG   actual_status;

    printf("DHCP In Progress...\r\n");

    /* Create the DHCP instance.  */
    nx_dhcp_create(&dhcp_0, &ip_0, "DHCP Client");

    /* Start the DHCP Client.  */
    nx_dhcp_start(&dhcp_0);

    /* Wait util address is solved. */
    nx_ip_status_check(&ip_0, NX_IP_ADDRESS_RESOLVED, &actual_status, NX_WAIT_FOREVER);
}
#endif  // (NX_DEMO_ENABLE_DHCP != 0)

#if (NX_DEMO_ENABLE_DNS != 0)

static UINT dns_create()
{

UINT    status;
ULONG   dns_server_address[4];
#if (NX_DEMO_ENABLE_DHCP != 0)
UINT    dns_server_address_size = 16;
#endif  // (NX_DEMO_ENABLE_DHCP != 0)

    /* Create a DNS instance for the Client.  Note this function will create
       the DNS Client packet pool for creating DNS message packets intended
       for querying its DNS server. */
    status = nx_dns_create(&dns_0, &ip_0, (UCHAR *)"DNS Client");
    if (status)
    {
        return(status);
    }

    /* Is the DNS client configured for the host application to create the pecket pool? */
#if (NX_DNS_CLIENT_USER_CREATE_PACKET_POOL != 0)

    /* Yes, use the packet pool created above which has appropriate payload size
       for DNS messages. */
    status = nx_dns_packet_pool_set(&dns_0, ip_0.nx_ip_default_packet_pool);
    if (status)
    {
        nx_dns_delete(&dns_0);
        return(status);
    }
#endif  // (NX_DNS_CLIENT_USER_CREATE_PACKET_POOL != 0)

#if (NX_DEMO_ENABLE_DHCP != 0)
    /* Retrieve DNS server address.  */
    nx_dhcp_interface_user_option_retrieve(&dhcp_0, 0, NX_DHCP_OPTION_DNS_SVR, (UCHAR *)(dns_server_address),
                                           &dns_server_address_size);
#else
    dns_server_address[0] = NX_DEMO_DNS_SERVER_ADDRESS;
#endif  // (NX_DEMO_ENABLE_DHCP != 0)

    /* Add an IPv4 server address to the Client list. */
    status = nx_dns_server_add(&dns_0, dns_server_address[0]);
    if (status)
    {
        nx_dns_delete(&dns_0);
        return(status);
    }

    /* Output DNS Server address.  */
    printf("DNS Server address: %lu.%lu.%lu.%lu\r\n",
           (dns_server_address[0] >> 24),
           (dns_server_address[0] >> 16 & 0xFF),
           (dns_server_address[0] >> 8 & 0xFF),
           (dns_server_address[0] & 0xFF));

    return(NX_SUCCESS);
}
#endif  // (NX_DEMO_ENABLE_DNS != 0)

