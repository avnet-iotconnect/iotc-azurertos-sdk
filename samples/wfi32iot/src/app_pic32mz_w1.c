/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_pic32mz_w1.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdio.h>
#include "definitions.h"

#include "system/console/sys_console.h"
#include "driver/wifi/pic32mzw1/include/wdrv_pic32mzw.h"
#include "driver/wifi/pic32mzw1/include/wdrv_pic32mzw_sta.h"
#include "driver/wifi/pic32mzw1/include/wdrv_pic32mzw_authctx.h"
#include "driver/wifi/pic32mzw1/include/wdrv_pic32mzw_bssctx.h"
#include "driver/wifi/pic32mzw1/include/wdrv_pic32mzw_bssfind.h"
#include "driver/wifi/pic32mzw1/include/wdrv_pic32mzw_common.h"
#include "driver/wifi/pic32mzw1/include/drv_pic32mzw1_crypto.h"
#include "config/pic32mz_w1/bsp/bsp.h"

#include "nx_crypto.h"
#include "nx_crypto_ec.h"
#include "nx_secure_x509.h"

#include "atca_basic.h"
#include "tng_atcacert_client.h"
#include "app_pic32mz_w1.h"
#include "cJSON.h"
#include "app_led.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
typedef enum 
{
    OPEN = 1,
    WPAWPA2MIXED,
    WEP,
    WPA2WPA3MIXED,
    WIFI_AUTH_MAX
} WIFI_AUTH;

typedef struct wifiConfiguration {
    WDRV_PIC32MZW_AUTH_CONTEXT authCtx;
    WDRV_PIC32MZW_BSS_CONTEXT bssCtx;
} wifiConfig;
wifiConfig g_wifiConfig;

typedef struct wifiCredentials {
    uint8_t ssid[WDRV_PIC32MZW_MAX_SSID_LEN];
    uint8_t auth;
    uint8_t key[WDRV_PIC32MZW_MAX_PSK_PASSWORD_LEN];
} wifiCred;
wifiCred wifi;


/* Default STA credentials */
#define APP_STA_DEFAULT_SSID        "wsn-2g"
#define APP_STA_DEFAULT_PASSPHRASE  "brucenegley"
#define APP_STA_DEFAULT_AUTH        WPAWPA2MIXED

/* Wi-Fi credentials status*/
#define CREDENTIALS_UNINITIALIZED       0
#define CREDENTIALS_VALID               1
#define CREDENTIALS_INVALID             2

#define SET_WIFI_CREDENTIALS(val)   app_pic32mz_w1Data.validCrednetials = val
#define CHECK_WIFI_CREDENTIALS()     app_pic32mz_w1Data.validCrednetials

#define REGISTRATION_ID_MAX_LEN 64
UCHAR g_registration_id[REGISTRATION_ID_MAX_LEN];
UINT g_registration_id_length;

extern UCHAR sample_device_private_key_ptr[];
extern UINT sample_device_private_key_len;

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_PIC32MZ_W1_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_PIC32MZ_W1_DATA app_pic32mz_w1Data;

/* Work buffer used by FAT FS during Format */
uint8_t CACHE_ALIGN work[SYS_FS_FAT_MAX_SS];

char fileBuffer[1024];

char default_id_scope[64]         =   "0ne006B6CF8";
char default_registration_id[64]  =   "PIC32MZW1";
char default_primary_key[128]     =   "mujHRQMx8dUsZETtlWxSonGZ24++L69c8KjIvZDT+5M=" ;  
  
extern uint8_t* _WDRV_PIC32MZW_Task_Stk_Ptr;

extern APP_CONNECT_STATUS appConnectStatus;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

/* USB event callback */
void USBDeviceEventHandler(USB_DEVICE_EVENT event, void * pEventData, uintptr_t context) {
    APP_PIC32MZ_W1_DATA * app_pic32mz_w1Data = (APP_PIC32MZ_W1_DATA*) context;
    switch (event) {
        case USB_DEVICE_EVENT_RESET:
        case USB_DEVICE_EVENT_DECONFIGURED:
            break;

        case USB_DEVICE_EVENT_CONFIGURED:
            break;

        case USB_DEVICE_EVENT_SUSPENDED:
            break;

        case USB_DEVICE_EVENT_POWER_DETECTED:
            /* VBUS is detected. Attach the device. */
            USB_DEVICE_Attach(app_pic32mz_w1Data->usbDeviceHandle);
            break;

        case USB_DEVICE_EVENT_POWER_REMOVED:
            /* VBUS is not detected. Detach the device */
            USB_DEVICE_Detach(app_pic32mz_w1Data->usbDeviceHandle);
            break;

            /* These events are not used in this demo */
        case USB_DEVICE_EVENT_RESUMED:
        case USB_DEVICE_EVENT_ERROR:
        case USB_DEVICE_EVENT_SOF:
        default:
            break;
    }
}

/* Wi-Fi connect callback */
static void wifiConnectCallback(DRV_HANDLE handle, WDRV_PIC32MZW_ASSOC_HANDLE assocHandle, WDRV_PIC32MZW_CONN_STATE currentState)
{
    switch (currentState) {
        case WDRV_PIC32MZW_CONN_STATE_DISCONNECTED:
            appConnectStatus.wifi = false;
            SYS_CONSOLE_MESSAGE("WiFi Reconnecting\r\n");            
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_RECONNECT;
            break;
        case WDRV_PIC32MZW_CONN_STATE_CONNECTED:
            appConnectStatus.wifi = true;
            LED_BLUE_On();
            GPIO_RA14_Clear();
            GPIO_RA13_Set();
            SYS_CONSOLE_MESSAGE("WiFi Connected\r\n");
            break;
        case WDRV_PIC32MZW_CONN_STATE_FAILED:
            appConnectStatus.wifi = false;
            SYS_CONSOLE_MESSAGE("WiFi connection failed\r\n");
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_RECONNECT;        
            break;
        case WDRV_PIC32MZW_CONN_STATE_CONNECTING:
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

/* Parse Wi-Fi configuration file */
/* Format is CMD:SEND_UART=wifi <SSID>,<PASSPHRASE>,<AUTH>*/

static bool parseWifiConfig(char *configBuffer)
{
    char *p; 
    bool ret = true;
    
    p = strtok(configBuffer, " ");
    if(p != NULL && !strncmp(p, APP_USB_MSD_WIFI_CONFIG_ID, strlen(APP_USB_MSD_WIFI_CONFIG_ID)))
    {
        p = (&configBuffer[strlen(APP_USB_MSD_WIFI_CONFIG_ID)+1]);
        p = strtok(NULL, ",");
        if (p)
            strcpy((char *)wifi.ssid, p);

        p = strtok(NULL, ",");
        if (p) 
        {
            if(strlen(p) == 1){ //<ssid>,<no key>, <open auth> ==> this token is auth type
                if(atoi(p) == OPEN)
                    wifi.auth = OPEN;
                else
                    ret = false;
            }
            else{               //<ssid,<key>, <non-open auth> ==> this token is key
                strcpy((char *) wifi.key, p);
                p = strtok(NULL, ",");
                if (p) 
                    wifi.auth = atoi(p);
                else
                    ret = false;
            }
        }
        else
            ret = false;

        SYS_CONSOLE_PRINT("SSID:%s - PASSPHRASE:%s - AUTH:%d\r\n", 
                                        wifi.ssid, 
                                        wifi.key, 
                                        wifi.auth);
    }
    return ret;
}


/* Parse Wi-Fi configuration file */
/* Format is CMD:SEND_UART=wifi <SSID>,<PASSPHRASE>,<AUTH>*/

static bool parseCloudConfig(char *configBuffer)
{    
    /*Parse the file */
    cJSON *messageJson = cJSON_Parse(configBuffer);
    if (messageJson == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();            
        if (error_ptr != NULL) {
            SYS_CONSOLE_PRINT("Message JSON parse Error. Error before: %s \r\n", error_ptr);
    }
        cJSON_Delete(messageJson);
        return false;
    }
            
    cJSON *id_scope_ep = cJSON_GetObjectItem(messageJson, AZURE_CLOUD_IDSCOPE_JSON_TAG);
    if (!id_scope_ep || id_scope_ep->type !=cJSON_String ) {
        SYS_CONSOLE_PRINT("JSON "AZURE_CLOUD_IDSCOPE_JSON_TAG" parsing error\r\n");
        cJSON_Delete(messageJson);
        return -1;
    }
    memset(default_id_scope, 0, sizeof(default_id_scope));
    sprintf((char *)default_id_scope, "%s", id_scope_ep->valuestring);

    //Get the ClientID
    cJSON *device_id_ep = cJSON_GetObjectItem(messageJson, AZURE_CLOUD_DEVICEID_JSON_TAG);
    if (!device_id_ep || device_id_ep->type !=cJSON_String ) {
        SYS_CONSOLE_PRINT("JSON "AZURE_CLOUD_IDSCOPE_JSON_TAG" parsing error\r\n");
        cJSON_Delete(messageJson);
        return false;
    }
    memset(default_registration_id, 0, sizeof(default_registration_id));
    sprintf((char *)default_registration_id, "%s", device_id_ep->valuestring);
    
    //Get the ClientID
    cJSON *primary_key_ep = cJSON_GetObjectItem(messageJson, AZURE_CLOUD_PRIMARY_KEY_JSON_TAG);
    if (!primary_key_ep || primary_key_ep->type !=cJSON_String ) {
        SYS_CONSOLE_PRINT("JSON "AZURE_CLOUD_IDSCOPE_JSON_TAG" parsing error\r\n");
        cJSON_Delete(messageJson);
        return false;
    }
    memset(default_primary_key, 0, sizeof(default_primary_key));
    sprintf((char *)default_primary_key, "%s", primary_key_ep->valuestring);
    
    cJSON_Delete(messageJson);
    SYS_CONSOLE_PRINT(
            "Data parsed from cloud.cfg - registration id will be overwritten with data extracted from certificate if X.509 authentication method is used\r\n");
    SYS_CONSOLE_PRINT("id_scope:%s - registration_id:%s - primary_key:%s \r\n", 
                                        default_id_scope, 
                                        default_registration_id, 
                                        default_primary_key);
    
    return true;
}
/* TODO:  Add any necessary local functions.
*/
/* Store Wi-Fi configurations to global g_wifiConfig struct */
bool APP_WifiConfig(char *ssid, char *pass, WIFI_AUTH auth, uint8_t channel)
{
    bool ret = true;
    uint8_t ssidLength = strlen((const char *) ssid);
    uint8_t pskLength = strlen(pass);

    WDRV_PIC32MZW_BSSCtxSetChannel(&g_wifiConfig.bssCtx, (WDRV_PIC32MZW_CHANNEL_ID)channel);
    //SYS_CONSOLE_PRINT("SSID is %s \r\n", ssid);

    if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSCtxSetSSID(&g_wifiConfig.bssCtx, (uint8_t * const) ssid, ssidLength)) {
        switch (auth) {
            case OPEN:
            {
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetOpen(&g_wifiConfig.authCtx)) {
                    SYS_CONSOLE_MESSAGE("APP_WFI: ERROR- Unable to set Authentication\r\n");
                    ret = false;
                }
                break;
            }

            case WPAWPA2MIXED:
            {
                if (WDRV_PIC32MZW_STATUS_OK != WDRV_PIC32MZW_AuthCtxSetPersonal(&g_wifiConfig.authCtx, (uint8_t *) pass, pskLength, WDRV_PIC32MZW_AUTH_TYPE_WPAWPA2_PERSONAL)) {
                    SYS_CONSOLE_MESSAGE("ERROR - Unable to set authentication to WPAWPA2 MIXED\r\n");
                    ret = false;
                }
                break;
            }
            
            case WEP:
            {
                //TODO
                break;
            }
            default:
            {
                SYS_CONSOLE_MESSAGE("ERROR-Set Authentication type");
                ret = false;
            }
        }
    }
    return ret;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_PIC32MZ_W1_Initialize ( void )

  Remarks:
    See prototype in app_pic32mz_w1.h.
 */

void APP_PIC32MZ_W1_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_MOUNT_DISK;



    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_PIC32MZ_W1_Tasks ( void )

  Remarks:
    See prototype in app_pic32mz_w1.h.
 */

void APP_PIC32MZ_W1_Tasks ( void )
{
    SYS_FS_FORMAT_PARAM opt;  
    /* Check the application's current state. */
    switch ( app_pic32mz_w1Data.appPic32mzW1State )
    {        
        case APP_PIC32MZ_W1_STATE_MOUNT_DISK:
        {            
            /* Mount the disk */
            if(SYS_FS_Mount(APP_DEVICE_NAME, APP_MOUNT_NAME, APP_FS_TYPE, 0, NULL) != 0)
            {
                /* The disk could not be mounted. Try mounting again until
                 * the operation succeeds. */
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_MOUNT_DISK;
            }
            else
            {      
                /* Mount was successful. Format the disk. */
                if (SYS_FS_ERROR_NO_FILESYSTEM == SYS_FS_Error())
                {
                    SYS_CONSOLE_PRINT("File system Mount error!\r\n");
                    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_FORMAT_DISK;                    
                }
                else
                {                    
                    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_MSD_CONNECT;
                    SYS_FS_DriveLabelSet(APP_MOUNT_NAME, "WFI32-IoT");
                    SYS_FS_CurrentDriveSet(APP_MOUNT_NAME);   
                }
            }

            break;
        }
        case APP_PIC32MZ_W1_STATE_MSD_CONNECT:
        {              
            app_pic32mz_w1Data.usbDeviceHandle = USB_DEVICE_Open(USB_DEVICE_INDEX_0, DRV_IO_INTENT_READWRITE);
            if (app_pic32mz_w1Data.usbDeviceHandle != USB_DEVICE_HANDLE_INVALID) {
                /* Set the Event Handler. We will start receiving events after
                 * the handler is set */

                USB_DEVICE_EventHandlerSet(app_pic32mz_w1Data.usbDeviceHandle, USBDeviceEventHandler, (uintptr_t) &app_pic32mz_w1Data);
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_CHECK_CERT_FILES;                
            } else {
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
            }
            break;
        }
        
        case APP_PIC32MZ_W1_STATE_FORMAT_DISK:
        {
            opt.fmt = SYS_FS_FORMAT_FAT;
            opt.au_size = 0;

            if (SYS_FS_DriveFormat (APP_MOUNT_NAME, &opt, (void *)work, SYS_FS_FAT_MAX_SS) != SYS_FS_RES_SUCCESS)
            {
                /* Format of the disk failed. */
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
            }
            else
            {
                SYS_FS_DriveLabelSet(APP_MOUNT_NAME, "WFI32-IoT");
                SYS_FS_CurrentDriveSet(APP_MOUNT_NAME);                
                /* Format succeeded. Open a file. */
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_MSD_CONNECT;                              
            
            }
            break;
        }
                
        case APP_PIC32MZ_W1_STATE_CHECK_CERT_FILES:
        {     
            extern ATCAIfaceCfg atecc608_0_init_data;
            ATCA_STATUS atcaStat;
            int status;
            char certFileName[ATCA_SERIAL_NUM_SIZE*3+15];
               
            /* set the next default case */
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_CHECK_AZURE_CFG_FILE;
                       
            atcaStat = atcab_init(&atecc608_0_init_data);            
            if (ATCA_SUCCESS == atcaStat) 
            {                    
                /* Uncomment below code to read and print ECC608's serial number */
                uint8_t sernum[ATCA_SERIAL_NUM_SIZE], tempAsciiBuf[ATCA_SERIAL_NUM_SIZE*3];
                size_t displen = sizeof (tempAsciiBuf);                
                
                size_t certificateSize = 0;
                uint8_t certificate[CERT_MAX_SIZE];
                size_t b64_certificateSize;
                char b64_certificate[CERT_MAX_SIZE];
                
                
                atcab_read_serial_number(sernum);
                                    
                memset(app_pic32mz_w1Data.ecc608SerialNum, 0, sizeof(app_pic32mz_w1Data.ecc608SerialNum));
                atcab_bin2hex_(sernum, ATCA_SERIAL_NUM_SIZE, (char *)tempAsciiBuf, &displen, false, false, true);
                sprintf(app_pic32mz_w1Data.ecc608SerialNum, "%s", tempAsciiBuf);                
                SYS_CONSOLE_PRINT("Serial Number of the Device: %s\r\n", app_pic32mz_w1Data.ecc608SerialNum);
                                
            
                /* Read the device certificate from ECC608 and store it to 
                 * 1) app_pic32mz_w1Data struct in binary, and if needed,
                 * 2) filesystem in PEM format 
                 */
                
                status = tng_atcacert_max_device_cert_size(&certificateSize);
                if (ATCA_SUCCESS != status) {
                    SYS_CONSOLE_PRINT("tng_atcacert_max_device_cert_size Failed \r\n");
                }
                if (certificateSize > CERT_MAX_SIZE) {
                    SYS_CONSOLE_PRINT("Certificate size (%d) exceeds the maximum (%d)\r\n", certificateSize, CERT_MAX_SIZE);
                    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                }
                status = tng_atcacert_read_device_cert((uint8_t*) & certificate, &certificateSize, NULL);
                if (ATCA_SUCCESS != status) {
                    SYS_CONSOLE_PRINT("tng_atcacert_read_device_cert Failed (%x) \r\n", status);
                }
                else {
                    /* Store device cert for later use */
                    app_pic32mz_w1Data.certSize = certificateSize;
                    memcpy(app_pic32mz_w1Data.ecc608DeviceCert, certificate, certificateSize);
                    sprintf(certFileName, "sn%s_device.pem", app_pic32mz_w1Data.ecc608SerialNum);
                    if (SYS_FS_FileStat(certFileName, &app_pic32mz_w1Data.fileStatus) != SYS_FS_RES_SUCCESS) {
                
                        app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(certFileName, SYS_FS_FILE_OPEN_WRITE);
                        if (app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID) {
                            
                            atcab_base64encode(certificate, certificateSize, b64_certificate, &b64_certificateSize);

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, PEM_HEADER, sizeof (PEM_HEADER) - 1) != (sizeof (PEM_HEADER) - 1)) {
                                SYS_CONSOLE_PRINT("Device certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, b64_certificate, b64_certificateSize) == b64_certificateSize) {
                                SYS_FS_FileSync(app_pic32mz_w1Data.fileHandle);
                                SYS_CONSOLE_PRINT("Device certificate saved in PEM format!\r\n");
                            } else {
                                SYS_CONSOLE_PRINT("Device certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, PEM_FOOTER, sizeof (PEM_FOOTER) - 1) != (sizeof (PEM_FOOTER) - 1)) {
                                SYS_CONSOLE_PRINT("Device certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle);
                        }
                        else
                        {
                            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            SYS_CONSOLE_PRINT("File open failed!\r\n");
                        } 
                    }
                }
                
                /* Read the signer certificate from ECC608 and store it to filesystem in PEM format */
                sprintf(certFileName, "sn%s_signer.pem", app_pic32mz_w1Data.ecc608SerialNum);                
                if(SYS_FS_FileStat(certFileName, &app_pic32mz_w1Data.fileStatus) != SYS_FS_RES_SUCCESS)            
                {                               
                    status = tng_atcacert_max_device_cert_size(&certificateSize);
                    if (ATCA_SUCCESS != status) {
                        SYS_CONSOLE_PRINT("tng_atcacert_max_device_cert_size Failed \r\n");                     
                    }
                    if (certificateSize > CERT_MAX_SIZE) {
                        /* TODO: */
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    status = tng_atcacert_read_signer_cert((uint8_t*) &certificate, &certificateSize);
                    if (ATCA_SUCCESS != status) {
                        SYS_CONSOLE_PRINT("tng_atcacert_read_signer_cert Failed (%x) \r\n", status);                        
                    }
                    else
                    {                                                      
                        app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(certFileName, SYS_FS_FILE_OPEN_WRITE);
                        if (app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID) {
                            
                            atcab_base64encode(certificate, certificateSize, b64_certificate, &b64_certificateSize);

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, PEM_HEADER, sizeof (PEM_HEADER) - 1) != (sizeof (PEM_HEADER) - 1)) {
                                SYS_CONSOLE_PRINT("Signer certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, b64_certificate, b64_certificateSize) == b64_certificateSize) {
                                SYS_FS_FileSync(app_pic32mz_w1Data.fileHandle);
                                SYS_CONSOLE_PRINT("Signer certificate saved in PEM format!\r\n");
                            } else {
                                SYS_CONSOLE_PRINT("Signer certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, PEM_FOOTER, sizeof (PEM_FOOTER) - 1) != (sizeof (PEM_FOOTER) - 1)) {
                                SYS_CONSOLE_PRINT("Signer certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle);
                        }
                        else
                        {
                            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            SYS_CONSOLE_PRINT("File open failed!\r\n");
                        } 
                    }
                }

                  /* Read the root certificate from ECC608 and store it to filesystem in PEM format */
                sprintf(certFileName, "sn%s_root.pem", app_pic32mz_w1Data.ecc608SerialNum);                
                if(SYS_FS_FileStat(certFileName, &app_pic32mz_w1Data.fileStatus) != SYS_FS_RES_SUCCESS)            
                {                               
                    status = tng_atcacert_max_device_cert_size(&certificateSize);
                    if (ATCA_SUCCESS != status) {
                        SYS_CONSOLE_PRINT("tng_atcacert_max_device_cert_size Failed \r\n");                     
                    }
                    if (certificateSize > CERT_MAX_SIZE) {
                        /* TODO: */
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    status = tng_atcacert_root_cert((uint8_t*) &certificate, &certificateSize);
                    if (ATCA_SUCCESS != status) {
                        SYS_CONSOLE_PRINT("tng_atcacert_root_cert Failed (%x) \r\n", status);                        
                    }
                    else
                    {                                                      
                        app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(certFileName, SYS_FS_FILE_OPEN_WRITE);
                        if (app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID) {
                            
                            atcab_base64encode(certificate, certificateSize, b64_certificate, &b64_certificateSize);

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, PEM_HEADER, sizeof (PEM_HEADER) - 1) != (sizeof (PEM_HEADER) - 1)) {
                                SYS_CONSOLE_PRINT("Root certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, b64_certificate, b64_certificateSize) == b64_certificateSize) {
                                SYS_FS_FileSync(app_pic32mz_w1Data.fileHandle);
                                SYS_CONSOLE_PRINT("Root certificate saved in PEM format!\r\n");
                            } else {
                                SYS_CONSOLE_PRINT("Root certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            if (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, PEM_FOOTER, sizeof (PEM_FOOTER) - 1) != (sizeof (PEM_FOOTER) - 1)) {
                                SYS_CONSOLE_PRINT("Root certificate file write fail!\r\n");
                                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            }

                            SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle);
                        }
                        else
                        {
                            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                            SYS_CONSOLE_PRINT("File open failed!\r\n");
                        } 
                    }
                }
                
                /* Use temporary cert struct for now as we need to extract the common name;
                 * this will be done again later, but we need the common name for registration ID 
                 * before that. With standard TNG devices the common name is always "snX", where 
                 * X is the serial number, but can we trust it's always the case? 
                 
                 * The private keys don't matter at this point 
                 */
                        
                NX_SECURE_X509_CERT device_certificate;
                
                
                nx_secure_x509_certificate_initialize(&device_certificate,
                                                            (UCHAR *)app_pic32mz_w1Data.ecc608DeviceCert, (USHORT)app_pic32mz_w1Data.certSize,
                                                            NX_NULL, 0,
                                                            NULL, 0,
                                                            NX_SECURE_X509_KEY_TYPE_HARDWARE);
                
                g_registration_id_length = device_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name_length;
                if (g_registration_id_length < REGISTRATION_ID_MAX_LEN) {
                    memcpy(g_registration_id, device_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name, g_registration_id_length);
                    SYS_CONSOLE_PRINT("Device Certificate Subject Common Name (used as a registration ID in DPS) is \r\n %.*s, length is %u\r\n\n",
                            g_registration_id_length,
                            g_registration_id,
                            g_registration_id_length);
                    
                } else {
                   SYS_CONSOLE_PRINT("Error in getting the common name!\r\n");
                   app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                }

              
                atcab_release();             
            }
            else
            {
                SYS_CONSOLE_PRINT("atcab_init failed, check ECC608!\r\n");                
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;                
            }
        break;
        }        
        case APP_PIC32MZ_W1_STATE_CHECK_AZURE_CFG_FILE:
        {       
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_CHECK_WIFI_CFG_FILE;
            if(SYS_FS_FileStat(AZURE_CLOUD_CFG_FILE_NAME, &app_pic32mz_w1Data.fileStatus) != SYS_FS_RES_SUCCESS)            
            {                
                app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(AZURE_CLOUD_CFG_FILE, SYS_FS_FILE_OPEN_WRITE);              
                if(app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID)
                {                    
                    cJSON *jsonObj = cJSON_CreateObject();
                    cJSON_AddItemToObject(jsonObj, AZURE_CLOUD_IDSCOPE_JSON_TAG, cJSON_CreateString((const char *)default_id_scope));
                    cJSON_AddItemToObject(jsonObj, AZURE_CLOUD_DEVICEID_JSON_TAG, cJSON_CreateString((const char *)default_registration_id));  
                    cJSON_AddItemToObject(jsonObj, AZURE_CLOUD_PRIMARY_KEY_JSON_TAG, cJSON_CreateString((const char *)default_primary_key));  
                    char * printBuffer = cJSON_Print(jsonObj);
                                            
                    if(printBuffer && (SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, printBuffer, strlen(printBuffer)) == strlen(printBuffer)))                                                  
                    { 
                        SYS_FS_FileSync(app_pic32mz_w1Data.fileHandle);                        
                    }
                    else
                    {
                        SYS_CONSOLE_PRINT("Cloud Configuration file write fail!\n");
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    cJSON_Delete(jsonObj);
                    SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle);  
                }
                else
                {
                    SYS_CONSOLE_PRINT("File open failed!\n");
                    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                }
                
            }
            break;
        }                
        case APP_PIC32MZ_W1_STATE_CHECK_WIFI_CFG_FILE:
        {                                 
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_READ_CERT_WIFI_CLOUD_CFG_FILE;
            if(SYS_FS_FileStat(AZURE_WIFI_CFG_FILE_NAME, &app_pic32mz_w1Data.fileStatus) != SYS_FS_RES_SUCCESS)            
            {                
                app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(AZURE_WIFI_CFG_FILE, SYS_FS_FILE_OPEN_WRITE);              
                if(app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID)
                {                    
                    memset(fileBuffer, 0, sizeof(fileBuffer));
                    sprintf(fileBuffer, APP_USB_MSD_WIFI_CONFIG_DATA_TEMPLATE, APP_STA_DEFAULT_SSID, APP_STA_DEFAULT_PASSPHRASE, APP_STA_DEFAULT_AUTH);
                    if(SYS_FS_FileWrite(app_pic32mz_w1Data.fileHandle, fileBuffer, strlen(fileBuffer)) == strlen(fileBuffer))                              
                    { 
                        SYS_FS_FileSync(app_pic32mz_w1Data.fileHandle);                                                 
                    }
                    else
                    {                                                
                        SYS_CONSOLE_PRINT("Wifi Configuration file write fail!\n");
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle); 
                }
                else
                {
                    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    SYS_CONSOLE_PRINT("File open failed!\n");
                }                                
            }
            break;
        }   
        
        case APP_PIC32MZ_W1_STATE_READ_CERT_WIFI_CLOUD_CFG_FILE:
        {
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_INIT;                        
            if(SYS_FS_FileStat(AZURE_WIFI_CFG_FILE_NAME, &app_pic32mz_w1Data.fileStatus) == SYS_FS_RES_SUCCESS)            
            {
                app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(AZURE_WIFI_CFG_FILE, SYS_FS_FILE_OPEN_READ);              
                if(app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID)
                {                    
                    memset(fileBuffer, 0, sizeof(fileBuffer));                
                    if(SYS_FS_FileRead(app_pic32mz_w1Data.fileHandle, fileBuffer, app_pic32mz_w1Data.fileStatus.fsize) == app_pic32mz_w1Data.fileStatus.fsize)                              
                    {                            
                        parseWifiConfig(fileBuffer);                      
                    }
                    else
                    {                                                
                        SYS_CONSOLE_PRINT("Wifi Configuration file read fail!\n");
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle); 
                }
            }
            
            if(SYS_FS_FileStat(AZURE_CLOUD_CFG_FILE_NAME, &app_pic32mz_w1Data.fileStatus) == SYS_FS_RES_SUCCESS)            
            {
                app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(AZURE_CLOUD_CFG_FILE, SYS_FS_FILE_OPEN_READ);              
                if(app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID)
                {                    
                    memset(fileBuffer, 0, sizeof(fileBuffer));                
                    if(SYS_FS_FileRead(app_pic32mz_w1Data.fileHandle, fileBuffer, app_pic32mz_w1Data.fileStatus.fsize) == app_pic32mz_w1Data.fileStatus.fsize)                              
                    { 
                        parseCloudConfig(fileBuffer);                       
                    }
                    else
                    {                                                
                        SYS_CONSOLE_PRINT("Cloud Configuration file read fail!\n");
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle); 
                }
            }
#if USE_CERTIFICATE            
            /* Read the device certificate file */
            if(SYS_FS_FileStat(app_pic32mz_w1Data.ecc608SerialNum, &app_pic32mz_w1Data.fileStatus) == SYS_FS_RES_SUCCESS)            
            {
                
                app_pic32mz_w1Data.fileHandle = SYS_FS_FileOpen(app_pic32mz_w1Data.fileStatus.fname, SYS_FS_FILE_OPEN_READ);              
                if(app_pic32mz_w1Data.fileHandle != SYS_FS_HANDLE_INVALID)
                {
                    SYS_CONSOLE_PRINT("File open for reading!\n");
                    memset(fileBuffer, 0, sizeof(fileBuffer));                
                    if(SYS_FS_FileRead(app_pic32mz_w1Data.fileHandle, fileBuffer, app_pic32mz_w1Data.fileStatus.fsize) == app_pic32mz_w1Data.fileStatus.fsize)                              
                    { 
                        SYS_CONSOLE_PRINT("Successfully read the %d device certificate!\n%s\n", app_pic32mz_w1Data.fileStatus.fsize, fileBuffer);                         
                        extern UCHAR *sample_device_cert_ptr;
                        extern UINT sample_device_cert_len;                        
                        sample_device_cert_ptr = (uint8_t *)fileBuffer;
                        sample_device_cert_len = app_pic32mz_w1Data.fileStatus.fsize;                                             
                    }
                    else
                    {                                                
                        SYS_CONSOLE_PRINT("Certificate file read fail!\n");
                        app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
                    }
                    SYS_FS_FileClose(app_pic32mz_w1Data.fileHandle); 
                }
            }
#endif            
            break;
        }
        
        case APP_PIC32MZ_W1_STATE_UNMOUNT_DISK:
        {
            /* Unmount the disk */
            if (SYS_FS_Unmount(APP_MOUNT_NAME) != 0)
            {
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
            }
            else
            {
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_INIT;
            }
            break;
        }
        /* Application's initial state. */
        case APP_PIC32MZ_W1_STATE_INIT:
        {                  
            SYS_STATUS status = WDRV_PIC32MZW_Status(sysObj.drvWifiPIC32MZW1);
            if (SYS_STATUS_READY == status) {
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_INIT_READY;                
                SYS_CONSOLE_MESSAGE("APP_WIFI Ready\r\n");
            }
            else
            {
                //SYS_CONSOLE_PRINT("[state = %d]\r\n", status);
            }
            break;
        }
        
        case APP_PIC32MZ_W1_STATE_INIT_READY:
        {            
            app_pic32mz_w1Data.wdrvHandle = WDRV_PIC32MZW_Open(0, 0);
            if (DRV_HANDLE_INVALID != app_pic32mz_w1Data.wdrvHandle) {
                SYS_CONSOLE_MESSAGE("Check credentials!\r\n");
                app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_CHECK_CREDENTIALS;
            }
            break;
        }

        /* Check on Wi-Fi from USB MSD */
        case APP_PIC32MZ_W1_STATE_CHECK_CREDENTIALS:
        {                       
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_CONFIG;              
            break; 
        }
        
        /* Configure and connect */
        case APP_PIC32MZ_W1_STATE_CONFIG:
        {         
            SYS_CONSOLE_PRINT("Connecting to Wi-Fi (%s)\r\n",wifi.ssid);            
            if (APP_WifiConfig((char*)wifi.ssid, 
                                (char*)wifi.key, 
                                (WIFI_AUTH)wifi.auth, 
                                WDRV_PIC32MZW_CID_ANY)) 
            {                
                if (WDRV_PIC32MZW_STATUS_OK == WDRV_PIC32MZW_BSSConnect(app_pic32mz_w1Data.wdrvHandle, 
                                                                        &g_wifiConfig.bssCtx, 
                                                                        &g_wifiConfig.authCtx, 
                                                                        wifiConnectCallback)) 
                {
                    app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_IDLE;
                    break;
                }
        
            }
            SYS_CONSOLE_MESSAGE("Failed connecting to Wi-Fi\r\n");
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_ERROR;
            break;
        }
     
        
        /* Idle */
        case APP_PIC32MZ_W1_STATE_IDLE:
        {
            break;
        }
        
        /* Reconnect */
        case APP_PIC32MZ_W1_STATE_RECONNECT:
        {            
            WDRV_PIC32MZW_Close(app_pic32mz_w1Data.wdrvHandle);
            app_pic32mz_w1Data.appPic32mzW1State = APP_PIC32MZ_W1_STATE_INIT;
            break;
        }      
        
        /* Error */
        case APP_PIC32MZ_W1_STATE_ERROR:
        {
            break;
        }
        
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
