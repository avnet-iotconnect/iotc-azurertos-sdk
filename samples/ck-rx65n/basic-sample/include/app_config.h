//
// Copyright: Avnet 2022
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

//
// If SYMMETRIC_KEY_INPUT is defined, then the user
// will be prompted on a terminal to input values to be used for:
// - IOTCONNECT_CPID
// - IOTCONNECT_ENV
// - IOTCONNECT_DUID
// - IOTCONNECT_SYMETRIC_KEY
//
// X.509 certificate based schemes still require setting values in this file and recompiling
// without SYMMETRIC_KEY_INPUT defined.
//
#define SYMMETRIC_KEY_INPUT 1

#ifndef SYMMETRIC_KEY_INPUT
#define IOTCONNECT_CPID ""
#define IOTCONNECT_ENV  ""

#define IOTCONNECT_DUID "" // you can supply a custom device UID, or...
#define DUID_PREFIX "" // mac address will be appended in format 012345abcdef

// if it is not defined, device certs will be used in iotconnect_app.c
//#define IOTCONNECT_SYMETRIC_KEY ""

// Enable Renesas RX TSIP hardware crypto core based X.509 authentication
#define ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
#endif // IOTCONNECT_INTERACTIVE_SYMMETRIC_KEY

#define SAMPLE_SNTP_SERVER_NAME "time.apple.com"    /* SNTP Server.  */

#endif // APP_CONFIG_H
