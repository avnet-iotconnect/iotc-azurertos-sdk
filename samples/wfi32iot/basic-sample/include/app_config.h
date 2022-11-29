//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#define IOTCONNECT_APP_USE_WEATHER_CLICK

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define IOTCONNECT_CPID "your_cpid"
#define IOTCONNECT_ENV  "your_environment"

// If using the ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE define, the DUID will be automatically generated
//#define IOTCONNECT_DUID "your_duid" // you can supply a custom device UID, or...
#define DUID_PREFIX "mchip-" // mac address will be appended in format 012345abcdef

// if it is not defined, device certs or secure element will be used in iotconnect_app.c
// #define IOTCONNECT_SYMETRIC_KEY ""

#define SAMPLE_SNTP_SERVER_NAME "time.google.com"    /* SNTP Server.  */

#define ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE

#endif // APP_CONFIG_H
