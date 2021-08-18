/**
  ******************************************************************************
  * @file    sensors_data.c
  * @author  MCD Application Team
  * @brief   Prepare data of B-L475E-IOT01 sensors.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

//
// Copyright: Avnet 2021
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 8/17/21.
//


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sensors_data.h"

#include "stm32l4xx_hal.h"
#include "stm32l4s5i_iot01.h"
#include "stm32l475e_iot01_env_sensors.h"
#include "stm32l475e_iot01_motion_sensors.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define INSTANCE_TEMPERATURE_HUMIDITY 0
#define INSTANCE_TEMPERATURE_PRESSURE 1
#define INSTANCE_GYROSCOPE_ACCELEROMETER 0
#define INSTANCE_MAGNETOMETER 1
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  init_sensors
  * @param  none
  * @retval 0  in case of success
  *         <0 BSP error code in case of failure
  */
int init_sensors(void)
{
  int32_t ret = 0;

  /* For sensors combos, must do the two initializations first, then the enables */
  ret = BSP_ENV_SENSOR_Init(INSTANCE_TEMPERATURE_HUMIDITY, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_ENV_SENSOR_Init(ENV_HUMIDITY) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Init(INSTANCE_TEMPERATURE_HUMIDITY, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_ENV_SENSOR_Init(ENV_TEMPERATURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Enable(INSTANCE_TEMPERATURE_HUMIDITY, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_ENV_SENSOR_Enable(ENV_HUMIDITY) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Enable(INSTANCE_TEMPERATURE_HUMIDITY, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_ENV_SENSOR_Enable(ENV_TEMPERATURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Init(INSTANCE_TEMPERATURE_PRESSURE, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_ENV_SENSOR_Init(ENV_PRESSURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_ENV_SENSOR_Enable(INSTANCE_TEMPERATURE_PRESSURE, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_ENV_SENSOR_Enable(ENV_PRESSURE) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Init(INSTANCE_MAGNETOMETER, MOTION_MAGNETO);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_MOTION_SENSOR_Init(MOTION_MAGNETO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Enable(INSTANCE_MAGNETOMETER, MOTION_MAGNETO);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_MOTION_SENSOR_Enable(MOTION_MAGNETO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Init(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_GYRO);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_MOTION_SENSOR_Init(MOTION_GYRO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Init(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_ACCELERO);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_MOTION_SENSOR_Init(MOTION_ACCELERO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Enable(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_ACCELERO);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_MOTION_SENSOR_Enable(MOTION_ACCELERO) returns %ld\n", ret);
    goto error;
  }

  ret = BSP_MOTION_SENSOR_Enable(INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_GYRO);
  if (ret != BSP_ERROR_NONE)
  {
    printf("BSP_MOTION_SENSOR_Enable(MOTION_GYRO) returns %ld\n", ret);
    goto error;
  }

  error:

  return ret;
}
static void add_env_sensor_value(IotclMessageHandle msg, const char* name, uint32_t instance, uint32_t function) {
    float_t value = 0;
    uint32_t ret = BSP_ENV_SENSOR_GetValue(instance, function, &value);
    if (ret != BSP_ERROR_NONE) {
        printf("Failed to get %s. Error was %lu", name, ret);
    } else {
        iotcl_telemetry_set_number(msg, name, value);
    }
}
static void add_motion_sensor_value(IotclMessageHandle msg, const char* name, uint32_t instance, uint32_t function) {
    BSP_MOTION_SENSOR_Axes_t value = {0};
    uint32_t ret = BSP_MOTION_SENSOR_GetAxes(instance, function, &value);
    if (ret != BSP_ERROR_NONE) {
        printf("Failed to get %s. Error was %lu", name, ret);
    } else {
        char full_name[30];
        // nest x y and z into a an object
        sprintf(full_name, "%s.%s", name, "x");
        iotcl_telemetry_set_number(msg, full_name, value.x);
        sprintf(full_name, "%s.%s", name, "y");
        iotcl_telemetry_set_number(msg, full_name, value.y);
        sprintf(full_name, "%s.%s", name, "z");
        iotcl_telemetry_set_number(msg, full_name, value.z);
    }
}
void sensors_add_telemetry(IotclMessageHandle msg) {

    add_env_sensor_value(msg, "temperature", INSTANCE_TEMPERATURE_HUMIDITY, ENV_TEMPERATURE);
    add_env_sensor_value(msg, "humidity", INSTANCE_TEMPERATURE_HUMIDITY, ENV_HUMIDITY);
    add_env_sensor_value(msg, "pressure", INSTANCE_TEMPERATURE_PRESSURE, ENV_PRESSURE);
    add_motion_sensor_value(msg, "gyroscope", INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_GYRO);
    add_motion_sensor_value(msg, "magnetometer", INSTANCE_MAGNETOMETER, MOTION_MAGNETO);
    add_motion_sensor_value(msg, "accelerometer", INSTANCE_GYROSCOPE_ACCELEROMETER, MOTION_ACCELERO);
}
