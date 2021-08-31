//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define WIFI_SSID       "your wifi ssid"
#define WIFI_PASSWORD   "your wifi password"

#define IOTCONNECT_CPID "your cpid"
#define IOTCONNECT_ENV  "your environment"

//#define IOTCONNECT_DUID "custom_DUID" // you can supply a custom device UID, or...
#define DUID_PREFIX "stm32-" // mac address will be appended in format 012345abcdef

// if it is not defined, device certs will be used in iotconnect_app.c
#define IOTCONNECT_SYMETRIC_KEY "symetric key"

#define SAMPLE_SNTP_SERVER_NAME "time.google.com"    /* SNTP Server.  */

#endif // APP_CONFIG_H
