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

/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** NetX Component                                                        */
/**                                                                       */
/**   User Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/
#ifndef NX_USER_H
#define NX_USER_H
#include "system/debug/sys_debug.h"
/*** NetX Configuration ***/
#define NX_PHYSICAL_HEADER                 (16 + 4 + 40)
#define NX_DEMO_PACKET_SIZE                1568
#define NX_DEMO_NUMBER_OF_PACKETS          20
#define NX_DEMO_PACKET_POOL_SIZE           (((NX_DEMO_PACKET_SIZE) + sizeof(NX_PACKET)) * (NX_DEMO_NUMBER_OF_PACKETS))
#define NX_DEMO_IP_STACK_SIZE              2048
#define NX_DEMO_IP_THREAD_PRIORITY         1
#define NX_DEMO_MAX_PHYSICAL_INTERFACES    1
#define AZURE_NET_INTERFACES               (NX_DEMO_MAX_PHYSICAL_INTERFACES)
    
#define NX_DEMO_DISABLE_LOOPBACK_INTERFACE     0
#define NX_DEMO_DISABLE_IPV6       1
#define NX_DEMO_DISABLE_IPV4               0
#define NX_DEMO_ENABLE_TCP         1
#define NX_DEMO_ENABLE_UDP         1
#define NX_DEMO_ENABLE_DHCP            1
#define NX_DEMO_IPV4_ADDRESS      IP_ADDRESS(0,0,0,0)
#define NX_DEMO_IPV4_MASK            IP_ADDRESS(0,0,0,0)
#define NX_DEMO_GATEWAY_ADDRESS        IP_ADDRESS(0,0,0,0)

#define NX_DEMO_DISABLE_IPV6       1
#define NX_DEMO_ENABLE_DNS            1
#define NX_DEMO_DNS_SERVER_ADDRESS             IP_ADDRESS(0,0,0,0)
#define NX_DNS_CLIENT_USER_CREATE_PACKET_POOL      1
#define NX_DEMO_ARP_CACHE_SIZE         1024
/*** Crypto Configuration ***/ 
#define NX_SECURE_ENABLE       1

/*** Azure IoT embedded C SDK Configuration ***/
#define NX_ENABLE_EXTENDED_NOTIFY_SUPPORT
#define NX_ENABLE_IP_PACKET_FILTER 
#define NXD_MQTT_CLOUD_ENABLE
#define NXD_MQTT_PING_TIMEOUT_DELAY        500
#define NXD_MQTT_SOCKET_TIMEOUT            0

#define NX_ENABLE_INTERFACE_CAPABILITY

#define printf(fmt, ...)    _SYS_DEBUG_PRINT(SYS_ERROR_INFO, fmt, ##__VA_ARGS__)
#endif  // NX_USER_H
