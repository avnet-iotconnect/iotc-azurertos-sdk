/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2018-2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : secure_boot.h
 * Version      : 1.10
 * Device(s)    : RX Family
 * Description  : sample program of TSIP secure program.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 30.04.2018 1.05     First Release
 *         : 28.09.2018 1.06     Clean up source file
 *         : 28.12.2018 1.07     Clean up source file
 *         : 30.09.2019 1.08     Modify data structure, Added support for ECC
 *         : 31.03.2020 1.09     Added support for AES-CCM, HMAC
 *         : 30.06.2020 1.10     Added support for ARC4
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_tsip_rx_if.h"
#include "r_flash_rx_if.h"

#ifndef SECURE_BOOT_H_
#define SECURE_BOOT_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Status */
#define R_SECURE_BOOT_SUCCESS            (0)
#define R_SECURE_BOOT_FAIL               (-1)
#define R_SECURE_BOOT_GOTO_COMMAND_SHELL (-2)

/* File name */
#define INITIAL_FIRMWARE_FILE_NAME "userprog.rsu"

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/* Firmware load related */
typedef struct _load_firmware_control_block
{
    uint32_t flash_write_buffer[FLASH_CF_MEDIUM_BLOCK_SIZE / 4];
    uint32_t flash_read_buffer[FLASH_CF_MEDIUM_BLOCK_SIZE / 4];
    uint32_t offset;
    uint32_t progress;
    uint8_t  program_checksum[R_TSIP_AES_BLOCK_BYTE_SIZE];
    uint32_t firmware_length;
    uint32_t session_key0[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
    uint32_t session_key1[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
    uint32_t iv[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
} st_load_firmware_control_block_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern st_load_firmware_control_block_t g_load_firmware_control_block;
extern tsip_tls_ca_certification_public_key_index_t g_key_index_1;
extern tsip_update_key_ring_t g_key_index_2;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
void tsip_keyring_restore(void);
int32_t secure_boot(void);
int32_t reset_key_provisioning_lifecycle_state(void);
void tsip_print_installed_key_index(void);

#endif /* SECURE_BOOT_H_ */
