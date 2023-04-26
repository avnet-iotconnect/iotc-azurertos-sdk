//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// for the TFM application, we will conflict on the WiFi SSID. It's supposed to be configured via console.
#define WIFI_SSID       "your WiFi SSID"
#define WIFI_PASSWORD   "your WiFi password"

#define IOTCONNECT_CPID "your CPID"
#define IOTCONNECT_ENV  "your environment"

#define IOTCONNECT_DUID "your device ID"

// if it is not defined, device certs will be used in iotconnect_app.c
#define IOTCONNECT_SYMETRIC_KEY "your symmetric key"

#define SAMPLE_SNTP_SERVER_NAME "time.google.com"    /* SNTP Server.  */

#endif // APP_CONFIG_H
