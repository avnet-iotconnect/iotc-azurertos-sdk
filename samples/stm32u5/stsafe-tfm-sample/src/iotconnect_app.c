//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 1/11/23.
//

#include "app_config.h"

#include "stm32u5xx.h"
#include "b_u585i_iot02a.h"

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect_common.h"
#include "iotconnect.h"
#include "iotc_auth_driver.h"
#include "sw_auth_driver.h"
#include "std_component.h"
#include "metadata.h"
#include "stm32_tfm_psa_auth_driver.h"

extern STD_COMPONENT std_comp; // use the instance in app_azure_iot.c, as that one is linked to the button counter
static char duid_buffer[IOTC_COMMON_NAME_MAX_LEN]; // from ATECC608 common name
static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context = NULL;

static char common_name_buffer[IOTC_COMMON_NAME_MAX_LEN + 1];

#define APP_VERSION "01.00.00"
#define std_component_name "std_comp"


//#define MEMORY_TEST
#ifdef MEMORY_TEST
#define TEST_BLOCK_SIZE  1 * 1024
#define TEST_BLOCK_COUNT 30
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
    for (int j = i-1; j >= 0; j--) {
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
				BSP_LED_On(LED_RED);
			} else {
				BSP_LED_Off(LED_RED);
			}
			command_status(data, true, command, "OK");
		} else if(NULL != strstr(command, "led-green") ) {
			if (NULL != strstr(command, "on")) {
				BSP_LED_On(LED_GREEN);
			} else {
				BSP_LED_Off(LED_GREEN);
			}
			command_status(data, true, command, "OK");
		} else if (NULL != strstr(command, "reset-counters") ) {
			std_comp.ButtonCounter = 0;
			command_status(data, true, command, "OK");
		} else {
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
    	stm32_tfm_psa_release_auth_driver(auth_driver_context);
    	auth_driver_context = NULL;
    }
}

static void publish_telemetry() {
    IotclMessageHandle msg = iotcl_telemetry_create(iotconnect_sdk_get_lib_config());

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);

    UINT status;
    if ((status = std_component_read_sensor_values(&std_comp)) == NX_AZURE_IOT_SUCCESS) {
    	iotcl_telemetry_set_number(msg, "temperature", std_comp.Temperature);
    	iotcl_telemetry_set_number(msg, "humidity", std_comp.Humidity);
    	iotcl_telemetry_set_number(msg, "pressure", std_comp.Pressure);
    	iotcl_telemetry_set_number(msg, "accelerometer.x", std_comp.Acc_X);
    	iotcl_telemetry_set_number(msg, "accelerometer.y", std_comp.Acc_Y);
    	iotcl_telemetry_set_number(msg, "accelerometer.z", std_comp.Acc_Z);
    	iotcl_telemetry_set_number(msg, "magnetometer.x", std_comp.Mag_X);
    	iotcl_telemetry_set_number(msg, "magnetometer.y", std_comp.Mag_Y);
    	iotcl_telemetry_set_number(msg, "magnetometer.z", std_comp.Mag_Z);
    	iotcl_telemetry_set_number(msg, "gyroscope.x", std_comp.Gyro_X);
    	iotcl_telemetry_set_number(msg, "gyroscope.y", std_comp.Gyro_Y);
    	iotcl_telemetry_set_number(msg, "gyroscope.z", std_comp.Gyro_Z);

    	// note the hook into app_azure_iot.c for button interrupt handler
    	iotcl_telemetry_set_number(msg, "button_counter", std_comp.ButtonCounter);

    } else {
    	printf("Failed to read sensor values, error: %u\r\n", status);
    }

    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    printf("Sending: %s\r\n", str);
    iotconnect_sdk_send_packet(str); // underlying code will report an error
    iotcl_destroy_serialized(str);
}

bool extract_cpid_and_duid_from_operational_cn(IotConnectClientConfig *config, char * operational_cn) {
	if (!operational_cn) {
		printf("Unable to extract the operational certificate common name.\r\n");
		return false;
	}
	strcpy(common_name_buffer, operational_cn);
	bool found_dash = false;
	for (int i = -0; i < strlen(common_name_buffer); i++) {
		if (common_name_buffer[i] == '-') {
			config->cpid = common_name_buffer;
			config->duid = &common_name_buffer[i+1];
			common_name_buffer[i] = 0;
			found_dash = true;
			break;
		}
	}
	if (!found_dash || 0 == strlen(config->cpid) || 0 == strlen(config->duid)) {
		printf("Unable to extract CPID and DUID from common name.\r\n");
		return false;
	}
	return true;
}


/* User push button callback*/
void app_on_user_button_pushed(void) {
    std_component_on_button_pushed(&std_comp);
}


/* Include the sample.  */
bool app_startup(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr) {

    printf("Starting App Version %s\r\n", APP_VERSION);

    metadata_storage* md = metadata_get_values();

    IotConnectClientConfig *config = iotconnect_sdk_init_and_get_config();
    azrtos_config.ip_ptr = ip_ptr;
	azrtos_config.pool_ptr = pool_ptr;
	azrtos_config.dns_ptr = dns_ptr;

    config->cpid = md->cpid;
    config->env = md->env;
    config->duid = md->duid; // custom device ID

    if (!md->cpid || !md->env || strlen(md->cpid) == 0 || strlen(md->env) == 0) {
    	printf("ERROR: CPID and Environment must be set in settings\r\n");
    }
    
	UINT status;
    if ((status = std_component_init(&std_comp, (UCHAR *)std_component_name,  sizeof(std_component_name) - 1))) {
        printf("Failed to initialize %s: error code = 0x%08x\r\n", std_component_name, status);
    }

    config->cmd_cb = on_command;
    config->ota_cb = on_ota;
    config->status_cb = on_connection_status;

    if(md->symmetric_key && strlen(md->symmetric_key) > 0) {
		config->auth.type = IOTC_KEY;
		config->auth.data.symmetric_key = md->symmetric_key;
    } else {
    	config->auth.type = IOTC_X509;

        auth_driver_context = NULL;
        struct stm32_tfm_psa_driver_parameters parameters = {0}; // dummy, for now
        IotcDdimInterface ddim_interface;
        IotcAuthInterfaceContext auth_context;
        if(stm32_tfm_psa_create_auth_driver( //
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
            stm32_tfm_psa_release_auth_driver(auth_context);
            return false;
        }

        char* operational_cn = ddim_interface.extract_operational_cn(auth_context);
        if (NULL == operational_cn) {
            stm32_tfm_psa_release_auth_driver(auth_context);
            printf("Unable to get the certificate common name.\r\n");
            return false;
        }
        if (!md->duid || strlen(md->duid) == 0) {
            printf("Using certificate CN as DUID.\r\n");
            strcpy(duid_buffer, operational_cn);
            config->duid = duid_buffer;
        } else {
            printf("Obtained certificate with CN: %s.\r\n", operational_cn);
        }
        printf("DUID: %s\r\n", config->duid);

        auth_driver_context = auth_context;
    }

    while (true) {
#ifdef MEMORY_TEST
        // check for leaks
        memory_test();
#endif //MEMORY_TEST
        if (iotconnect_sdk_init(&azrtos_config)) {
            printf("Unable to establish the IoTConnect connection.\r\n");
            return false;
        }
        // send telemetry periodically
        for (int i = 0; i < 36000; i++) { // if 1 msg per second = 10 hours
            if (iotconnect_sdk_is_connected()) {
                publish_telemetry();  // underlying code will report an error
                iotconnect_sdk_poll(1000);
            } else {
                return false;
            }
        }
        iotconnect_sdk_disconnect();
#ifdef MEMORY_TEST
        // check for leaks
        memory_test();
#endif //MEMORY_TEST
    }
    printf("Done.\r\n");
    return true;
}