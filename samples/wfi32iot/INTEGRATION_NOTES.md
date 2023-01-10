### Integration Notes
This file is intended to be for record keeping by the developers working on this project.


### Project Integration Notes
* Comment out most of sample_azure_iot_embedded_sdk.c, except the azureGlue_crypto_hmac_256_calculate function.
* In azureGlue_crypto_hmac_256_calculate, put the TLS buffer on the stack.
* Remove sntp_time_sync and unix_time_get in sample azure iot entry
* replace static declarations with #include "azrtos_time.h", "app_config.h" and add SNTP host to the time_sync call. Comment out //#define RUN_AZURE
* Edit tasks.c and remove references to CMD task
* Remove references to sample_device_private_key* from sample_read_device_cert.c
* OR---- remove code after the globals define
* Call iotconnect_app in sample_azure_iot_entry.:
```C
#include <stdbool.h>
extern bool iotconnect_sample_app(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr);
// line 96:
    /* Start sample.  */
    iotconnect_sample_app(ip_ptr, pool_ptr, dns_ptr);
```
* Reduce size of packet buffer from 40 to 20 (NX_DEMO_PACKET_POOL_SIZE) in nx_user.h
* In tasks.c, declare a separate buffer and don't use first unused memory
* In tasks.c, remove or comment out the CMD task.
* Add a workaround for print buffer overflow and use it in publish_telemetry:
```C
#define PRINT_BUFF_SIZE 128
// workaround for the UART having issues printing long lines
static void print_string(const char* str) {   
    static char buff[PRINT_BUFF_SIZE];
   
    for (int c = 0; c < strlen(str); c+=PRINT_BUFF_SIZE) {
        size_t ccount = strlen(&str[c]) < PRINT_BUFF_SIZE ? strlen(&str[c]) : PRINT_BUFF_SIZE;
        strncpy(buff, &str[c], ccount);
        buff[ccount] = 0;
        printf("%s", buff);
        fflush(stdout);
        tx_thread_sleep(10);
    }
    printf("\r\n");
}
```
### Project File Integration Notes (for reference)
* Enable compiler optimizations:
  * Place data in own section (-fdata-sections) 
* Overwrite sample device identity so that it's a proper array
* Rename the project to "iotconnect-demo"
* "Add existing items from folders" at top level and point o basic-sample and iot-azurertos-sdk
* Exclude the following from project configuration:
  * authentication/to_auth_driver.c
* Exclude the following Azure Demo files from project configuration:
  * nx_azure_iot_ciphersuites.c
* Set heap size to 8192 (maybe?) in gcc-ld settings 
* Define IOTC_NEEDS_C_TIME, IOTC_NEEDS_GETTIMEOFDAY_OU and NX_WEB_HTTPS_ENABLE in gcc preprocessor settings
* add addons/web and exclude other files
* add preprocessor define NX_WEB_HTTPS_ENABLE
* Add the following to include paths:
  * ../basic-sample/include
  * ../iotc-azrtos-sdk/authentication/driver
  * ../../../iotc-azrtos-sdk/authentication/include
  * ../../../iotc-azrtos-sdk/azrtos-layer/include
  * ../../../iotc-azrtos-sdk/include
  * ../src/third_party/azure_rtos/netxduo/addons/web
  * ../iotc-azrtos-sdk/iotc-c-lib/include
  * ../iotc-azrtos-sdk/cJSON
  * ../iotc-azrtos-sdk/libTO/include
  * ../src/azure_rtos_demo/pkcs11_ciphersuites

