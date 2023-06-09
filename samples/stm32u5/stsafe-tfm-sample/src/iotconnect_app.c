//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 1/11/23.
//

#include "iotconnect_app_config.h"

#include "stm32u5xx.h"
#include "b_u585i_iot02a.h"

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect_common.h"
#include "iotconnect.h"
#include "azrtos_ota_fw_client.h"
#include "azrtos_adu_agent.h"
#include "iotc_auth_driver.h"
#include "sw_auth_driver.h"
#include "std_component.h"
#include "metadata.h"
#include "stm32_tfm_psa_auth_driver.h"

extern void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr);

extern STD_COMPONENT std_comp; // use the instance in app_azure_iot.c, as that one is linked to the button counter
static char duid_buffer[IOTC_COMMON_NAME_MAX_LEN]; // from ATECC608 common name
static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context = NULL;

static char common_name_buffer[IOTC_COMMON_NAME_MAX_LEN + 1];
#define APP_VERSION "1.0.0"
#define std_component_name "std_comp"


//#define MEMORY_TEST
#ifdef MEMORY_TEST
#define TEST_BLOCK_SIZE  1 * 1024
#define TEST_BLOCK_COUNT 30
static void *blocks[TEST_BLOCK_COUNT];
void memory_test(void) {
    int i = 0;
    for (; i < TEST_BLOCK_COUNT; i++) {
        void *ptr = malloc(TEST_BLOCK_SIZE);
        // printf("0x%lx\r\n", (unsigned long) ptr);
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

static bool download_event_handler(IotConnectDownloadEvent* event) {
    switch (event->type) {
    case IOTC_DL_STATUS:
        if (event->status == NX_SUCCESS) {
            printf("Download success\r\n");
        } else {
            printf("Download failed with code 0x%x\r\n", event->status);
        }
        break;
    case IOTC_DL_FILE_SIZE:
        printf("Download file size is %i\r\n", event->file_size);
        break;
    case IOTC_DL_DATA:
        printf("%i%%\r\n", (event->data.offset + event->data.data_size) * 100 / event->data.file_size);
        break;
    default:
        printf("Unknown event type %d received from download client!\r\n", event->type);
        break;
    }
    return true;
}

// Parses the URL into host and resource strings which will be malloced
// Ensure to free the two pointers on success
static UINT split_url(const char *url, char **host_name, char**resource) {
    int host_name_start = 0;
    size_t url_len = strlen(url);

    if (!host_name || !resource) {
        printf("split_url: Invalid usage\r\n");
        return NX_INVALID_PARAMETERS;
    }
    *host_name = NULL;
    *resource = NULL;
    int slash_count = 0;
    for (size_t i = 0; i < url_len; i++) {
        if (url[i] == '/') {
            slash_count++;
            if (slash_count == 2) {
                host_name_start = i + 1;
            } else if (slash_count == 3) {
                const size_t slash_start = i;
                const size_t host_name_len = i - host_name_start;
                const size_t resource_len = url_len - i;
                *host_name = malloc(host_name_len + 1); //+1 for null
                if (NULL == *host_name) {
                    return NX_POOL_ERROR;
                }
                memcpy(*host_name, &url[host_name_start], host_name_len);
                (*host_name)[host_name_len] = 0; // terminate the string

                *resource = malloc(resource_len + 1); //+1 for null
                if (NULL == *resource) {
                    free(*host_name);
                    return NX_POOL_ERROR;
                }
                memcpy(*resource, &url[slash_start], resource_len);
                (*resource)[resource_len] = 0; // terminate the string

                return NX_SUCCESS;
            }
        }
    }
    return NX_INVALID_PARAMETERS; // URL could not be parsed
}

// Parses the URL into host and path strings.
// It re-uses the URL storage by splitting it into two null-terminated strings.
static UINT start_ota(char *url) {
    IotConnectHttpRequest req = { 0 };

    UINT status = split_url(url, &req.host_name, &req.resource);
    if (status) {
        printf("start_ota: Error while splitting the URL, code: 0x%x\r\n", status);
        return status;
    }

    req.azrtos_config = &azrtos_config;
    // URLs should come in with blob.core.windows.net and similar so Digicert cert should work for all
    req.tls_cert = (unsigned char*) IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2;
    req.tls_cert_len = IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2_SIZE;

    status = iotc_ota_fw_download(
            &req,
			nx_azure_iot_adu_agent_driver,
            false,
            download_event_handler);
    if (status) {
        printf("OTA Failed with code 0x%x\r\n", status);
    } else {
        printf("OTA Download Success\r\n");
    }
    free(req.host_name);
    free(req.resource);
    return status;
}

static bool is_app_version_same_as_ota(const char *version) {
    return strcmp(APP_VERSION, version) == 0;
}

static bool app_needs_ota_update(const char *version) {
    return strcmp(APP_VERSION, version) < 0;
}

static void on_ota(IotclEventData data) {
    const char *message = NULL;
    bool needs_ota_commit = false;
    char *url = iotcl_clone_download_url(data, 0);
    bool success = false;
    if (NULL != url) {
        printf("Download URL is: %s\r\n", url);
        const char *version = iotcl_clone_sw_version(data);
        if (!version) {
            printf("Failed to clone SW version! Out of memory?");
            message = "Failed to clone SW version";
        } else if (is_app_version_same_as_ota(version)) {
            printf("OTA request for same version %s. Sending success\r\n", version);
            success = true;
            message = "Version is matching";
        } else if (app_needs_ota_update(version)) {
            printf("OTA update is required for version %s.\r\n", version);

            if (start_ota(url)) {
                message = "OTA Failed";
            } else {
                success = true;
                needs_ota_commit = true;
                message = NULL;
            }
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
    if (needs_ota_commit) {
        printf("Waiting for ack to be sent by the network\r\n.,,");
        tx_thread_sleep(5 * NX_IP_PERIODIC_RATE);
        UINT status = iotc_ota_fw_apply();
        if (status) {
            printf("Failed to apply firmware! Error was: %d\r\n", status);
        }
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
    IotclMessageHandle msg = iotcl_telemetry_create();

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

        tx_thread_sleep(1000);
        iothub_start_device_agent(
            IOTC_ADU_STM,
            IOTC_ADU_STM32U5,
            NULL, // not used
            NULL, // not used
            APP_VERSION
            );

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
