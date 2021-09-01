/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/**************************************************************************/

#ifdef USE_AZRTOS_ADU_AGENT_DRIVER
#include "stm32l4xx_hal.h"
#include "nx_azure_iot_adu_agent.h"

/* Define the bank2 address for new firmware.  */
#define FLASH_BANK2_ADDR                (FLASH_BASE + FLASH_BANK_SIZE)

/* Define the internal variables.  */
static uint32_t total_write_size;
static uint32_t write_size;
static union
{
    uint64_t write_buffer_int64;
    unsigned char write_buffer_char[8];
}write_buffer;

static int remainder_count;

/* ADU driver entry.  */
void nx_azure_iot_adu_agent_stm32l4xx_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr);

/* Internal functions.  */
static int boot_bank_set(uint32_t bank);
static int boot_bank_get(void);
static int internal_flash_erase(unsigned int bank, unsigned int size);
static int internal_flash_write(unsigned char* destination_ptr, unsigned char* source_ptr, unsigned int size);
static int internal_firmware_install(void);
static void internal_firmware_apply(void);


/****** DRIVER SPECIFIC ******/
void  nx_azure_iot_adu_agent_stm32l4xx_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr)
{

UINT status;


    /* Default to successful return.  */
    driver_req_ptr -> nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_SUCCESS;

    /* Process according to the driver request type.  */
    switch (driver_req_ptr -> nx_azure_iot_adu_agent_driver_command)
    {

        case NX_AZURE_IOT_ADU_AGENT_DRIVER_INITIALIZE:
        {

            /* Process initialize requests.  */
            break;
        }

        case NX_AZURE_IOT_ADU_AGENT_DRIVER_PREPROCESS:
        {

            /* Process firmware preprocess requests before writing firmware.
               Such as: erase the flash at once to improve the speed.  */

            /* Erase the flash for new firmware.  */
            if (boot_bank_get() == FLASH_BANK_1)
            {
                status = internal_flash_erase(FLASH_BANK_2, driver_req_ptr -> nx_azure_iot_adu_agent_driver_firmware_size);
            }
            else
            {
                status = internal_flash_erase(FLASH_BANK_1, driver_req_ptr -> nx_azure_iot_adu_agent_driver_firmware_size);
            }

            /* Check status.  */
            if (status)
            {
                driver_req_ptr -> nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
            }

            break;
        }

        case NX_AZURE_IOT_ADU_AGENT_DRIVER_WRITE:
        {

            /* Process firmware write requests.  */

            /* Write firmware contents.
               1. This function must support figure out which bank it should write to.
               2. Write firmware contents into new bank.
               3. Decrypt and authenticate the firmware itself if needed.
            */

            /* Write firmware contents into flash.  */
            UCHAR   *dest_ptr = (UCHAR*)(FLASH_BANK2_ADDR + driver_req_ptr -> nx_azure_iot_adu_agent_driver_firmware_data_offset);
            status = internal_flash_write(dest_ptr,
                                          driver_req_ptr -> nx_azure_iot_adu_agent_driver_firmware_data_ptr,
                                          driver_req_ptr -> nx_azure_iot_adu_agent_driver_firmware_data_size);

            /* Check status.  */
            if (status)
            {
                driver_req_ptr -> nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
            }

            break;
        }

        case NX_AZURE_IOT_ADU_AGENT_DRIVER_INSTALL:
        {

            /* Set the new firmware for next boot.  */
            status = internal_firmware_install();

            /* Check status.  */
            if (status)
            {
                driver_req_ptr -> nx_azure_iot_adu_agent_driver_status = NX_AZURE_IOT_FAILURE;
            }

            break;
        }

        case NX_AZURE_IOT_ADU_AGENT_DRIVER_APPLY:
        {

            /* Apply the new firmware, and reboot device from that.*/
            internal_firmware_apply();

            break;
        }
        default:
        {

            /* Invalid driver request.  */

            /* Default to successful return.  */
            driver_req_ptr -> nx_azure_iot_adu_agent_driver_status =  NX_AZURE_IOT_FAILURE;
        }
    }
}

/* Set bank for boot.  */
static int boot_bank_set(uint32_t bank)
{
    unsigned int rc = 0;
    FLASH_OBProgramInitTypeDef OBInit;
    /* Set BFB2 bit to enable boot from Flash Bank2 */
    /* Allow Access to the Flash control registers and user Flash. */
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples. */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
    /* Allow Access to the option bytes sector. */
    HAL_FLASH_OB_Unlock();
    /* Get the Dual boot configuration status. */
    HAL_FLASHEx_OBGetConfig(&OBInit);

    /* Enable/Disable dual boot feature */
    OBInit.OptionType = OPTIONBYTE_USER;
    OBInit.USERType = OB_USER_BFB2;
    switch (bank)
    {
    case FLASH_BANK_1:
        OBInit.USERConfig = OB_BFB2_DISABLE;
        break;
    case FLASH_BANK_2:
        OBInit.USERConfig = OB_BFB2_ENABLE;
        break;
    case FLASH_BANK_BOTH:
        OBInit.USERConfig =
                ((OBInit.USERConfig & OB_BFB2_ENABLE) == OB_BFB2_ENABLE) ?
                        OB_BFB2_DISABLE : OB_BFB2_ENABLE;
        break;
    default:
        rc = -1;
    }

    if (HAL_FLASHEx_OBProgram(&OBInit) != HAL_OK)
    { /* Failed setting the option bytes configuration.
     * Call 'HAL_FLASH_GetError()' for details. */
        rc = -1;
    }

    HAL_FLASH_Lock();

    return rc;
}

/* Get current boot bank.  */
static int boot_bank_get(void)
{
    FLASH_OBProgramInitTypeDef OBInit;

    /* Clear OPTVERR bit set on virgin samples. */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    /* Get the Dual boot configuration status. */
    HAL_FLASHEx_OBGetConfig(&OBInit);

    return ((OBInit.USERConfig & OB_BFB2_ENABLE) == OB_BFB2_ENABLE) ?
            FLASH_BANK_2 : FLASH_BANK_1;
}

/* Erase internal flash sectors by bank and size.  */
static int internal_flash_erase(unsigned int bank, unsigned int size)
{

FLASH_EraseInitTypeDef  erase_init;
uint32_t                page_error;
HAL_StatusTypeDef       status;


    if (size > FLASH_BANK_SIZE)
        return -1;

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Banks = bank;
    erase_init.Page = 0;
    erase_init.NbPages = (size + FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE;
    HAL_FLASH_Unlock();
    // This error happens if we erased or partially wrote into the bank's blocks, but it did not complete
    // It basically means that the boot bank targeted for booting failed.
    if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_PEMPTY) != 0)
    {
      __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PEMPTY);
    }
    status = HAL_FLASHEx_Erase(&erase_init, &page_error);
    HAL_FLASH_Lock();
    if ((status != HAL_OK) || page_error != 0xFFFFFFFF)
    {
        return -1;
    }
    else
    {
        total_write_size = size;
        remainder_count = -1;
        write_size = 0;
        return 0;
    }
}

/* Write firmware into internal flash.  */
static int internal_flash_write(unsigned char* destination_ptr, unsigned char* source_ptr, unsigned int size)
{

HAL_StatusTypeDef       status;


    write_size += size;

    for (; (remainder_count >= 0) && (remainder_count < 8) && (size > 0); destination_ptr++, size--)
    {
        write_buffer.write_buffer_char[remainder_count++] = *source_ptr++;
    }

    HAL_FLASH_Unlock();

    if (remainder_count == 8)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(destination_ptr - 8), write_buffer.write_buffer_int64);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
        remainder_count = -1;
    }
    for (; size > 8; size -= 8, destination_ptr += 8, source_ptr += 8)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(destination_ptr), (uint64_t)*(uint32_t*)source_ptr | ((uint64_t)*(uint32_t*)(source_ptr + 4)) << 32);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
    }
    if (size > 0)
    {
        remainder_count = 0;
    }
    for (; size > 0; size --)
    {
        write_buffer.write_buffer_char[remainder_count++] = *source_ptr++;
        destination_ptr++;
    }
    if ((remainder_count >= 0) && (write_size >= total_write_size))
    {
        while (remainder_count < 8)
        {
            write_buffer.write_buffer_char[remainder_count++] = 0xFF;
            destination_ptr++;
        }

        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(destination_ptr - 8), write_buffer.write_buffer_int64);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
    }
    HAL_FLASH_Lock();
    return 0;
}

/* Install new firmware.  */
static int internal_firmware_install(void)
{

UINT    status;


    /* Set the new bank for boot.  */
    if (boot_bank_get() == FLASH_BANK_1)
    {
        status = boot_bank_set(FLASH_BANK_2);
    }
    else
    {
        status = boot_bank_set(FLASH_BANK_1);
    }

    /* Check status.  */
    if (status)
    {
        return(NX_AZURE_IOT_FAILURE);
    }

    /* Return success. */
    return(NX_AZURE_IOT_SUCCESS);
}


/* Apply new firmware, and reboot device from new firmware.  */
static void internal_firmware_apply(void)
{

    /* Allow Access to the Flash control registers and user Flash. */
    HAL_FLASH_Unlock();

    /* Allow Access to the option bytes sector. */
    HAL_FLASH_OB_Unlock();

    /* Setting OBL_LAUNCH generates a reset.
       so the option byte loading is performed under system reset.  */
    HAL_FLASH_OB_Launch();
}

#endif //USE_AZRTOS_ADU_AGENT_DRIVER
