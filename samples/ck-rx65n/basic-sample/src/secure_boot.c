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
 * File Name    : secure_boot.c
 * Version      : 1.16
 * Device(s)    : RX Family
 * Description  : sample program of TSIP secure program.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 30.04.2018 1.05     First Release
 *         : 28.09.2018 1.06     Add RSA Key Generation, AES, TDES, RSA Key update features, RX66T support
 *         : 28.12.2018 1.07     Add RX72T support
 *         : 30.09.2019 1.08     Added support for GCC and IAR compiler, ECC API, RX23W and RX72M
 *         : 31.03.2020 1.09     Added support for AES-CCM, HMAC key generation, ECDH, Key Wrap API, RX66N and RX72N
 *         : 30.06.2020 1.10     Added support for ARC4, ECC(P-384) API
 *         : 30.09.2020 1.11     Added support for DH, ECDHE P-512r1 API, and generalization of KDF.
 *         :                     Added support for Key wrap API with TSIP-Lite.
 *         : 30.06.2021 1.12     Added support for RX23W chip version D
 *         : 31.08.2021 1.13     Added support for RX671
 *         : 22.10.2021 1.14     Added support for TLS1.3
 *         : 31.03.2022 1.15     Added support for TLS1.3(RX72M_RX72N_RX66N)
 *         : 30.06.2022 1.16     Added support for RSA 3k/4k and updated support for TLS1.3
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include "r_tsip_rx_if.h"
#include "r_flash_rx_if.h"
#include "r_sci_rx_if.h"
#include "secure_boot.h"
#include "key_data.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* Status */
#define SECURE_BOOT_SUCCESS         (0)
#define SECURE_BOOT_FAIL            (-1)
#define SECURE_BOOT_GOTO_INSTALL    (-2)
#define SECURE_BOOT_NOT_FOUND_FILE  (-3)

/* Secure boot related */
#define MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT (3)
#define SECURE_BOOT_PARAMETER_NUMBER         (5)

/* Data Flash bank */
#define DF_BANK_MAIN    (0)
#define DF_BANK_MIRROR  (1)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
st_load_firmware_control_block_t g_load_firmware_control_block;
char g_print_buffer[512];

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static const char s_lifecycle_string[][64] =
{
    {
        "LIFECYCLE_STATE_BLANK"
    },
    {
        "LIFECYCLE_STATE_ON_THE_MARKET"
    },
    {
        "LIFECYCLE_STATE_UPDATING"
    },
};

static int32_t secure_boot_rsa_key_generate(void);

static int32_t check_secure_boot_status_blank(void);
static int32_t check_secure_boot_status_on_the_market(uint32_t bank_info);
static int32_t check_secure_boot_status(void);
static int32_t compare_sha1_value(uint8_t *pmsg_buff, uint32_t msg_legth, uint8_t *psrc_sha1);
static void calc_sha1_value(uint8_t *pmsg_buff, uint32_t msg_legth, uint8_t *sha1_val);
static void check_dataflash_area(uint32_t retry_counter);
static void software_reset(void);
static void update_dataflash_data_from_image(uint32_t select_df_bank);
static void tsip_install_keyring(void);

/**********************************************************************************************************************
 * Function Name: secure_boot
 * Description  : key install and check data flash
 * Arguments    : none
 * Return Value : R_SECURE_BOOT_SUCCESS - 
 *                    Secure boot success.
 *                R_SECURE_BOOT_FAIL - 
 *                    Secure boot failure.
 *********************************************************************************************************************/
int32_t secure_boot(void)
{
    int32_t result_secure_boot = SECURE_BOOT_SUCCESS;

    result_secure_boot = check_secure_boot_status();
    if (SECURE_BOOT_SUCCESS == result_secure_boot)
    {
        return R_SECURE_BOOT_SUCCESS;
    }
    else if (SECURE_BOOT_GOTO_INSTALL == result_secure_boot)
    {
        R_TSIP_Open(NULL, NULL);
        printf("========== generate user key index phase ==========\r\n");
        if (R_SECURE_BOOT_SUCCESS != secure_boot_rsa_key_generate())
        {
            R_TSIP_Close();
            return R_SECURE_BOOT_FAIL;
        }

        memset(&g_key_block_image.key_data, 0xFF,
        /* Casting uint32_t is used for address. */
        (uint32_t)g_key_block_image.key_data.iv + R_TSIP_AES_CBC_IV_BYTE_SIZE - (uint32_t)&g_key_block_image.key_data);
        /* Casting uint8_t pointer is used for address. */
        calc_sha1_value((uint8_t*)&g_key_block_image.firmware_update_control_data,
        (sizeof(g_key_block_image.firmware_update_control_data)) + (sizeof(g_key_block_image.key_data)),
        g_key_block_image.hash_sha1);

        printf("========== install user key index phase ==========\r\n");
        g_key_block_image.firmware_update_control_data.lifecycle_state = LIFECYCLE_STATE_UPDATING;

        tsip_install_keyring();

        result_secure_boot = R_SECURE_BOOT_SUCCESS;
        printf("Software Reset ...\r\n");
        R_BSP_SoftwareDelay(3000, BSP_DELAY_MILLISECS);
        R_TSIP_Close();
        software_reset();
    }
    else if (SECURE_BOOT_FAIL == result_secure_boot)
    {
        printf("secure boot sequence: fail.\r\n");
        result_secure_boot = R_SECURE_BOOT_FAIL;
    }
    else
    {
        printf("unknown status.\r\n");
        result_secure_boot = R_SECURE_BOOT_FAIL;
    }
    return result_secure_boot;
}
/**********************************************************************************************************************
 End of function secure_boot
 *********************************************************************************************************************/

int32_t reset_key_provisioning_lifecycle_state(void)
{
#ifdef ERASABLE_KEYS
	g_key_block_image.firmware_update_control_data.lifecycle_state = LIFECYCLE_STATE_BLANK;
	memcpy(&g_key_block_image, &g_original_key_block_data, sizeof(st_key_block_data_t));
	tsip_install_keyring();
#endif //ERASABLE_KEYS
	return R_SECURE_BOOT_SUCCESS;
}
/**********************************************************************************************************************
 End of function reset_key_provisioning_lifecycle_state
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_secure_boot_status
 * Description  : check data flash status
 * Arguments    : none
 * Return Value : SECURE_BOOT_SUCCESS - 
 *                    Secure boot success.
 *                SECURE_BOOT_GOTO_INSTALL - 
 *                    Transition to installation phase.
 *                SECURE_BOOT_FAIL - 
 *                    Secure boot failure.
 *********************************************************************************************************************/
static int32_t check_secure_boot_status(void)
{
    int32_t secure_boot_error_code = SECURE_BOOT_SUCCESS;
    uint32_t bank_info = 255;

    printf("Checking flash ROM status.\r\n");

    printf("status = %s\r\n", 
            s_lifecycle_string[g_key_block_image.firmware_update_control_data.lifecycle_state]);
    R_FLASH_Control(FLASH_CMD_BANK_GET, &bank_info);
    printf("bank info = %u. (start bank = %u)\r\n", bank_info, (bank_info ^ 0x01));

    switch (g_key_block_image.firmware_update_control_data.lifecycle_state)
    {
        case LIFECYCLE_STATE_BLANK:
            if (R_SECURE_BOOT_SUCCESS != check_secure_boot_status_blank())
            {
                secure_boot_error_code = SECURE_BOOT_FAIL;
            }
            else
            {
                secure_boot_error_code = SECURE_BOOT_GOTO_INSTALL;
            }
        break;
        case LIFECYCLE_STATE_UPDATING:
            printf("update data flash\r\n");
            g_key_block_image.firmware_update_control_data.lifecycle_state = LIFECYCLE_STATE_ON_THE_MARKET;
            tsip_install_keyring();
        /*break;*/

        /* In this case, next state "LIFECYCLE_STATE_ON_THE_MARKET" is needed to execute (no break) */
        case LIFECYCLE_STATE_ON_THE_MARKET:
            if (R_SECURE_BOOT_SUCCESS != check_secure_boot_status_on_the_market(bank_info))
            {
                secure_boot_error_code = SECURE_BOOT_FAIL;
            }
            else
            {
                secure_boot_error_code = SECURE_BOOT_SUCCESS;
            }
        break;
        default:
            printf("illegal flash ROM status code 0x%x.\r\n",
                    g_key_block_image.firmware_update_control_data.lifecycle_state);
            check_dataflash_area(0);
            R_BSP_SoftwareDelay(1000, BSP_DELAY_MILLISECS);
            secure_boot_error_code = SECURE_BOOT_FAIL;
        break;
    }
    return secure_boot_error_code;
}
/**********************************************************************************************************************
 End of function check_secure_boot_status
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_dataflash_area
 * Description  : check data flash key area
 * Arguments    : retry_counter - 
 *                    Retry counter
 * Return Value : none
 *********************************************************************************************************************/
static void check_dataflash_area(uint32_t retry_counter)
{
    if (retry_counter)
    {
        printf("recover retry count = %d.\r\n", retry_counter);
        if (MAX_CHECK_DATAFLASH_AREA_RETRY_COUNT == retry_counter)
        {
            printf("retry over the limit.\r\n");
            while (1)
            {
                R_BSP_NOP();
            }
        }
    }
    printf("data flash(main) hash check...");

    if (R_SECURE_BOOT_SUCCESS
            /* Casting uint8_t pointer is used for address. */
            == compare_sha1_value((uint8_t*)&g_key_block_image.firmware_update_control_data,
            (sizeof(g_key_block_image.firmware_update_control_data)) + (sizeof(g_key_block_image.key_data)),
            /* Casting uint8_t pointer is used for address. */
            (uint8_t*)&g_key_block_image.hash_sha1))
    {
        printf("OK\r\n");
        printf("data flash(mirror) hash check...");

        if (R_SECURE_BOOT_SUCCESS
            /* Casting uint8_t pointer is used for address. */
            == compare_sha1_value((uint8_t*)&g_key_block_image.firmware_update_control_data,
            (sizeof(g_key_block_image.firmware_update_control_data)) + (sizeof(g_key_block_image.key_data)),
            /* Casting uint8_t pointer is used for address. */
            (uint8_t*)&g_key_block_data_mirror.hash_sha1))
        {
            printf("OK\r\n");
        }
        else
        {
            printf("NG\r\n");
            printf("recover mirror from main.\r\n");
            memcpy(&g_key_block_image, &g_key_block_data, sizeof(g_key_block_data));
            update_dataflash_data_from_image(DF_BANK_MIRROR);
            check_dataflash_area(retry_counter + 1);
        }
    }
    else
    {
        printf("NG\r\n");
        printf("data flash(mirror) hash check...");

        if (R_SECURE_BOOT_SUCCESS
            /* Casting uint8_t pointer is used for address. */
            == compare_sha1_value((uint8_t*)&g_key_block_image.firmware_update_control_data,
            (sizeof(g_key_block_image.firmware_update_control_data)) + (sizeof(g_key_block_image.key_data)),
            /* Casting uint8_t pointer is used for address. */
            (uint8_t*)&g_key_block_data_mirror.hash_sha1))
        {
            printf("OK\r\n");
            printf("recover main from mirror.\r\n");
            memcpy(&g_key_block_image, &g_key_block_data_mirror, sizeof(g_key_block_data_mirror));
            update_dataflash_data_from_image(DF_BANK_MAIN);
            check_dataflash_area(retry_counter + 1);
        }
        else
        {
            printf("NG\r\n");
            printf("Data flash is completely broken.\r\n");
            printf("Please erase all code flash.\r\n");
            printf("And, write secure boot using debugger.\r\n");
            while (1)
            {
                R_BSP_NOP();
            }
        }
    }
    return;
}
/**********************************************************************************************************************
 End of function check_dataflash_area
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: update_dataflash_data_from_image
 * Description  : update data flash
 * Arguments    : select_df_bank - 
 *                    Select bank for data flash to update.(DF_BANK_MAIN or DF_BANK_MIRROR)
 * Return Value : none
 *********************************************************************************************************************/
static void update_dataflash_data_from_image(uint32_t select_df_bank)
{
    uint32_t required_dataflash_block_num = 0;
    flash_err_t flash_error_code = FLASH_SUCCESS;
    flash_block_address_t key_index_block_data_address = FLASH_DF_BLOCK_0;
    char const *p_df_bank[] =
    {
        "main", "mirror"
    };

    if (DF_BANK_MAIN == select_df_bank)
    {
        /* Casting flash_block_address_t is Data Flash address. */
        key_index_block_data_address = (flash_block_address_t)&g_key_block_data;
    }
    else    /* DF_BANK_MIRROR == select_df_bank */
    {
        /* Casting flash_block_address_t is Data Flash address. */
        key_index_block_data_address = (flash_block_address_t)&g_key_block_data_mirror;
    }

    required_dataflash_block_num = ((sizeof(st_key_block_data_t)) + (FLASH_DF_BLOCK_SIZE - 1)) / FLASH_DF_BLOCK_SIZE;

    printf("erase data flash(%s)...", p_df_bank[select_df_bank]);

    flash_error_code = R_FLASH_Erase(key_index_block_data_address, required_dataflash_block_num);
    if (FLASH_SUCCESS == flash_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_FLASH_Erase() returns error code = %d.\r\n", flash_error_code);
        return;
    }

    printf("write data flash(%s)...", p_df_bank[select_df_bank]);

    /* Casting uint32_t is Data Flash address. */
    flash_error_code = R_FLASH_Write((uint32_t)&g_key_block_image, (uint32_t)key_index_block_data_address,
            sizeof(st_key_block_data_t));
    if (FLASH_SUCCESS == flash_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_FLASH_Write() returns error code = %d.\r\n", flash_error_code);
        return;
    }
    if (DF_BANK_MAIN != select_df_bank)
    {
        if (!memcmp(&g_key_block_data, &g_key_block_data_mirror, sizeof(st_key_block_data_t)))
        {
            printf("data flash setting OK.\r\n");
        }
        else
        {
            printf("data flash setting NG.\r\n");
        }
    }
    return;
}
/**********************************************************************************************************************
 End of function update_dataflash_data_from_image
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: software_reset
 * Description  : execute Soft Reset
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void software_reset(void)
{
    R_BSP_InterruptsDisable();
    R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_LPC_CGC_SWR);
    /* Cast in the r_bsp module. */
    SYSTEM.SWRR = 0xa501;
    while (1) /* software reset */
    {
        R_BSP_NOP();
    }
}
/**********************************************************************************************************************
 End of function software_reset
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: secure_boot_rsa_key_generate
 * Description  : RSA key generate
 * Arguments    : none
 * Return Value : R_SECURE_BOOT_FAIL - 
 *                    Fail key generate
 *                R_SECURE_BOOT_SUCCESS - 
 *                    Success key generate
 *********************************************************************************************************************/
static int32_t secure_boot_rsa_key_generate(void)
{
    e_tsip_err_t tsip_error_code = TSIP_SUCCESS;

#if TSIP_RSASSA_1024 == 1 || TSIP_RSAES_1024 == 1
    printf("generate rsa1024 public key index: ");
    tsip_error_code = R_TSIP_GenerateRsa1024PublicKeyIndex(
    /* Casting uint8_t pointer is used for address. */
    (uint8_t*)g_key_block_image.key_data.encrypted_provisioning_key, (uint8_t*)g_key_block_image.key_data.iv, (uint8_t*)
    g_key_block_image.key_data.encrypted_user_rsa1024_ne_key, &g_key_block_image.key_data.user_rsa1024_ne_key_index);
    if (TSIP_SUCCESS == tsip_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_TSIP_GenerateRsa1024PublicKeyIndex() returns = %d\r\n", tsip_error_code);
        return R_SECURE_BOOT_FAIL;
    }
    printf("generate rsa1024 private key index: ");
    tsip_error_code = R_TSIP_GenerateRsa1024PrivateKeyIndex(
    /* Casting uint8_t pointer is used for address. */
    (uint8_t*)g_key_block_image.key_data.encrypted_provisioning_key, (uint8_t*)g_key_block_image.key_data.iv, (uint8_t*)
    g_key_block_image.key_data.encrypted_user_rsa1024_nd_key, &g_key_block_image.key_data.user_rsa1024_nd_key_index);
    if (TSIP_SUCCESS == tsip_error_code)
    {
        printf("OK\r\n");
    }
    else
    {
        printf("NG\r\n");
        printf("R_TSIP_GenerateRsa1024PrivateKeyIndex() returns = %d\r\n", tsip_error_code);
        return R_SECURE_BOOT_FAIL;
    }
#else
#error RSA 1024-bit keys must be provided for TSIP usage.
#endif  /* TSIP_RSASSA_1024 == 1 || TSIP_RSAES_1024 == 1 */
    return R_SECURE_BOOT_SUCCESS;
}
/**********************************************************************************************************************
 End of function secure_boot_rsa_key_generate
 *********************************************************************************************************************/



/**********************************************************************************************************************
 * Function Name: check_secure_boot_status_blank
 * Description  : check STATE_BLANK
 * Arguments    : none
 * Return Value : R_SECURE_BOOT_FAIL - 
 *                    Fail user program install
 *                R_SECURE_BOOT_SUCCESS - 
 *                    Success user program install
 *********************************************************************************************************************/
static int32_t check_secure_boot_status_blank(void)
{
    /* Casting uint8_t pointer is used for address. */
    if (compare_sha1_value((uint8_t*)&g_key_block_data.firmware_update_control_data,
            (sizeof(g_key_block_data.firmware_update_control_data)) + (sizeof(g_key_block_data.key_data)),
            /* Casting uint8_t pointer is used for address. */
            (uint8_t*)&g_key_block_data.hash_sha1))
    {
        printf("NG\r\n");
        printf("Key structure SHA1 HASH value is NOT match.\n\r");
        return R_SECURE_BOOT_FAIL;
    }

    return R_SECURE_BOOT_SUCCESS;
}
/**********************************************************************************************************************
 End of function check_secure_boot_status_blank
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: check_secure_boot_status_on_the_market
 * Description  : check STATE_ON_THE_MARKET
 * Arguments    : bank_info - 
 *                    Bank information
 * Return Value : R_SECURE_BOOT_FAIL - 
 *                    Fail user program install
 *                R_SECURE_BOOT_SUCCESS - 
 *                    Success user program install
 *********************************************************************************************************************/
static int32_t check_secure_boot_status_on_the_market(uint32_t bank_info)
{
    check_dataflash_area(0);
    return R_SECURE_BOOT_SUCCESS;
}
/**********************************************************************************************************************
 End of function check_secure_boot_status_on_the_market
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: calc_sha1_value
 * Description  : Calc SHA1 Value from message
 * Arguments    : pmsg_buff - 
 *                    massage buffer
 *                msg_legth - 
 *                    massage length
 *                sha1_val - 
 *                    calculate SHA1 value
 * Return Value : none
 *********************************************************************************************************************/
static void calc_sha1_value(uint8_t *pmsg_buff, uint32_t msg_legth, uint8_t *sha1_val)
{
    tsip_sha_md5_handle_t tsip_sha_md5_handle =
    {
        0
    };
    uint32_t digest_length = 0;

    R_TSIP_Sha1Init(&tsip_sha_md5_handle);
    R_TSIP_Sha1Update(&tsip_sha_md5_handle, pmsg_buff, msg_legth);
    R_TSIP_Sha1Final(&tsip_sha_md5_handle, sha1_val, &digest_length);

    return;
}
/**********************************************************************************************************************
 End of function calc_sha1_value
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: compare_sha1_value
 * Description  : calc message SHA1 value and compare source sha1 value
 * Arguments    : pmsg_buff - 
 *                    massage buffer
 *                msg_legth - 
 *                    massage length
 *                psrc_sha1 - 
 *                    compare soruce SHA1 value
 * Return Value : R_SECURE_BOOT_FAIL - 
 *                    Not match SHA1 value
 *                R_SECURE_BOOT_SUCCESS - 
 *                    match SHA1 value
 *********************************************************************************************************************/
static int32_t compare_sha1_value(uint8_t *pmsg_buff, uint32_t msg_legth, uint8_t *psrc_sha1)
{
    uint8_t hash_sha1[R_TSIP_SHA1_HASH_LENGTH_BYTE_SIZE] =
    {
        0
    };

    calc_sha1_value(pmsg_buff, msg_legth, hash_sha1);
    if (memcmp(psrc_sha1, hash_sha1, R_TSIP_SHA1_HASH_LENGTH_BYTE_SIZE))
    {
        return R_SECURE_BOOT_FAIL;
    }
    return R_SECURE_BOOT_SUCCESS;
}
/**********************************************************************************************************************
 End of function compare_sha1_value
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: tsip_keyring_restore
 * Description  : Store key generation information
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
void tsip_keyring_restore(void)
{
    memcpy(&g_key_block_image, &g_key_block_data, sizeof(st_key_block_data_t));
    return;
}
/**********************************************************************************************************************
 End of function tsip_keyring_restore
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: tsip_install_keyring
 * Description  : Write key generation information to date flash
 * Arguments    : none
 * Return Value : none
 *********************************************************************************************************************/
static void tsip_install_keyring(void)
{
    /* Casting uint8_t pointer is used for address. */
    calc_sha1_value((uint8_t*)&g_key_block_image.firmware_update_control_data,
        (sizeof(g_key_block_image.firmware_update_control_data)) + (sizeof(g_key_block_image.key_data)),
        g_key_block_image.hash_sha1);

    update_dataflash_data_from_image(DF_BANK_MAIN);
    update_dataflash_data_from_image(DF_BANK_MIRROR);
    return;
}
/**********************************************************************************************************************
 End of function tsip_install_keyring
 *********************************************************************************************************************/

void tsip_print_installed_key_index(void)
{
    char tmp[256] =
    {
        0
    };
    uint32_t i = 0;

    sprintf(g_print_buffer, "-----------------print_installed_key_value-----------------\r\n");
    strcat(g_print_buffer, "the followings are installed key indexes.\r\n");
    sprintf(tmp, "user_rsa1024_nd_key_index:\t\t");
    strcat(g_print_buffer, tmp);
    for (i = 0;
        i < ((sizeof(g_key_block_image.key_data.user_rsa1024_nd_key_index.value.key_management_info1)) / 4); i++)
    {
        sprintf(tmp, "%08x", g_key_block_image.key_data.user_rsa1024_nd_key_index.value.key_management_info1[i]);
        strcat(g_print_buffer, tmp);
    }
    for (i = 0; i < (sizeof(g_key_block_image.key_data.user_rsa1024_nd_key_index.value.key_n)); i++)
    {
        sprintf(tmp, "%02x", g_key_block_image.key_data.user_rsa1024_nd_key_index.value.key_n[i]);
        strcat(g_print_buffer, tmp);
    }
    for (i = 0;
        i < ((sizeof(g_key_block_image.key_data.user_rsa1024_nd_key_index.value.key_management_info2)) / 4); i++)
    {
        sprintf(tmp, "%08x", g_key_block_image.key_data.user_rsa1024_nd_key_index.value.key_management_info2[i]);
        strcat(g_print_buffer, tmp);
    }
    strcat(g_print_buffer, "\r\n");

    sprintf(tmp, "user_rsa1024_ne_key_index:\t\t");
    strcat(g_print_buffer, tmp);
    for (i = 0;
        i < ((sizeof(g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_management_info1)) / 4); i++)
    {
        sprintf(tmp, "%08x", g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_management_info1[i]);
        strcat(g_print_buffer, tmp);
    }
    for (i = 0; i < (sizeof(g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_n)); i++)
    {
        sprintf(tmp, "%02x", g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_n[i]);
        strcat(g_print_buffer, tmp);
    }
    for (i = 0; i < (sizeof(g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_e)); i++)
    {
        sprintf(tmp, "%02x", g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_e[i]);
        strcat(g_print_buffer, tmp);
    }
    for (i = 0; i < (sizeof(g_key_block_image.key_data.user_rsa1024_ne_key_index.value.dummy)); i++)
    {
        sprintf(tmp, "%02x", g_key_block_image.key_data.user_rsa1024_ne_key_index.value.dummy[i]);
        strcat(g_print_buffer, tmp);
    }
    for (i = 0;
        i < ((sizeof(g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_management_info2)) / 4); i++)
    {
        sprintf(tmp, "%08x", g_key_block_image.key_data.user_rsa1024_ne_key_index.value.key_management_info2[i]);
        strcat(g_print_buffer, tmp);
    }
    strcat(g_print_buffer, "\r\n");

    printf("%s", g_print_buffer);
    g_print_buffer[0] = 0;

    return;
}
