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

extern ALTITUDE2_RETVAL ALTITUDE2_status;
static ALTITUDE2_Data altitude2;

extern PHT_RETVAL PHT_status;
static PHT_Data pht;

extern temphum14_return_value_t TEMPHUM14_status;
static uint32_t HTU31_serialNumber;

extern vavpress_sensor_param_data_t VAVPRESS_param_data;
extern vavpress_return_value_t VAVPRESS_status;
extern ultralowpress_return_value_t ULTRALOWPRESS_status;
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

static void publish_telemetry() {
    IotclMessageHandle msg = iotcl_telemetry_create(iotconnect_sdk_get_lib_config());

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
   
    
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
    // random number 0-100, cast to int so that it removes decimals in json
    iotcl_telemetry_set_number(msg, "random", (int)((double)rand() / (double)RAND_MAX * 100.0));
    
    iotcl_telemetry_set_number(msg, "WFI32IoT_temperature", APP_SENSORS_readTemperature());
    iotcl_telemetry_set_number(msg, "WFI32IoT_light", APP_SENSORS_readLight());

    iotcl_telemetry_set_number(msg, "WFI32IoT_button1", button_press_data.flag.sw1);
    iotcl_telemetry_set_number(msg, "WFI32IoT_button2", button_press_data.flag.sw2);
    iotcl_telemetry_set_number(msg, "WFI32IoT_button1_count", button_press_data.sw1_press_count);
    iotcl_telemetry_set_number(msg, "WFI32IoT_button2_count", button_press_data.sw2_press_count);
    button_press_data.flag.sw1 = 0;
    button_press_data.flag.sw2 = 0;
    
    if (ULTRALOWPRESS_status == ULTRALOWPRESS_OK && ULTRALOWPRESS_isReady()) {
        ULTRALOWPRESS_clearStatus();
        iotcl_telemetry_set_number(msg, "ULP_temperature", ULTRALOWPRESS_getTemperature());
        iotcl_telemetry_set_number(msg, "ULP_pressure", ULTRALOWPRESS_getPressure());            
    }

    if (VAVPRESS_status == VAVPRESS_OK) {
        float temperature, pressure;
        if (VAVPRESS_getSensorReadings(&VAVPRESS_param_data, &pressure, &temperature) == VAVPRESS_OK) {
            iotcl_telemetry_set_number(msg, "VAV_temperature", temperature);
            iotcl_telemetry_set_number(msg, "VAV_pressure", pressure);
        }
    }

    if (ALTITUDE2_status == ALTITUDE2_OK) {
        float ALT2_temperature, ALT2_pressure, ALT2_altitude;
        ALTITUDE2_readData(&altitude2, &ALT2_temperature, &ALT2_pressure, &ALT2_altitude);
        iotcl_telemetry_set_number(msg, "ALT2_temperature", ALT2_temperature);
        iotcl_telemetry_set_number(msg, "ALT2_pressure", ALT2_pressure);
        iotcl_telemetry_set_number(msg, "ALT2_altitude", ALT2_altitude);
    }

    if (PHT_status == PHT_OK) {
        float PHT_temperature, PHT_pressure, PHT_humidity;
        PHT_getTemperaturePressure(&pht, &PHT_temperature, &PHT_pressure);
        PHT_getRelativeHumidity(&PHT_humidity);
        iotcl_telemetry_set_number(msg, "PHT_temperature", PHT_temperature);
        iotcl_telemetry_set_number(msg, "PHT_pressure", PHT_pressure);
        iotcl_telemetry_set_number(msg, "PHT_humidity", PHT_humidity);
    }

    if (HTU31_serialNumber != 0) {
        float TEMPHUM14_temperature, TEMPHUM14_humidity;
        TEMPHUM14_setConversion( TEMPHUM14_I2C_SLAVE_ADDR_GND, TEMPHUM14_CONVERSION_HUM_OSR_0_020, TEMPHUM14_CONVERSION_TEMP_0_040 );
        TEMPHUM14_getTemperatureHumidity (TEMPHUM14_I2C_SLAVE_ADDR_GND, &TEMPHUM14_temperature, &TEMPHUM14_humidity );
        iotcl_telemetry_set_number(msg, "TEMPHUM14_temperature", TEMPHUM14_temperature);
        iotcl_telemetry_set_number(msg, "TEMPHUM14_humidity", TEMPHUM14_humidity);

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

    printf("Detecting attached Click Board sensors...\r\n");

    VAVPRESS_init();
    if (VAVPRESS_status == VAVPRESS_OK) {
        printf("VAV Click detected\r\n");
    } else {
        printf("VAV Click not detected\r\n");
    }

    uint32_t SM8436_serialNumber = ULTRALOWPRESS_init();
    if (ULTRALOWPRESS_status == ULTRALOWPRESS_OK) {
        printf("ULP Click detected. Serial Number = %u\r\n", SM8436_serialNumber);
    } else {
        printf("ULP Click not detected\r\n");
    }

    ALTITUDE2_status = ALTITUDE2_init(&altitude2);
    if (ALTITUDE2_status == ALTITUDE2_OK) {
        printf("ALT2 Click detected\r\n");
    } else {
        printf("ALT2 Click not detected\r\n");
    }

    PHT_status = PHT_init(&pht);
    if (PHT_status == PHT_OK) {
        printf("PHT Click detected\r\n");
    } else {
        printf("PHT Click not detected\r\n");
    }

    HTU31_serialNumber = TEMPHUM14_init(TEMPHUM14_I2C_SLAVE_ADDR_GND);
    if (HTU31_serialNumber != 0) {
        printf("TEMPHUM14 Click detected. Serial Number = %u\r\n", HTU31_serialNumber);
    } else {
        printf("TEMPHUM14 Click not detected\r\n");
    }

    
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
        // send telemetry at regular intervals
        for (int i = 0; i < 36000; i++) { // 10 hours if 1s delay
            if (iotconnect_sdk_is_connected()) {
                publish_telemetry();  // underlying code will report an error
                iotconnect_sdk_poll(1000);
            } else {
                return false;
            }
        }
        iotconnect_sdk_disconnect();
    }
    printf("Done.\r\n");
    return true;
}
