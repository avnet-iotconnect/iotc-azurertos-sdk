//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#include <stdio.h>

#include "app_config.h"

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect_common.h"
#include "iotconnect.h"
#include "iotc_auth_driver.h"
#include "sw_auth_driver.h"
#include "app.h" // original Microchip app include for sensors

#ifdef ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE
#include "pkcs11_atca_auth_driver.h"
#include "atcacert/atcacert_pem.h"
#endif

extern UCHAR _nx_driver_hardware_address[];


// Various Click Board and onboard sensors/buttons supported by the demo
extern button_press_data_t button_press_data;
// ///////////////////


static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context = NULL;

#ifdef ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE
static char duid_buffer[IOTC_COMMON_NAME_MAX_LEN]; // from ATECC608 common name
#endif
#define APP_VERSION "01.00.00"

//#define MEMORY_TEST
#ifdef MEMORY_TEST
#define TEST_BLOCK_SIZE  10 * 1024
#define TEST_BLOCK_COUNT 100
static void *blocks[TEST_BLOCK_COUNT];
void memory_test() {
    int i = 0;
    for (; i < TEST_BLOCK_COUNT; i++) {
        void *ptr = malloc(TEST_BLOCK_SIZE);
        if (!ptr) {
            break;
        }
        blocks[i] = ptr;
    }
    printf("====Allocated %d blocks of size %d (of max %d)===\r\n", i, TEST_BLOCK_SIZE, TEST_BLOCK_COUNT);
    for (int j = 0; j < i; j++) {
        free(blocks[j]);
    }
}
#endif /* MEMORY_TEST */

static bool is_app_version_same_as_ota(const char *version) {
    return strcmp(APP_VERSION, version) == 0;
}

static bool app_needs_ota_update(const char *version) {
    return strcmp(APP_VERSION, version) < 0;
}

static void on_ota(IotclEventData data) {
    const char *message = NULL;
    char *url = iotcl_clone_download_url(data, 0);
    bool success = false;
    if (NULL != url) {
        printf("Download URL is: %s\r\n", url);
        const char *version = iotcl_clone_sw_version(data);
        if (is_app_version_same_as_ota(version)) {
            printf("OTA request for same version %s. Sending success\r\n", version);
            success = true;
            message = "Version is matching";
        } else if (app_needs_ota_update(version)) {
            printf("OTA update is required for version %s.\r\n", version);
            success = false;
            message = "Not implemented";
        } else {
            printf("Device firmware version %s is newer than OTA version %s. Sending failure\r\n", APP_VERSION,
                    version);
            // Not sure what to do here. The app version is better than OTA version.
            // Probably a development version, so return failure?
            // The user should decide here.
            success = false;
            message = "Device firmware version is newer";
        }

        free((void*) url);
        free((void*) version);
    } else {
        // compatibility with older events
        // This app does not support FOTA with older back ends, but the user can add the functionality
        const char *command = iotcl_clone_command(data);
        if (NULL != command) {
            // URL will be inside the command
            printf("Command is: %s\r\n", command);
            message = "Old back end URLS are not supported by the app";
            free((void*) command);
        }
    }
    const char *ack = iotcl_create_ack_string_and_destroy_event(data, success, message);
    if (NULL != ack) {
        printf("Sent OTA ack: %s\r\n", ack);
        iotconnect_sdk_send_packet(ack);
        free((void*) ack);
    }
}

static void command_status(IotclEventData data, bool status, const char *command_name, const char *message) {
    const char *ack = iotcl_create_ack_string_and_destroy_event(data, status, message);
    printf("command: %s status=%s: %s\r\n", command_name, status ? "OK" : "Failed", message);
    printf("Sent CMD ack: %s\r\n", ack);
    iotconnect_sdk_send_packet(ack);
    free((void*) ack);
}

static void on_command(IotclEventData data) {
    char *command = iotcl_clone_command(data);
    if (NULL != command) {
        if(NULL != strstr(command, "led-red") ) {
            if (NULL != strstr(command, "on")) {
                LED_RED_On();
            } else {
                LED_RED_Off();
            }
            command_status(data, true, command, "OK");
        } else if (NULL != strstr(command, "led-green") ) {
            if (NULL != strstr(command, "on")) {
                LED_GREEN_On();
            } else {
                LED_GREEN_Off();
            }
            command_status(data, true, command, "OK");
        } else if (NULL !=  strstr(command, "led-blue") ) {
            if (NULL != strstr(command, "on")){
                LED_BLUE_On();
            } else {
                LED_BLUE_Off();
            }
            command_status(data, true, command, "OK");
        } else if (NULL !=  strstr(command, "reset-counters") ) {
            button_press_data.sw1_press_count = 0;
            button_press_data.sw2_press_count = 0;
            command_status(data, true, command, "OK");
        } else {
            tx_thread_sleep(100); // sleep because of the UART flood issue with this board
            printf("Unknown command:%s\r\n", command);
            tx_thread_sleep(100);
            command_status(data, false, command, "Not implemented");
        }
        free((void*) command);
    } else {
        command_status(data, false, "?", "Internal error");
    }
}

static void on_connection_status(IotConnectConnectionStatus status) {
    // Add your own status handling
    switch (status) {
    case MQTT_CONNECTED:
        printf("IoTConnect Client Connected\r\n");
        break;
    case MQTT_DISCONNECTED:
        printf("IoTConnect Client Disconnected\r\n");
        break;
    default:
        printf("IoTConnect Client ERROR\r\n");
        break;
    }
    if (NULL != auth_driver_context) {
#ifdef ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE
    	pkcs11_atca_release_auth_driver(auth_driver_context);
#else        
    	release_sw_der_auth_driver(auth_driver_context);
#endif
        auth_driver_context = NULL;
    }
}

static void publish_telemetry(int warming_up, int connections[]) {
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
   
    
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
    
    iotcl_telemetry_set_number(msg, "Onboard_Temp_DegC", APP_SENSORS_readTemperature());
    iotcl_telemetry_set_number(msg, "Onboard_Light_Lux", APP_SENSORS_readLight());

    iotcl_telemetry_set_number(msg, "WFI32IoT_button1", button_press_data.flag.sw1);
    iotcl_telemetry_set_number(msg, "WFI32IoT_button2", button_press_data.flag.sw2);
    iotcl_telemetry_set_number(msg, "WFI32IoT_button1_count", button_press_data.sw1_press_count);
    iotcl_telemetry_set_number(msg, "WFI32IoT_button2_count", button_press_data.sw2_press_count);
    button_press_data.flag.sw1 = 0;
    button_press_data.flag.sw2 = 0;
    
    //Air Quality 7 Click Reading
    if(connections[0] == 1)
    {
        //Array entries are  [tVOC, CO2, Resistor Value, Status]
        float air_data[] = {0, 0, 0, 0, 0, 0};
        AIRQUALITY7_readData(air_data);
        //If the sensor has been warming up for 15 minutes
        if(warming_up == 181)
        {
            //Report status and data from readData function output
            if(air_data[3] == 1)
            {
               iotcl_telemetry_set_string(msg, "AIR7_Status", "ERROR"); 
            }
            else
            {
               iotcl_telemetry_set_string(msg, "AIR7_Status", "READY");
            }
            iotcl_telemetry_set_number(msg, "AIR7_tVOC_ppb", air_data[0]);   
            iotcl_telemetry_set_number(msg, "AIR7_CO2_ppm", air_data[1]);
        }
        //If the sensor is still warming up
        else
        {
            //Report status and zeros for data
            iotcl_telemetry_set_string(msg, "AIR7_Status", "WARMING_UP");
            iotcl_telemetry_set_number(msg, "AIR7_tVOC_ppb", 0);   
            iotcl_telemetry_set_number(msg, "AIR7_CO2_ppm", 0);
        }
    }
    
    
    //Altitude 2 Click Reading
    if(connections[1] == 1)
    {
        //Array entries are  [Pressure, Temperature]
        float alt2_data[] = {0, 0};
        float alt2_cal_vals[] = {0, 0, 0, 0, 0, 0};
        ALTITUDE2_cal_vals(alt2_cal_vals);
        ALTITUDE2_readData(alt2_data, alt2_cal_vals);
        //Report data pulled from readData function output
        iotcl_telemetry_set_number(msg, "ALT2_Temp_DegC", alt2_data[1]);
        iotcl_telemetry_set_number(msg, "ALT2_Pressure_mBar", alt2_data[0]);  
    } 
    
    
    //Altitude 4 Click Reading
    if(connections[2] == 1)
    {
        //Array entries are  [Pressure, Altitude, Temperature]
        float alt4_data[] = {0, 0, 0};
        ALTITUDE4_readData(alt4_data);
        //Report data pulled from readData function output
        iotcl_telemetry_set_number(msg, "ALT4_Temp_DegC", alt4_data[2]);
        iotcl_telemetry_set_number(msg, "ALT4_Pressure_mBar", alt4_data[0]);   
        iotcl_telemetry_set_number(msg, "ALT4_Altitude_m", alt4_data[1]);
    }
    
    //PHT Click Reading
    if(connections[3] == 1)
    {
        //Array entries are  [Pressure, Temperature, Relative Humidity]
        float pht_data[] = {0, 0, 0};
        float pht_cal_vals[] = {0, 0, 0, 0, 0, 0};
        PHT_cal_vals(pht_cal_vals);
        PHT_readData(pht_data, pht_cal_vals);
        //Report data pulled from readData function output
        iotcl_telemetry_set_number(msg, "PHT_Temp_DegC", pht_data[1]);
        iotcl_telemetry_set_number(msg, "PHT_Pressure_mBar", pht_data[0]);  
        iotcl_telemetry_set_number(msg, "PHT_Humidity_Percent", pht_data[2]);
    } 
    
    //T6713-6H Proto Click Reading
    if(connections[4] == 1)
    {
        //Array entries are  [CO2]
        int CO2_data[] = {0};
        T6713_readData(CO2_data);
        //If the sensor has been warming up for 10 minutes
        if(warming_up > 120)
        {
            //Report status and data from readData function output
            iotcl_telemetry_set_string(msg, "T6713_Status", "READY");
            iotcl_telemetry_set_number(msg, "T6713_CO2_ppm", CO2_data[0]);
        }
        //If the sensor is still warming up
        else
        {
            //Report status and zeros for data
            iotcl_telemetry_set_string(msg, "T6713_Status", "WARMING_UP");
            iotcl_telemetry_set_number(msg, "T6713_CO2_ppm", 0);
        }
    }

    
    //T9602 Terminal Click Reading
    if(connections[5] == 1)
    {
        //Array entries are  [Relative Humidity, Temperature]
        float hum_data[] = {0, 0};
        T9602_readData(hum_data);
        //If the sensor has been warming up for 2 minutes
        if(warming_up > 24)
        {
            //Report status and data from readData function output
            iotcl_telemetry_set_string(msg, "T9602_Status", "READY");
            iotcl_telemetry_set_number(msg, "T9602_Humidity_Percent", hum_data[0]);
            iotcl_telemetry_set_number(msg, "T9602_Temp_DegC", hum_data[1]);
        }
        //If the sensor is still warming up
        else
        {
            //Report status and zeros for data
            iotcl_telemetry_set_string(msg, "T9602_Status", "WARMING_UP");
            iotcl_telemetry_set_number(msg, "T9602_Humidity_Percent", 0);
            iotcl_telemetry_set_number(msg, "T9602_Temp_DegC", 0);
        }
    }
    
    
    //TempHum14 Click Reading
    if(connections[6] == 1)
    {
        //Array entries are  [Relative Humidity, Temperature]
        float TH_data[] = {0, 0};
        TEMPHUM14_setConversion(0x40, TEMPHUM14_CONVERSION_HUM_OSR_0_020, TEMPHUM14_CONVERSION_TEMP_0_040 );
        TEMPHUM14_getTemperatureHumidity (0x40, TH_data);

        //Report data from readData function output
        iotcl_telemetry_set_number(msg, "TH14_Humidity_Percent", TH_data[1]);
        iotcl_telemetry_set_number(msg, "TH14_Temp_DegC", TH_data[0]);
    }
    
    //ULP Click Reading
    if(connections[7] == 1)
    {
        //Array entries are  [Temperature, Pressure]
        float ULP_data[] = {0, 0};
        if (ULTRALOWPRESS_isReady()) 
        {
            ULTRALOWPRESS_getData(ULP_data);
            //Report data from getData function output
            iotcl_telemetry_set_number(msg, "ULP_Temp_DegC", ULP_data[0]);
            iotcl_telemetry_set_number(msg, "ULP_Pressure_Pa", ULP_data[1]);
        }
    }
    
    //VAVPress Click Reading
    if(connections[8] == 1)
    {
        //Array entries are  [Pressure, Temperature]
        float VAV_data[] = {0, 0};
        VAVPRESS_getSensorReadings(VAV_data);
        //Report data from getSensorReadings function output
        iotcl_telemetry_set_number(msg, "VAV_Pressure_Pa", VAV_data[0]);
        iotcl_telemetry_set_number(msg, "VAV_Temp_DegC", VAV_data[1]);
    }


    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    
    // The string would be too long due to issues with UART.
    // No point in printing it, as it would scramble the output.
    printf("Sending message length %u\r\n", strlen(str));
    iotconnect_sdk_send_packet(str); // underlying code will report an error
    iotcl_destroy_serialized(str);
}

/* Include the sample.  */
bool iotconnect_sample_app(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr) {
    printf("Starting App Version %s\r\n", APP_VERSION);
    
    APP_SWITCH_init();

    APP_SENSORS_init();
    
    //Time in-between data reportings (ms)
    int loop_time = 5000;
    
    //Keeps track of how long the main loop has been running 
    //for the sensors that have warm-up times
    int warming_up = 0;
    
    //Will hold cumulative delay time for all attached sensors
    int delay_total = 0;

    printf("Detecting attached Click Board sensors...\r\n");

    //Array for telling whether or not each click board is present (1 or 0)
    //Structure is {AQ7, ALT2, ALT4, PHT, T6713, T9602, TH14, ULP, VAV}
    int connections[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    //AQ7
    float aq7_data[] = {0, 0, 0, 0};
    AIRQUALITY7_readData(aq7_data);
    //If calibration values is non-zero, it is connected
    if(0 != aq7_data[2])
    {
        printf("Air Quality 7 Click Detected!\r\n");
        connections[0] = 1;
        delay_total += 100;
    }
    
    //ALT2
    float alt2_cal[] = {0, 0, 0, 0, 0, 0};
    ALTITUDE2_cal_vals(alt2_cal);
    //If a calibration values is non-zero, it is connected
    if(0 != alt2_cal[0])
    {
        printf("Altitude 2 Click Detected!\r\n");
        connections[1] = 1;
        delay_total += 90;
    }
    
    //ALT4
    float alt4_vals[] = {0, 0, 0};
    ALTITUDE4_readData(alt4_vals);
    //If a pressure value is not 260 or a temp value is not -40, it is connected
    //(+260 and -40 correspond to zero-readings on ALT4)
    if(260 != alt4_vals[0] || -40 != alt4_vals[2])
    {
        printf("Altitude 4 Click Detected!\r\n");
        connections[2] = 1;
        delay_total += 20;
    }
    
    //PHT
    float pht_cal[] = {0, 0, 0, 0, 0, 0};
    PHT_cal_vals(pht_cal);
    //If a calibration values is non-zero, it is connected
    if(0 != pht_cal[0])
    {
        printf("PHT Click Detected!\r\n");
        connections[3] = 1;
        delay_total += 120;
    }
    
    //T6713
    int CO2_val[] = {0};
    T6713_readData(CO2_val);
    //If a CO2 measurement is non-zero, it is connected
    if(0 != CO2_val[0])
    {
        printf("T6713 CO2 Sensor Detected!\r\n");
        connections[4] = 1;
        delay_total += 100;
    }
    
    //T9602
    float hum_vals[] = {0, 0};
    //If humidity or temperature measurements are non-zero, it is connected
    //(-40 corresponds to a zero reading for temperature)
    T9602_readData(hum_vals);
    if(0 != hum_vals[0] || -40 != hum_vals[1])
    {
        printf("T9602 TempHum Sensor Detected!\r\n");
        connections[5] = 1;
        delay_total += 100;
    }
    
    //TH14
    uint32_t TH14sn = 0;
    //If a humidity measurement is non-zero, it is connected
    TH14sn = TEMPHUM14_init(0x40);
    if(0 != TH14sn)
    {
        printf("TempHum14 Click Detected!\r\n");
        connections[6] = 1;
        delay_total += 10;
    }
    
    //ULP
    uint32_t ULPsn = 0;
    //If serial number is non-zero, it is connected
    ULPsn = ULTRALOWPRESS_init();
    if(0 != ULPsn)
    {
        printf("Ultra Low Press Click Detected!\r\n");
        connections[7] = 1;
        delay_total += 10;
    }
    
    //VAV
    uint16_t VAV_calID = 0;
    //If calibration ID is non-zero, it is connected
    VAV_calID = VAVPRESS_init();
    if(0 != VAV_calID)
    {
        printf("VAVPress Click Detected!\r\n");
        connections[8] = 1;
    }
    
    //The time to wait after each cycle of the main loop is loop time minus
    //500ms, minus the total delays of the attached sensors
    int loop_delay = (loop_time - 500) - delay_total;

    
    IotConnectClientConfig *config = iotconnect_sdk_init_and_get_config();
    azrtos_config.ip_ptr = ip_ptr;
	azrtos_config.pool_ptr = pool_ptr;
	azrtos_config.dns_ptr = dns_ptr;

    IotConnectAppConfig *app_cfg = get_app_config();
    config->cpid = app_cfg->cpid;
    config->env = app_cfg->env;
    config->duid = app_cfg->duid;

    char* symmetric_key = app_cfg->symmetric_key;
    if (NULL != symmetric_key && strlen(symmetric_key) != 0) {
        config->auth.type = IOTC_KEY;
        config->auth.data.symmetric_key = symmetric_key;
    } else {
        config->auth.type = IOTC_X509;
    }
    
    config->cmd_cb = on_command;
    config->ota_cb = on_ota;
    config->status_cb = on_connection_status;


#ifdef ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE
    // we will duplicate this if guard check, so #ifdef is easier to read
    if (config->auth.type == IOTC_X509) { 
        auth_driver_context = NULL;
        struct pkcs11_atca_driver_parameters parameters = {0}; // dummy, for now
        IotcDdimInterface ddim_interface;
        IotcAuthInterfaceContext auth_context;
        if(pkcs11_atca_create_auth_driver( //
                &(config->auth.data.x509.auth_interface), //
                &ddim_interface, //
                &auth_context, //
                &parameters)) { //
            return false;
        }
        config->auth.data.x509.auth_interface_context = auth_context;

        uint8_t* cert;
        size_t cert_size;

        config->auth.data.x509.auth_interface.get_cert(
                auth_context, //
                &cert, //
                &cert_size //
                );
        if (0 == cert_size) {
            printf("Unable to get the certificate from the driver.\r\n");
            pkcs11_atca_release_auth_driver(auth_context);
            return false;
        }
        char* b64_string_buffer = malloc(1024); // cert should be around 500, so this should not exceed 1024
        if (NULL == b64_string_buffer) {
            pkcs11_atca_release_auth_driver(auth_context);
            printf("WARNING: Unable to allocate memory to display the certificate.\r\n");
        } else {
            size_t b64_string_len = 1024; // in/out parameter. See atcacert_encode_pem_cert()
            atcacert_encode_pem_cert(cert, cert_size, b64_string_buffer, &b64_string_len);
            b64_string_buffer[b64_string_len] = 0;
            printf("Device certificate ( %u bytes):\r\n%s\r\n", cert_size, b64_string_buffer);
            free(b64_string_buffer);
        }

        printf("Obtained device certificate:\r\n\r\n");
        char* operational_cn = ddim_interface.extract_operational_cn(auth_context);
        if (NULL == operational_cn) {
            pkcs11_atca_release_auth_driver(auth_context);
            printf("Unable to get the certificate common name.\r\n");
            return false;
        }
        strcpy(duid_buffer, operational_cn);
        config->duid = duid_buffer;
        printf("DUID: %s\r\n", config->duid);

        auth_driver_context = auth_context;
    }
#else // not ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE
    // we will duplicate this if guard check, so #ifdef is easier to read
    if (config->auth.type == IOTC_X509) { 
        extern const UCHAR sample_device_cert_ptr[];
        extern const UINT sample_device_cert_len;
        extern const UCHAR sample_device_private_key_ptr[];
        extern const UINT sample_device_private_key_len;
        struct sw_der_driver_parameters dp = {0};
        dp.cert = (uint8_t *) (sample_device_cert_ptr);
        dp.cert_size = sample_device_cert_len;
        dp.key = (uint8_t *) (sample_device_private_key_ptr);
        dp.key_size = sample_device_private_key_len;
        dp.crypto_method = &crypto_method_ec_secp256;
        if(create_sw_der_auth_driver( //
                    &(config->auth.data.x509.auth_interface), //
                    &(config->auth.data.x509.auth_interface_context), //
                    &dp)) { //
                return false;
            }
        auth_driver_context = config->auth.data.x509.auth_interface_context;
    }
#endif // ENABLE_DDIM_PKCS11_ATCA_DRIVER_SAMPLE

    while (true) {
#ifdef MEMORY_TEST
        // check for leaks
        memory_test();
#endif //MEMORY_TEST
        if (iotconnect_sdk_init(&azrtos_config)) {
            printf("Unable to establish the IoTConnect connection.\r\n");
            return false;
        }
        //MAIN LOOP
        while (1) 
        {
            if (iotconnect_sdk_is_connected()) 
            {
                publish_telemetry(warming_up, connections);
                //Yellow LED Blinks for 500ms when telemetry is being sent
                LED_YELLOW_On();
                iotconnect_sdk_poll(500);
                LED_YELLOW_Off();
                iotconnect_sdk_poll(loop_delay);
                //Maximum warm-up time is 15 minutes
                if(warming_up < 181)
                {
                    warming_up++;
                }
            } 
            else 
            {
                return false;
            }
        }
        iotconnect_sdk_disconnect();
    }
    printf("Done.\r\n");
    return true;
}