/**
 ******************************************************************************
 * @file    metadata.c
 * @author  MCD Application Team
 * @brief   .
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "metadata.h"
#include "psa/protected_storage.h"

#define METADATA_UID 1 // ID in PSA storage

#define VERSION_01 "IOTC01" // metadata version


/* Private macro -------------------------------------------------------------*/
#define MODIFY_SSID     '1'
#define MODIFY_PASSWORD '2'
#define MODIFY_ENV      '3'
#define MODIFY_CPID     '4'
#define MODIFY_DUID     '5'
#define MODIFY_SYM_KEY  '6'
#define CLEAR_AND_RESET '9'
#define WRITE_AND_RESET '0'

static metadata_storage md;

char *pWIFI_SSID = md.wifi_ssid;
char *pWIFI_PASSWORD = md.wifi_pswd;

static uint32_t metadata_get_data(void);
static uint32_t metadata_write_data(void);
static uint32_t metadata_set_default(void);
static void flush_up_to_newline(void);

uint32_t metadata_init(void) {
	return metadata_get_data();
}

char metadata_display_menu(void) {
	char choice;
	printf("\r\n* Values for CPID and Environment must be set.");
	printf("\r\n* To use x509 authentication, leave symmetric key blank.");
	printf("\r\n* If Device Unique ID (DUID) is blank, auto-generated DUID will be used.");
	printf("\r\n  This value is displayed later during application startup");

	printf("\r\n%c - Set Wi-Fi SSID", MODIFY_SSID);
	printf("\r\n%c - Set Wi-Fi Password", MODIFY_PASSWORD);
	printf("\r\n%c - Set Environment", MODIFY_ENV);
	printf("\r\n%c - Set CPID", MODIFY_CPID);
	printf("\r\n%c - Set DUID", MODIFY_DUID);
	printf("\r\n%c - Set Symmetric Key", MODIFY_SYM_KEY);

	printf("\r\n%c - Clear all values and reset", CLEAR_AND_RESET);
	printf("\r\n%c - Write values and reset", WRITE_AND_RESET);
	printf("\r\n");

	choice = getchar();

	printf("\r\n");

	return choice;
}

uint32_t metadata_process_command(char command) {
	switch (command) {

	case MODIFY_SSID:
		printf("%s\r\n", "Enter Wi-Fi SSID:");
		scanf("%[^\r]31s", md.wifi_ssid);
		printf("WiFi SSID set to: \"%s\"\r\n", md.wifi_ssid);
		flush_up_to_newline();
		break;

	case MODIFY_PASSWORD:
		printf("%s\r\n", "Enter Wi-Fi Password:");
		scanf("%[^\r]63s", md.wifi_pswd);
		printf("WiFi Password set to: \"%s\"\r\n", md.wifi_pswd);
		flush_up_to_newline();
		break;

	case MODIFY_ENV:
		printf("%s\r\n", "Enter Environment:");
		scanf("%[^\r]15s", md.env);
		printf("Environment set to: \"%s\"\r\n", md.env);
		flush_up_to_newline();
		break;

	case MODIFY_CPID:
		printf("%s\r\n", "Enter CPID:");
		scanf("%[^\r]63s", md.cpid);
		printf("CPID set to: \"%s\"\r\n", md.cpid);
		flush_up_to_newline();
		break;

	case MODIFY_DUID:
		printf("%s\r\n", "Enter DUID:");
		scanf("%[^\r]63s", md.duid);
		printf("DUID set to: \"%s\"\r\n", md.duid);
		flush_up_to_newline();
		break;

	case MODIFY_SYM_KEY:
		printf("%s\r\n", "Enter Symmetric Key:");
		scanf("%[^\r]127s", md.symmetric_key);
		printf("Symmetric Key set to: \"%s\"\r\n", md.symmetric_key);
		flush_up_to_newline();
		break;

	case CLEAR_AND_RESET:
		metadata_set_default();
		metadata_write_data();
		NVIC_SystemReset();
		break;

	case WRITE_AND_RESET:
		metadata_write_data();
		NVIC_SystemReset();
		break;

	default:
		printf("%s\r\n", "Error: Re-Enter Number");
	}

	return METADATA_SUCCESS;
}

static uint32_t metadata_get_data(void) {
	int32_t err;
	size_t actual_size = 0;

	// clear local data first, in case there is any
	metadata_set_default();

	err = psa_ps_get(METADATA_UID, 0, sizeof(md), &md, &actual_size);

	if (err != BSP_ERROR_NONE) {
		printf("Failed to get metadata\r\n");
		return METADATA_ERROR;
	}

	if (strcmp(md.header, VERSION_01) != 0) {
		printf("WARNING: Incompatible data is stored. Clearing values...\r\n");
		metadata_set_default();
		// size won't match probably, so make sure we don't fail below
		actual_size = sizeof(md);
	}

	if (actual_size != sizeof(md)) {
		printf("Metadata size mismatch\r\n");
		return METADATA_ERROR;
	}

	printf("\r\n");
	printf("WiFi SSID     : %s\r\n", strlen(md.wifi_ssid) == 0 		? "(not set)" : md.wifi_ssid);
	printf("WiFi Password : %s\r\n", strlen(md.wifi_pswd) == 0 		? "(not set)" : "*******");
	printf("Environment   : %s\r\n", strlen(md.env) == 0 			? "(not set)" : md.env);
	printf("CPID          : %s\r\n", strlen(md.cpid) == 0 			? "(not set)" : md.cpid);
	printf("DUID          : %s\r\n", strlen(md.duid) == 0 			? "(not set)" : md.duid);
	printf("Symmetric Key : %s\r\n", strlen(md.symmetric_key) == 0 	? "(not set)" : "*****************");

	return METADATA_SUCCESS;
}

static uint32_t metadata_write_data(void) {
	int32_t err;

	err = psa_ps_set(METADATA_UID, sizeof(md), &md, 0);

	if (err != BSP_ERROR_NONE) {
		return METADATA_ERROR;
	}

	return METADATA_SUCCESS;
}

static uint32_t metadata_set_default(void) {
	memset(&md, 0, sizeof(md));
	strcpy(md.header, VERSION_01);
	return METADATA_SUCCESS;
}

static void flush_up_to_newline(void) {
	char cur;

	do {
		scanf("%c", &cur);
	} while (cur != '\r'); //\r -> CR || \n -> CR + LF (TeraTerm)
}

metadata_storage* metadata_get_values(void) {
    return &md;
}