/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : thread_x_entry.c
 * Version      : 1.0
 * Description  : declare Thread entry function  
 **********************************************************************************************************************/
/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
//#include "azurertos_object_init.h"
#include "tx_api.h"
#include "r_hs300x_if.h"
#include "r_comms_i2c_if.h"
#include "hs300x_sensor_thread_entry.h"

#if COMMS_I2C_CFG_DRIVER_I2C
#include "r_riic_rx_if.h"
#endif
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
#include "r_sci_iic_rx_if.h"
#endif

#include <stdio.h>

extern TX_THREAD               hs300x_sensor_thread;

/* Sequence */
typedef enum e_demo_sequence
{
    DEMO_SEQUENCE_1 = (1),
    DEMO_SEQUENCE_2,
    DEMO_SEQUENCE_3,
    DEMO_SEQUENCE_4,
    DEMO_SEQUENCE_5,
    DEMO_SEQUENCE_6,
} demo_sequence_t;

/* Callback status */
typedef enum e_demo_callback_status
{
    DEMO_CALLBACK_STATUS_WAIT = (0),
    DEMO_CALLBACK_STATUS_SUCCESS,
    DEMO_CALLBACK_STATUS_REPEAT,
} demo_callback_status_t;

/* See Developer Assistance in the project */
static int g_comms_i2c_bus0_quick_setup(void);
static int g_hs300x_sensor0_quick_setup(void);

static volatile demo_callback_status_t  gs_demo_callback_status;
static volatile rm_hs300x_data_t        gs_hs300x_data;
extern 			TX_THREAD               hs300x_sensor_thread;

static double temperature_measurement;
static double humidity_measurement;

/* Quick setup for g_comms_i2c_bus0. */
static int g_comms_i2c_bus0_quick_setup(void)
{
	i2c_master_instance_t * p_driver_instance = (i2c_master_instance_t *) g_comms_i2c_bus0_extended_cfg.p_driver_instance;

    /* Open i2c driver */
    if(COMMS_DRIVER_I2C == p_driver_instance->driver_type)
    {
#if COMMS_I2C_CFG_DRIVER_I2C
        riic_return_t i2c_ret;
        riic_info_t * p_i2c_info = (riic_info_t *)p_driver_instance->p_info;

        p_i2c_info->ch_no = (uint8_t) p_driver_instance->driver_channel;
        i2c_ret = R_RIIC_Open(p_i2c_info);
        switch (i2c_ret)
        {
            case RIIC_ERR_LOCK_FUNC:
            case RIIC_ERR_INVALID_CHAN:
            case RIIC_ERR_INVALID_ARG:
            case RIIC_ERR_OTHER:
            	tx_thread_delete(&hs300x_sensor_thread);
            	return -1;
            default:
                break;
        }
#else
        printf("COMMS_I2C_CFG_DRIVER_I2C not handled\r\n");
#endif
    }
    else if(COMMS_DRIVER_SCI_I2C == p_driver_instance->driver_type)
    {
#if COMMS_I2C_CFG_DRIVER_SCI_I2C
        sci_iic_return_t i2c_ret;
        sci_iic_info_t * p_i2c_info = (sci_iic_info_t *) p_driver_instance->p_info;

        p_i2c_info->ch_no = (uint8_t) p_driver_instance->driver_channel;
        i2c_ret = R_SCI_IIC_Open(p_i2c_info);
        switch (i2c_ret)
        {
            case SCI_IIC_ERR_LOCK_FUNC:
            case SCI_IIC_ERR_INVALID_CHAN:
            case SCI_IIC_ERR_INVALID_ARG:
            case SCI_IIC_ERR_OTHER:
            	tx_thread_delete(&hs300x_sensor_thread);
            	return -1;
            default:
                break;
        }
#else
        printf("COMMS_I2C_CFG_DRIVER_SCI_I2C not handled\r\n");
#endif
    } else {
    	printf("Unknown I2C driver type\r\n");
    }

    /* Create a semaphore for blocking if a semaphore is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore)
    {
        tx_semaphore_create(g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_handle,
                            g_comms_i2c_bus0_extended_cfg.p_blocking_semaphore->p_semaphore_name,
                            (ULONG) 0);
    	return -1;
    }

    /* Create a recursive mutex for bus lock if a recursive mutex is not NULL */
    if (NULL != g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex)
    {
        tx_mutex_create(g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_handle,
                        g_comms_i2c_bus0_extended_cfg.p_bus_recursive_mutex->p_mutex_name,
                        TX_INHERIT);
    	return -1;
    }
    return 0;
}

void hs300x_user_callback0 (rm_hs300x_callback_args_t * p_args)
{
    if (RM_HS300X_EVENT_SUCCESS == p_args->event)
    {
        gs_demo_callback_status = DEMO_CALLBACK_STATUS_SUCCESS;
    }
    else
    {
        gs_demo_callback_status = DEMO_CALLBACK_STATUS_REPEAT;
    }
}

/* Quick setup for g_hs300x_sensor0. */
static int g_hs300x_sensor0_quick_setup(void)
{
    fsp_err_t err;

    /* Open HS300X sensor instance, this must be done before calling any HS300X API */
    err = RM_HS300X_Open(g_hs300x_sensor0.p_ctrl, g_hs300x_sensor0.p_cfg);
    if (FSP_SUCCESS != err)
    {
    	tx_thread_delete(&hs300x_sensor_thread);
    	return -1;
    }

    return 0;
}

/* New Thread entry function */
void hs300x_sensor_thread_entry(ULONG entry_input)
{
    /* TODO: add your own code here */
    fsp_err_t err;
    rm_hs300x_raw_data_t raw_data;
    demo_sequence_t sequence = DEMO_SEQUENCE_1;

    /* Open the Bus */
    if(g_comms_i2c_bus0_quick_setup() != 0)
    {
    	printf("g_comms_i2c_bus0_quick_setup failed\r\n");
    	return;
    }

    /* Open HS300X */
    if(g_hs300x_sensor0_quick_setup() != 0)
    {
    	printf("g_hs300x_sensor0_quick_setup failed\r\n");
    	return;
    }

    while (1)
    {
        /* Switch to another thread? */
        tx_thread_sleep(1);

        switch(sequence)
        {
            case DEMO_SEQUENCE_1 :
            {
                /* Clear status */
                gs_demo_callback_status = DEMO_CALLBACK_STATUS_WAIT;

                /* Start the measurement */
                err = RM_HS300X_MeasurementStart(g_hs300x_sensor0.p_ctrl);
                if (FSP_SUCCESS == err)
                {
                    sequence = DEMO_SEQUENCE_2;
                }
                else
                {
                    tx_thread_delete(&hs300x_sensor_thread);
                	printf("RM_HS300X_MeasurementStart failed\r\n");
                }
            }
            break;

            case DEMO_SEQUENCE_2 :
            {
                switch(gs_demo_callback_status)
                {
                    case DEMO_CALLBACK_STATUS_WAIT :
                        break;
                    case DEMO_CALLBACK_STATUS_SUCCESS :
                        sequence = DEMO_SEQUENCE_3;
                        break;
                    case DEMO_CALLBACK_STATUS_REPEAT :
                        sequence = DEMO_SEQUENCE_1;
                        break;
                    default :
                        tx_thread_delete(&hs300x_sensor_thread);
                        break;
                }
            }
            break;

            case DEMO_SEQUENCE_3 :
            {
                /* Switch to another thread */
                tx_thread_sleep(1);

                /* Clear status */
                gs_demo_callback_status = DEMO_CALLBACK_STATUS_WAIT;

                /* Read data */
                err = RM_HS300X_Read(g_hs300x_sensor0.p_ctrl, &raw_data);
                if (FSP_SUCCESS == err)
                {
                    sequence = DEMO_SEQUENCE_4;
                }
                else
                {
                    tx_thread_delete(&hs300x_sensor_thread);
                }
            }
            break;

            case DEMO_SEQUENCE_4 :
            {
                switch(gs_demo_callback_status)
                {
                    case DEMO_CALLBACK_STATUS_WAIT :
                        break;
                    case DEMO_CALLBACK_STATUS_SUCCESS :
                        sequence = DEMO_SEQUENCE_5;
                        break;
                    case DEMO_CALLBACK_STATUS_REPEAT :
                        sequence = DEMO_SEQUENCE_3;
                        break;
                    default :
                        tx_thread_delete(&hs300x_sensor_thread);
                        break;
                }
            }
            break;

            case DEMO_SEQUENCE_5 :
            {
                /* Switch to another thread */
                tx_thread_sleep(1);

                /* Calculate data */
                err = RM_HS300X_DataCalculate(g_hs300x_sensor0.p_ctrl,
                							  &raw_data,
											  (rm_hs300x_data_t *)&gs_hs300x_data);
                if (FSP_SUCCESS == err)
                {
                    sequence = DEMO_SEQUENCE_6;
                    /* Sensor data is valid. Describe the process by referring to the calculated sensor data. */
                    temperature_measurement = (double) (gs_hs300x_data.temperature.integer_part) + ((double) gs_hs300x_data.temperature.decimal_part)/100.0;
                    humidity_measurement = (double) (gs_hs300x_data.humidity.integer_part) + ((double) gs_hs300x_data.humidity.decimal_part)/100.0;
                }
                else if (FSP_ERR_SENSOR_INVALID_DATA == err)
                {
                    sequence = DEMO_SEQUENCE_3;
                    /* Sensor data is invalid. */
                }
                else
                {
                    tx_thread_delete(&hs300x_sensor_thread);
                }
            }
            break;

            case DEMO_SEQUENCE_6 :
            {
                /* Wait 4 seconds. See table 4 on the page 6 of the datasheet. */
                tx_thread_sleep(400);
                sequence = DEMO_SEQUENCE_1;
            }
            break;

            default :
                tx_thread_delete(&hs300x_sensor_thread);
                break;
        }
    }
}

double get_temperature(void)
{
	return temperature_measurement;
}

double get_humidity(void)
{
	return humidity_measurement;
}
