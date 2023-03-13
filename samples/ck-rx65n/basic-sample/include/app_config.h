//
// Copyright: Avnet 2022
// Created by Russell Bateman <rbateman@witekio.com> on 11/18/22.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define IOTCONNECT_CPID ""
#define IOTCONNECT_ENV  ""

#define IOTCONNECT_DUID "" // you can supply a custom device UID, or...
#define DUID_PREFIX "" // mac address will be appended in format 012345abcdef

// if it is not defined, device certs will be used in iotconnect_app.c
//#define IOTCONNECT_SYMETRIC_KEY ""

// Enable Renesas RX TSIP hardware crypto core based X.509 authentication
#define ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE

#define SAMPLE_SNTP_SERVER_NAME "time.google.com"    /* SNTP Server.  */

#endif // APP_CONFIG_H
