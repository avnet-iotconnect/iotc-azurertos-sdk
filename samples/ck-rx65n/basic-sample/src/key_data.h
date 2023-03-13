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
 * Copyright (C) 2018-2022 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : key_data.h
 * Version      : 1.16
 * Device(s)    : RX Family
 * Description  : sample program of TSIP Key data.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 28.02.2018 1.04     First Release
 *         : 30.04.2018 1.05     Add TDES key information
 *         : 28.09.2018 1.06     Add update key ring information
 *         : 28.12.2018 1.07     Add comment
 *         : 30.09.2019 1.08     Modify data structure, Added support for ECC
 *         : 31.03.2020 1.09     Removed the s_flash
 *         : 30.06.2020 1.10     Added support for ARC4
 *         : 30.09.2020 1.11     Added support for ECC(P-384)
 *         : 30.06.2021 1.12     Added support for device with TSIP and 8KB data flash memory
 *         : 31.08.2021 1.13     Modify data structure, Added support for HMAC
 *         : 22.10.2021 1.14     Resize tsip_hmac_sha_key_index_t structure
 *         : 31.03.2022 1.15     Change the description of pragma section
 *         : 30.06.2022 1.16     Added support for RSA(3072 and 4096 bit)
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "r_tsip_rx_if.h"

#ifndef KEY_DATA_H_
#define KEY_DATA_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/** Life cycle status **/
#define LIFECYCLE_STATE_BLANK             (0)
#define LIFECYCLE_STATE_ON_THE_MARKET     (1)
#define LIFECYCLE_STATE_UPDATING          (2)

/** Key block data address **/
#define KEY_BLOCK_DATA           (0x00100000)
#define KEY_BLOCK_DATA_MIRROR    (0x00104000)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
/** Firmware update data and user key datas */
typedef struct key_block_data
{
    /** State management data for update firmware */
    struct
    {
        uint32_t                         user_program_max_cnt;
        uint32_t                         lifecycle_state;
        uint32_t                         program_mac0[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
        uint32_t                         program_mac1[R_TSIP_AES_BLOCK_BYTE_SIZE / sizeof(uint32_t)];
    }
    firmware_update_control_data;

    /** User key datas */
    struct
    {
        uint8_t                          encrypted_user_rsa1024_ne_key[R_TSIP_RSA1024_NE_KEY_BYTE_SIZE + 16];
        uint8_t                          encrypted_user_rsa1024_nd_key[R_TSIP_RSA1024_ND_KEY_BYTE_SIZE + 16];
        uint8_t                          encrypted_provisioning_key[R_TSIP_AES_CBC_IV_BYTE_SIZE * 2];
        uint8_t                          iv[R_TSIP_AES_CBC_IV_BYTE_SIZE];
        tsip_rsa1024_public_key_index_t  user_rsa1024_ne_key_index;
        tsip_rsa1024_private_key_index_t user_rsa1024_nd_key_index;
    }
    key_data;

    uint8_t                              hash_sha1[R_TSIP_SHA1_HASH_LENGTH_BYTE_SIZE];
} st_key_block_data_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern const st_key_block_data_t g_key_block_data;
extern const st_key_block_data_t g_key_block_data_mirror;
extern st_key_block_data_t       g_key_block_image;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/

#endif /* KEY_DATA_H_ */

