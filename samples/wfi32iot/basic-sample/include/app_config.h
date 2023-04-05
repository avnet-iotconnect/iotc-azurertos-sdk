//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 12/12/22.
//

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#define ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE

// Unlike other applications in this repo, the configuration for this project is
// loaded from the USB Mass Storage Device CLOUD.CFG file.
// The default JSON of the configuration file will be pre-loaded. 
// Fill the document's JSON values per your account settings.
// DUID will be ignored if secure element X509 authentication is used. It can be left blank.
// If symmetric key is left blank, then X509 authentication will be used.
// If ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE is set, make sure that symmetric key is left blank in the document.
 typedef struct {
    char *env;    // Environment name. Contact your representative for details.
    char *cpid;   // Settings -> Company Profile.
    char *duid;   // Name of the device - ignored for X509 secure element authentication.
    char *symmetric_key;   // Symmetric key specified in JSON or NULL
} IotConnectAppConfig;

// This function will return the config read from the JSON by the MSD driver.
IotConnectAppConfig* get_app_config(void);

#define SAMPLE_SNTP_SERVER_NAME "time.apple.com"    /* SNTP Server.  */

#endif // APP_CONFIG_H
