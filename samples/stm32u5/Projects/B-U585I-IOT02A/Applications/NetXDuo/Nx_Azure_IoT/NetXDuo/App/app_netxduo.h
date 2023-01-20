/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.h
  * @author  MCD Application Team
  * @brief   NetXDuo applicative header file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the ST_LICENSE file
  * in the root directory of this software component.
  * If no ST_LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_NETXDUO_H__
#define __APP_NETXDUO_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "nx_api.h"

/* USER CODE BEGIN Includes */
#include "main.h"
#include "nxd_dhcp_client.h"
#include "nxd_mqtt_client.h" 
#include "nxd_dns.h"  
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
  
/* Threads configuration */  
#define PAYLOAD_SIZE                1544
#define NX_PACKET_POOL_SIZE         (( PAYLOAD_SIZE + sizeof(NX_PACKET)) * 20)
#define DEFAULT_MEMORY_SIZE         1024
#define ARP_MEMORY_SIZE             DEFAULT_MEMORY_SIZE   
#define DEFAULT_MAIN_PRIORITY       10
#define DEFAULT_PRIORITY            5  
#define APP_PRIORITY                16  
#define THREAD_MEMORY_SIZE          8 * DEFAULT_MEMORY_SIZE

/* application configuration */
#define NULL_ADDRESS                0  
/* if USER_DNS_ADDRESS is not defined, the DNS server is retrieved from DHCP answer */
/*
#define USER_DNS_ADDRESS            IP_ADDRESS(1, 1, 1, 1)
*/

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define PRINT_IP_ADDRESS(string, addr)           do { \
                                              printf(string " %lu.%lu.%lu.%lu\r\n", \
                                                ((addr) >> 24) & 0xff,                        \
                                                  ((addr) >> 16) & 0xff,                      \
                                                    ((addr) >> 8) & 0xff,                     \
                                                      ((addr) & 0xff));                       \
                                            } while(0)      
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT MX_NetXDuo_Init(VOID *memory_ptr);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_NETXDUO_H__ */
