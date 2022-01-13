//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define WIFI_SSID       "your wifi ssid"
#define WIFI_PASSWORD   "your wifi password"

#define IOTCONNECT_CPID ""
#define IOTCONNECT_ENV  ""

#define IOTCONNECT_DUID "" // you can supply a custom device UID, or...
#define DUID_PREFIX "nxp-" // mac address will be appended in format 012345abcdef

// if it is not defined, device certs will be used in iotconnect_app.c
#define IOTCONNECT_SYMETRIC_KEY ""

#define SAMPLE_SNTP_SERVER_NAME "time.google.com"    /* SNTP Server.  */

#endif // APP_CONFIG_H
