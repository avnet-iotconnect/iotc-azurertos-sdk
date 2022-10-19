/**
  ******************************************************************************
  * @file    stm32l4s5i_iot01.h
  * @author  MCD Application Team
  * @brief   STM32L4S5I IOT01 board support package
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l4s5i_iot01.h"

/** @defgroup BSP BSP
  * @{
  */

/** @defgroup STM32L4S5I_IOT01 STM32L4S5I_IOT01
  * @{
  */

/** @defgroup STM32L4S5I_IOT01_LOW_LEVEL LOW LEVEL
  * @{
  */

/** @defgroup STM32L4S5I_IOT01_LOW_LEVEL_Private_Defines LOW LEVEL Private Def
  * @{
  */
/**
 * @brief STM32L4S5I IOT01 BSP Driver version number
   */
#define __STM32L4S5I_IOT01_BSP_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __STM32L4S5I_IOT01_BSP_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __STM32L4S5I_IOT01_BSP_VERSION_SUB2   (0x00) /*!< [15:8]  sub2 version */
#define __STM32L4S5I_IOT01_BSP_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __STM32L4S5I_IOT01_BSP_VERSION        ((__STM32L4S5I_IOT01_BSP_VERSION_MAIN << 24)\
                                                 |(__STM32L4S5I_IOT01_BSP_VERSION_SUB1 << 16)\
                                                 |(__STM32L4S5I_IOT01_BSP_VERSION_SUB2 << 8 )\
                                                 |(__STM32L4S5I_IOT01_BSP_VERSION_RC))
/**
  * @}
  */

/** @defgroup STM32L4S5I_IOT01_LOW_LEVEL_Private_Variables LOW LEVEL Variables
  * @{
  */
#define DISCOVERY_I2C1_SCL_PIN                     GPIO_PIN_10
#define DISCOVERY_I2C1_SDA_PIN                     GPIO_PIN_11


static I2C_HandleTypeDef i2cHandle;

/**
  * @}
  */
/** @defgroup STM32L4S5I_IOT01_LOW_LEVEL_Private_FunctionPrototypes LOW LEVEL Private Function Prototypes
  * @{
  */
static void     I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler);
static void     I2Cx_MspDeInit(I2C_HandleTypeDef *i2c_handler);
static void     I2Cx_Init(I2C_HandleTypeDef *i2c_handler);
static void     I2Cx_DeInit(I2C_HandleTypeDef *i2c_handler);

/*******************************************************************************
                            BUS OPERATIONS
*******************************************************************************/

/******************************* I2C Routines *********************************/
/**
  * @brief  Initializes I2C MSP.
  * @param  i2c_handler  I2C handler
  * @retval None
  */
static void I2Cx_MspInit(I2C_HandleTypeDef *i2c_handler)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /*** Configure the GPIOs ***/
  /* Enable GPIO clock */
  DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_ENABLE();

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
  gpio_init_structure.Mode = GPIO_MODE_AF_OD;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = DISCOVERY_I2Cx_SCL_SDA_AF;
  HAL_GPIO_Init(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

  HAL_GPIO_Init(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, &gpio_init_structure);

  /*** Configure the I2C peripheral ***/
  /* Enable I2C clock */
  DISCOVERY_I2Cx_CLK_ENABLE();

  /* Force the I2C peripheral clock reset */
  DISCOVERY_I2Cx_FORCE_RESET();

  /* Release the I2C peripheral clock reset */
  DISCOVERY_I2Cx_RELEASE_RESET();

  /* Enable and set I2Cx Interrupt to a lower priority */
  HAL_NVIC_SetPriority(DISCOVERY_I2Cx_EV_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_EV_IRQn);

  /* Enable and set I2Cx Interrupt to a lower priority */
  HAL_NVIC_SetPriority(DISCOVERY_I2Cx_ER_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DISCOVERY_I2Cx_ER_IRQn);
}

/**
  * @brief  DeInitializes I2C MSP.
  * @param  i2c_handler  I2C handler
  * @retval None
  */
static void I2Cx_MspDeInit(I2C_HandleTypeDef *i2c_handler)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* Configure I2C Tx, Rx as alternate function */
  gpio_init_structure.Pin = DISCOVERY_I2Cx_SCL_PIN | DISCOVERY_I2Cx_SDA_PIN;
  HAL_GPIO_DeInit(DISCOVERY_I2Cx_SCL_SDA_GPIO_PORT, gpio_init_structure.Pin);
  /* Disable GPIO clock */
  DISCOVERY_I2Cx_SCL_SDA_GPIO_CLK_DISABLE();

  /* Disable I2C clock */
  DISCOVERY_I2Cx_CLK_DISABLE();
}

/**
  * @brief  Initializes I2C HAL.
  * @param  i2c_handler  I2C handler
  * @retval None
  */
static void I2Cx_Init(I2C_HandleTypeDef *i2c_handler)
{
  /* I2C configuration */
  i2c_handler->Instance              = I2C1;
  i2c_handler->Init.Timing           = DISCOVERY_I2Cx_TIMING; //0x10D1143A;// DISCOVERY_I2Cx_TIMING// DISCOVERY_I2Cx_TIMING;
  i2c_handler->Init.OwnAddress1      = 0;
  i2c_handler->Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  i2c_handler->Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  i2c_handler->Init.OwnAddress2      = 0;
  i2c_handler->Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  i2c_handler->Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

  /* Init the I2C */
  I2Cx_MspInit(i2c_handler);
  HAL_I2C_Init(i2c_handler);

  /**Configure Analogue filter */
  HAL_I2CEx_ConfigAnalogFilter(i2c_handler, I2C_ANALOGFILTER_ENABLE);
}

/**
  * @brief  DeInitializes I2C HAL.
  * @param  i2c_handler  I2C handler
  * @retval None
  */
static void I2Cx_DeInit(I2C_HandleTypeDef *i2c_handler)
{  /* DeInit the I2C */
  I2Cx_MspDeInit(i2c_handler);
  HAL_I2C_DeInit(i2c_handler);
}

void I2Cx_SE_Init(I2C_HandleTypeDef *i2c_handler)
{
	I2Cx_Init(&i2cHandle);
}

void I2Cx_SE_DeInit(I2C_HandleTypeDef *i2c_handler)
{
	I2Cx_DeInit(&i2cHandle);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
