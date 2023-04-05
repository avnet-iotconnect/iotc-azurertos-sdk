/**
  ******************************************************************************
  * @file    metadata.h
  * @author  MCD Application Team
  * @brief   
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


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef METADATA_H
#define METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "mx_wifi.h"
  
#define METADATA_SUCCESS 0
#define METADATA_ERROR   (!METADATA_SUCCESS)

#define MD_VERSION_STR_SIZE 8
#define MD_ENV_SIZE         16
#define MD_CPID_SIZE        64
#define MD_DUID_SIZE        64 // really max duid is 64 + 1, so making it nicely packed for now
#define MD_SYM_KEY_SIZE     128

typedef struct __attribute__((__packed__)) metadata_storage {
	char header[MD_VERSION_STR_SIZE];
	char wifi_ssid[MX_MAX_SSID_LEN]; // defined in mx_wifi.h
	char wifi_pswd[MX_MAX_KEY_LEN]; // defined in mx_wifi.h
	char env[MD_ENV_SIZE];
	char cpid[MD_CPID_SIZE];
	char duid[MD_DUID_SIZE];
	char symmetric_key[MD_SYM_KEY_SIZE];
} metadata_storage;

uint32_t metadata_init(void);
char     metadata_display_menu(void);
uint32_t metadata_process_command(char command);
metadata_storage* metadata_get_values(void);

#ifdef __cplusplus
}
#endif

#endif /* METADATA_H */

