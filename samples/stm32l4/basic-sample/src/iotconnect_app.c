//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
#include "app_config.h"

#include "nx_api.h"
#include "wifi.h"
#include "nxd_dns.h"
#include "iotconnect_common.h"
#include "iotconnect.h"
#include "iotconnect_certs.h"
#include "azrtos_ota_fw_client.h"
#include "azrtos_adu_agent.h"
#include "iotc_auth_driver.h"
#include "sw_auth_driver.h"

// make sure this is after app_config.h
#ifdef ENABLE_DDIM_TO_DRIVER_SAMPLE
#include "TO.h" // for TO_HW_SN_SIZE
#include "to_auth_driver.h"
#include "iotconnect_di.h"
#endif


// from nx_azure_iot_adu_agent_<boardname>_driver.c
extern void nx_azure_iot_adu_agent_driver(NX_AZURE_IOT_ADU_AGENT_DRIVER *driver_req_ptr);

static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context;

#define X509_COMMON_NAME_LENGTH 64
static char common_name_buffer[X509_COMMON_NAME_LENGTH + 1];


#define APP_VERSION "1.0.0"

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


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static char* compose_device_id() {
#define PREFIX_LEN (sizeof(DUID_PREFIX) - 1)
    uint8_t mac_addr[6] = { 0 };
    static char duid[PREFIX_LEN + sizeof(mac_addr) * 2 + 1 /* null */] = DUID_PREFIX;
    if (WIFI_GetMAC_Address(mac_addr) == WIFI_STATUS_OK) {
        for (int i = 0; i < sizeof(mac_addr); i++) {
            sprintf(&duid[PREFIX_LEN + i * 2], "%02x", mac_addr[i]);
        }
    } else {
        strcpy(&duid[PREFIX_LEN], "NOMACADDR");
    }
    return duid;
}
#pragma GCC diagnostic pop

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
    // URLs should come in with blob.core.windows.net and similar so baltimore cert should work for all
    req.tls_cert = (unsigned char*) IOTCONNECT_BALTIMORE_ROOT_CERT;
    req.tls_cert_len = IOTCONNECT_BALTIMORE_ROOT_CERT_SIZE;

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
        command_status(data, false, command, "Not implemented");
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
    	release_sw_der_auth_driver(auth_driver_context);
    }
}

static void publish_telemetry() {
    IotclMessageHandle msg = iotcl_telemetry_create(iotconnect_sdk_get_lib_config());

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
    iotcl_telemetry_set_number(msg, "cpu", 3.123); // test floating point numbers

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

/* Include the sample.  */
bool app_startup(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr) {
    printf("Starting App Version %s\r\n", APP_VERSION);
    IotConnectClientConfig *config = iotconnect_sdk_init_and_get_config();
    azrtos_config.ip_ptr = ip_ptr;
	azrtos_config.pool_ptr = pool_ptr;
	azrtos_config.dns_ptr = dns_ptr;

    config->cpid = IOTCONNECT_CPID;
    config->env = IOTCONNECT_ENV;
#ifdef IOTCONNECT_DUID
    config->duid = IOTCONNECT_DUID; // custom device ID
#else
    config->duid = (char*) compose_device_id(); // <DUID_PREFIX>-<MAC Address>
#endif
    config->cmd_cb = on_command;
    config->ota_cb = on_ota;
    config->status_cb = on_connection_status;

#ifdef IOTCONNECT_SYMETRIC_KEY
    config->auth.type = IOTC_KEY;
    config->auth.data.symmetric_key = IOTCONNECT_SYMETRIC_KEY;
#else
    config->auth.type = IOTC_X509;

#ifdef ENABLE_DDIM_TO_DRIVER_SAMPLE
    struct to_driver_parameters parameters = {0};
    parameters.operational_identity_slot = 2;
    parameters.operational_csr_slot = 2;
    parameters.bootstrap_identity_slot = 1;
    IotcDdimInterface ddim_interface;
    IotcAuthInterfaceContext auth_context;
    if(to_create_auth_driver( //
            &(config->auth.data.x509.auth_interface), //
            &ddim_interface, //
			&auth_context, //
			&parameters)) { //
    	return false;
    }
    config->auth.data.x509.auth_interface_context = auth_context;
    uint8_t serial[TO_HW_SN_SIZE];
    size_t serial_size;
    if(config->auth.data.x509.auth_interface.get_serial(
    		auth_context, //
			serial, //
			&serial_size//
			)) {
    	return false;
    }
    printf("DDIM:   SN: ");
	for (int i=0; i < serial_size; i++) {
		printf("%02x", serial[i]);
	}
	printf("\r\n");

   uint8_t* op_cert;
   size_t op_cert_size;

   config->auth.data.x509.auth_interface.get_cert(
		   	auth_context,
			&op_cert,
			&op_cert_size
			);

    // ignore error, try to get operational cert if cert size 0
    //if (0 == op_cert_size) {
        printf("Obtaining the operational certificate via DDIM.\r\n");
        if (iotcdi_obtain_operational_identity(&azrtos_config, &ddim_interface, auth_context, config->env)) {
            printf("Failed to obtain the operational certificate via DDIM.\r\n");
            return false;
        }
    //}
    if (false == extract_cpid_and_duid_from_operational_cn(config, ddim_interface.extract_operational_cn(auth_context))) {
    	return false;
    }

#else // not ENABLE_DDIM_TO_DRIVER_SAMPLE (software x509)
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

#endif // ENABLE_DDIM_TO_DRIVER_SAMPLE

#endif // IOTCONNECT_SYMETRIC_KEY

    while (true) {
#ifdef MEMORY_TEST
        // check for leaks
        memory_test();
#endif //MEMORY_TEST
        if (iotconnect_sdk_init(&azrtos_config)) {
#ifdef ENABLE_DDIM_TO_DRIVER_SAMPLE
        	// We should try obtain a new operational identity
        	IotclSyncResult sync_result = iotconnect_get_last_sync_result();
        	if (IOTCL_SR_DEVICE_NOT_REGISTERED == sync_result || IOTCL_SR_DEVICE_INACTIVE == sync_result) {
                if (iotcdi_obtain_operational_identity(&azrtos_config, &ddim_interface, auth_context, config->env)) {
                	printf("Operational certificate is missing. Obtaining it via DDIM.\r\n");
                	return false;
                }
				if (false == extract_cpid_and_duid_from_operational_cn(config, ddim_interface.extract_operational_cn(auth_context))) {
					return false;
				}
				printf("Obtained new operational certificate from DDIM.\r\n");
				continue;
        	}
#endif
            printf("Unable to establish the IoTConnect connection.\r\n");
            return false;
        }

        tx_thread_sleep(1000);
        iothub_start_device_agent(
            IOTC_ADU_STM,
            IOTC_ADU_STM32L4S5,
            "AVNET",
            "SAMPLEAPP",
            APP_VERSION
            );

        // send telemetry periodically
        for (int i = 0; i < 50; i++) {
            if (iotconnect_sdk_is_connected()) {

                publish_telemetry();  // underlying code will report an error
                iotconnect_sdk_poll(15000);
            } else {
                return false;
            }
        }
        iotconnect_sdk_disconnect();
    }
    printf("Done.\r\n");
    return true;
}
