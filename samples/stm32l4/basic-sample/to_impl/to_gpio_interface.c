
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

#include "stm32l4s5i_iot01.h"

#define GPIO_MCU_TO136_POWER_PIN            GPIO_PIN_3
#define GPIO_MCU_TO136_POWER_PORT           GPIOA
void TO_Power_GPIO_Init()
{
  GPIO_InitTypeDef  gpio_init_structure;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  gpio_init_structure.Pin   = GPIO_MCU_TO136_POWER_PIN;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  // TODO: LOW ? Why VERY_HIGH?
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = 0; // Nik: TODO understand this

  HAL_GPIO_Init(GPIO_MCU_TO136_POWER_PORT, &gpio_init_structure);
}

void TO_Power_GPIO_Deinit()
{
  HAL_GPIO_WritePin(GPIO_MCU_TO136_POWER_PORT, GPIO_MCU_TO136_POWER_PIN, GPIO_PIN_SET);
  HAL_GPIO_DeInit(GPIO_MCU_TO136_POWER_PORT, GPIO_MCU_TO136_POWER_PIN);
}

void TO_Power_GPIO_On()
{
  HAL_GPIO_WritePin(GPIO_MCU_TO136_POWER_PORT, GPIO_MCU_TO136_POWER_PIN, GPIO_PIN_RESET);
}

void TO_Power_GPIO_Off()
{
  HAL_GPIO_WritePin(GPIO_MCU_TO136_POWER_PORT, GPIO_MCU_TO136_POWER_PIN, GPIO_PIN_SET);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

