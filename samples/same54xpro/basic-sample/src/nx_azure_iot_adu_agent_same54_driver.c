/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/**************************************************************************/

#include "tx_api.h"
#include "nx_api.h"
#include "driver_init_flash.h"
#include "driver_init.h"

#include "utils.h"
#include "hal_flash.h"
#include "nx_azure_iot_adu_agent.h"


/* Define the bank2 address for new firmware.  */
#define FLASH_BANK2_ADDR                0x80000


/* adu_agent driver entry.  */
void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr);


/****** DRIVER SPECIFIC ******/
void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr)
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
             uint32_t   dest_addr = (FLASH_BANK2_ADDR + driver_req_ptr -> nx_azure_iot_adu_agent_driver_firmware_data_offset);

            /* Write data to flash */
            status =  flash_write(&FLASH_0, dest_addr,
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
            
            break;
        } 
            
        case NX_AZURE_IOT_ADU_AGENT_DRIVER_APPLY:
        {
            
            /* Apply the new firmware, and reboot device from that.*/   
            
            flash_swap(&FLASH_0);
        
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