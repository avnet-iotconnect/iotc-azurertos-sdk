/**
  ******************************************************************************
  * @file    sensors_data.h
  * @author  MCD Application Team
  * @brief   Header for sensors data cloud publishing
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

#ifndef SENSORS_DATA_H
#define SENSORS_DATA_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "iotconnect.h"

int init_sensors(void);
void sensors_add_telemetry(IotclMessageHandle msg);

#ifdef __cplusplus
}
#endif
#endif /* SENSORS_DATA_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
