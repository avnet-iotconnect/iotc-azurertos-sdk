* Add include files recursively with 
* _Min_Heap_Size = 0x2000 (from 200) in STM32U585AIIX_RAM.ld
* Add application PROJECT_LOC/../../../../../../basic-sample
* Add sdk with PROJECT_LOC/../../../../../../iotc-azrtos-sdk
* Exclude unneeded files from the project build
  * cJSON tests
  * iotc-c-lib tests
  * to_auth_driver.c
* Add all include paths from the application and SDK
* Remove sntp_time_sync call from App_Azure_IoT_Thread_Entry in app_netxduo.c
* Add NX_WEB_HTTPS_ENABLE to the preprocessor defines
* checkin .cproject and .project
* app_netxduo.h
  * Increase THREAD_MEMORY_SIZE from 4 to 8
  * Reduce NX_PACKET_POOL_SIZE from 30 packets to 20
* app_netxduo.c:
  * Add #include "azrtos_time.h", #include "app_config.h"
  * Remove declarations for time-related functions
  * Call our sync: ret = sntp_time_sync(&IpInstance, &AppPool, &DnsClient, SAMPLE_SNTP_SERVER_NAME);
  * Call our app: app_startup(&IpInstance, &AppPool, &DnsClient);
  
