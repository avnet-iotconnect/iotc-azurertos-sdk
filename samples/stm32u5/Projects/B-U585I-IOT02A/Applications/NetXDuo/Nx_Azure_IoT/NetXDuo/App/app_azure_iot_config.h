/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/****************************************************************************************/
/*                                                                                      */
/* The following source code has been taken from 'sample_config.h'                      */
/* by Microsoft (https://github.com/azure-rtos) and modified by STMicroelectronics.     */
/* Main changes summary :                                                               */
/* - Adaptation to STMicroelectronics B-U585I-IoT02A Discovery board PnP Model          */
/* - Add WiFi SSID and password configuration                                           */
/*                                                                                      */
/****************************************************************************************/

#ifndef APP_AZURE_IOT_CONFIG_H
#define APP_AZURE_IOT_CONFIG_H

#ifdef __cplusplus
extern   "C" {
#endif

// use the IoTConnect app config file for WiFi credentials
#include "app_config.h"
#if 0
/*
TODO`s: Configure wifi settings.
*/
#define WIFI_SSID                                   ""
#define WIFI_PASSWORD                               ""
/*
END TODO section
*/
#endif


/* This application uses Symmetric key (SAS) to connect to IoT Hub by default,
   simply defining USE_DEVICE_CERTIFICATE and setting your device certificate in device_identity.c
   to connect to IoT Hub with x509 certificate. Set up X.509 security in your Azure IoT Hub,
   refer to https://docs.microsoft.com/en-us/azure/iot-hub/iot-hub-security-x509-get-started  */
/* #define USE_DEVICE_CERTIFICATE                      1  */

/*
TODO`s: Configure core settings of application for your IoTHub.
*/

/* Defined, DPS is enabled.  */
/*
#define ENABLE_DPS
*/

#ifndef ENABLE_DPS

/* Required when DPS is not used.  */
/* These values can be picked from device connection string which is of format : HostName=<host1>;DeviceId=<device1>;SharedAccessKey=<key1>
   HOST_NAME can be set to <host1>,
   DEVICE_ID can be set to <device1>,
   DEVICE_SYMMETRIC_KEY can be set to <key1>.  */
#ifndef HOST_NAME
#define HOST_NAME                                   ""
#endif /* HOST_NAME */

#ifndef DEVICE_ID
#define DEVICE_ID                                   ""
#endif /* DEVICE_ID */

#else /* !ENABLE_DPS */

/* Required when DPS is used.  */
#ifndef ENDPOINT
#define ENDPOINT                                    ""
#endif /* ENDPOINT */

#ifndef ID_SCOPE
#define ID_SCOPE                                    ""
#endif /* ID_SCOPE */

#ifndef REGISTRATION_ID
#define REGISTRATION_ID                             ""
#endif /* REGISTRATION_ID */

#endif /* ENABLE_DPS */

/* Optional SYMMETRIC KEY.  */
#ifndef DEVICE_SYMMETRIC_KEY
#define DEVICE_SYMMETRIC_KEY                        ""
#endif /* DEVICE_SYMMETRIC_KEY */

/* Optional module ID.  */
#ifndef MODULE_ID
#define MODULE_ID                                   ""
#endif /* MODULE_ID */

#if (USE_DEVICE_CERTIFICATE == 1)

/* Using X509 certificate authenticate to connect to IoT Hub,
   set the device certificate as your device.  */

/* Device Key type. */
#ifndef DEVICE_KEY_TYPE
#define DEVICE_KEY_TYPE                             NX_SECURE_X509_KEY_TYPE_RSA_PKCS1_DER
#endif /* DEVICE_KEY_TYPE */

#endif /* USE_DEVICE_CERTIFICATE */

/*
END TODO section
*/

/* Define the Azure Iot Plug and Play device template */
#define NX_AZURE_IOT_PNP_MODEL_ID                    "dtmi:stmicroelectronics:b_u585i_iot02a:standard_fw;3"

/* Define the Azure RTOS IOT thread stack and priority.  */
#ifndef NX_AZURE_IOT_STACK_SIZE
#define NX_AZURE_IOT_STACK_SIZE                     (2560)
#endif /* NX_AZURE_IOT_STACK_SIZE */

#ifndef NX_AZURE_IOT_THREAD_PRIORITY
#define NX_AZURE_IOT_THREAD_PRIORITY                (4)
#endif /* NX_AZURE_IOT_THREAD_PRIORITY */

/* Define the Azure IOT max size of device id and hostname buffers */
#ifndef NX_AZURE_IOT_MAX_BUFFER
#define NX_AZURE_IOT_MAX_BUFFER                    (256)
#endif /* NX_AZURE_IOT_MAX_BUFFER */

#ifdef __cplusplus
}
#endif
#endif /* APP_AZURE_IOT_CONFIG_H */
