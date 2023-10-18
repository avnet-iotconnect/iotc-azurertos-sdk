//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
#include <stdlib.h>
#include "app_config.h"

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect_common.h"
#include "iotconnect.h"
#include "iotconnect_certs.h"
#include "iotconnect_lib_config.h"
#include "azrtos_ota_fw_client.h"
#include "iotc_auth_driver.h"
#include "sw_auth_driver.h"
#include "rx65n_tsip_auth_driver.h"
#include "fx_api.h"

#include "hardware_setup.h"
#include "hs300x_sensor_thread_entry.h"

static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context;

#define MAX_RSA_2048BIT_CERTIFICATE_SIZE_IN_BYTES (2048) //FIXME - Reduce this to max possible size of 2048bit RSA cert

#ifdef ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
static char duid_buffer[IOTC_COMMON_NAME_MAX_LEN + 1]; // from ATECC608 common name
#endif
#define APP_VERSION "01.00.00"

/***********************************************************************************************************************
 * ThreadX object control blocks definitions
 **********************************************************************************************************************/
TX_THREAD               hs300x_sensor_thread;

/***********************************************************************************************************************
 * Stacks definitions
 **********************************************************************************************************************/
UCHAR               hs300x_sensor_thread_memory_stack[1024];

#ifdef SYMMETRIC_KEY_INPUT

static char iotconnect_cpid[CONFIG_IOTCONNECT_CPID_MAX_LEN + 1] = {0};
static char iotconnect_env[CONFIG_IOTCONNECT_ENV_MAX_LEN + 1] = {0};
static char iotconnect_duid[CONFIG_IOTCONNECT_DUID_MAX_LEN + 1] = {0};
static char iotconnect_symmetric_key[256] = {0};

#endif



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

#ifndef SYMMETRIC_KEY_INPUT
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static char* compose_device_id() {
#define PREFIX_LEN (sizeof(DUID_PREFIX) - 1)
    uint8_t mac_addr[6] = { 0 };
    static char duid[PREFIX_LEN + sizeof(mac_addr) * 2 + 1 /* null */] = DUID_PREFIX;
    for (int i = 0; i < sizeof(mac_addr); i++) {
        sprintf(&duid[PREFIX_LEN + i * 2], "%02x", mac_addr[i]);
    }
    printf("DUID: %s\r\n", duid);
    return duid;
}
#pragma GCC diagnostic pop
#endif // SYMMETRIC_KEY_INPUT

static void on_ota(IotclEventData data) {
    const char *message = NULL;
    bool success = false;

    message = "Firmware OTA not supported by device";
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
    char* context = NULL;
    char *command = iotcl_clone_command(data);
    if (NULL != command) {
        char* token = strtok_r(command, " ", &context);
        if(!strncmp(token, "led", 3)) {
            token = strtok_r(NULL, " ", &context);
            RX65N_LED_STATE state = (token[0] == '1') ||
                                    (strncmp(token, "true", 4) == 0 ) ?
                                    ON : OFF;
            set_led(LED2, state);
            set_led(LED1, state);
            command_status(data, true, command, "LED set");
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
}

static void publish_telemetry() {
    const char *str;
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);

    iotcl_telemetry_set_number(msg, "temperature", get_temperature());
    iotcl_telemetry_set_number(msg, "humidity", get_humidity());

    iotcl_telemetry_set_number(msg, "button", read_user_switch());

//    sensors_add_telemetry(msg);
    str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    if (NULL != str) {
        printf("Sending: %s\r\n", str);
        iotconnect_sdk_send_packet(str); // underlying code will report an error
        iotcl_destroy_serialized(str);
    }
}


#ifdef SYMMETRIC_KEY_INPUT

/*
 * global variables for filex
 *
 * Taken from e2studio ccrx demo
 *
 *  */
extern CHAR* ram_disk_memory;
FX_MEDIA        ram_disk;
FX_FILE         my_file;
const char media_name[] = "RAM DISK";

VOID _fx_ram_driver(FX_MEDIA *media_ptr);

/* Buffer for FileX FX_MEDIA sector cache. This must be large enough for at least one
   sector, which are typically 512 bytes in size.  */

unsigned char media_memory[512];

//
const char credentials_filename[] = "creds.txt";



// be careful with changing these constants
#define IOTC_CONFIG_BUFF_LEN_GENERIC 64
#define IOTC_CONFIG_BUFF_LEN_SYMMKEY 256
typedef struct credentials {
	char cpid[IOTC_CONFIG_BUFF_LEN_GENERIC];
	char env[IOTC_CONFIG_BUFF_LEN_GENERIC];
	char duid[IOTC_CONFIG_BUFF_LEN_GENERIC];
	char symmkey[IOTC_CONFIG_BUFF_LEN_SYMMKEY];
} credentials_t;

static UINT save_creds_to_file(void *data, size_t size, const char* filename){
	UINT status;

	status =  fx_media_open(&ram_disk, media_name, _fx_ram_driver, ram_disk_memory, media_memory, sizeof(media_memory));

	/* Check the media open status.  */
	if (status != FX_SUCCESS)
	{
		printf("Failed to open filex media status: %d\r\n", status);
		goto SAVE_CLEANUP;
	}

	status =  fx_file_open(&ram_disk, &my_file, filename, FX_OPEN_FOR_READ);

	if (status != FX_SUCCESS) {
		printf("Failed to open file. status: %d\r\n", status);
		if (status != FX_NOT_FOUND){
			goto SAVE_CLEANUP;
		}
		printf("Creating new credentials file\r\n");
	} else {
		printf("File found. Deleting before writing\r\n");

		status = fx_file_close(&my_file);
		if (status != FX_SUCCESS){
			printf("failed to close filex file\r\n");
			goto SAVE_CLEANUP;
		}

		status = fx_file_delete(&ram_disk, filename);

		if (status != FX_SUCCESS){
			printf("Failed to delete credentials file error: %d\r\n", status);
			goto SAVE_CLEANUP;
		}

	}

	status = fx_file_create(&ram_disk, filename);

	if (status != FX_SUCCESS){
		printf("Failed to create file. status: %d\r\n", status);
		goto SAVE_CLEANUP;
	}

	status =  fx_file_open(&ram_disk, &my_file, filename, FX_OPEN_FOR_WRITE);

	if (status != FX_SUCCESS){
		printf("Failed to open file\r\n");
		goto SAVE_CLEANUP;
	}

	status =  fx_file_seek(&my_file, 0);

	if (status != FX_SUCCESS){
		printf("failed to rewind file\r\n");
		goto SAVE_CLEANUP;
	}

	status = fx_file_write(&my_file, data, size);

	if (status != FX_SUCCESS){
		printf("failed to write data to file\r\n");
		goto SAVE_CLEANUP;
	}
	status =  fx_file_seek(&my_file, 0);

	if (status != FX_SUCCESS){
		printf("failed to rewind file\r\n");
		goto SAVE_CLEANUP;
	}



	status = fx_media_flush(&ram_disk);

	if (status != FX_SUCCESS){
		printf("fx_media_flush failed: %d\r\n", status);
		goto SAVE_CLEANUP;
	}

	UINT status_cleanup;
SAVE_CLEANUP:

	status_cleanup = fx_file_close(&my_file);
	if (status_cleanup != FX_SUCCESS && status_cleanup != FX_NOT_OPEN){
		printf("failed to close filex file: %d\r\n");
		return status_cleanup;
	}
	status_cleanup = fx_media_close(&ram_disk);

	if (status_cleanup != FX_SUCCESS && status_cleanup != FX_MEDIA_NOT_OPEN){
		printf("failed to close filex media\r\n");
		return status_cleanup;
	}

	return status;
}

static UINT load_creds_from_file(void *data, size_t size_to_read, const char* filename){
	UINT status;

	status =  fx_media_open(&ram_disk, media_name, _fx_ram_driver, ram_disk_memory, media_memory, sizeof(media_memory));

		/* Check the media open status.  */
	if (status != FX_SUCCESS)
	{
		printf("Failed to open filex media status: %d\r\n", status);
		goto LOAD_CLEANUP;
	}

	status =  fx_file_open(&ram_disk, &my_file, filename, FX_OPEN_FOR_READ);

	if (status != FX_SUCCESS){
		printf("Failed to open file\r\n");
		goto LOAD_CLEANUP;
	}

	status =  fx_file_seek(&my_file, 0);

	if (status != FX_SUCCESS){
		printf("failed to rewind file\r\n");
		goto LOAD_CLEANUP;
	}

	ULONG actual = 0;

	status = fx_file_read(&my_file, data, size_to_read, &actual);
	if (status != FX_SUCCESS){
		printf("failed to read creds file\r\n");
		goto LOAD_CLEANUP;
	}

	status = fx_file_close(&my_file);
	if (status != FX_SUCCESS){
		printf("failed to close filex file\r\n");
		goto LOAD_CLEANUP;
	}

	status = fx_media_close(&ram_disk);

	if (status != FX_SUCCESS){
		printf("failed to close filex media\r\n");

	}
	UINT status_cleanup;
LOAD_CLEANUP:


	status_cleanup = fx_file_close(&my_file);
	if (status_cleanup != FX_SUCCESS && status_cleanup != FX_NOT_OPEN){
		printf("failed to close filex file: %d\r\n");
		return status_cleanup;
	}
	status_cleanup = fx_media_close(&ram_disk);

	if (status_cleanup != FX_SUCCESS && status_cleanup != FX_MEDIA_NOT_OPEN){
		printf("failed to close filex media\r\n");
		return status_cleanup;
	}


	return status;
}
#endif

/* Include the sample.  */
static bool app_startup(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr) {
    printf("Starting App Version %s\r\n", APP_VERSION);
    IotConnectClientConfig *config = iotconnect_sdk_init_and_get_config();
    azrtos_config.ip_ptr = ip_ptr;
    azrtos_config.pool_ptr = pool_ptr;
    azrtos_config.dns_ptr = dns_ptr;

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


#ifdef SYMMETRIC_KEY_INPUT


	credentials_t creds;

    char in_buff[2];
    while(1){
    	printf("to load credentials: type '1'\r\nto enter credentials: type '2'\r\n");
    	scanf("%c", in_buff);
    	printf("\r\n");
    	if (in_buff[0] == '1'){
    		if (load_creds_from_file(&creds, sizeof(credentials_t), credentials_filename) == FX_SUCCESS){
    			break;
    		} else {
    			printf("Failed to load credentials from file. Try again or input them\r\n");
    		}
    	}else if (in_buff[0] == '2') {

    		printf("Device is setup for interactive input of its symmetric key:\r\n\r\n");


    		printf("Type the CPID:\r\n");
    		scanf("%s", creds.cpid);
    		printf("\r\n");
    		printf("Type the ENV:\r\n");
    		scanf("%s", creds.env);
    		printf("\r\n");
    		printf("Type the DUID:\r\n");
    		scanf("%s", creds.duid);
    		printf("\r\n");
    		printf("Type the SYMMETRIC_KEY:\r\n");
    		scanf("%s", creds.symmkey);
    		printf("\r\n");
    		break;
    	}
    }


    //
    // Only options for SYMMETRIC_KEY_INPUT
    //
    config->cpid = creds.cpid;
    config->env = creds.env;
    config->duid = creds.duid;
    config->auth.type = IOTC_KEY;
    config->auth.data.symmetric_key = creds.symmkey;

    //
    // Echo back values typed in, just for confirmation
    //
    printf("The values you typed were:\r\n");
    printf("CPID: '%s'\r\n", config->cpid);
    printf("ENV: '%s'\r\n", config->env);
    printf("DUID: '%s'\r\n", config->duid);
    printf("SYMMETRIC_KEY: '%s'\r\n", config->auth.data.symmetric_key);


#else
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

#ifdef ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
    auth_driver_context = NULL;
    struct rx65n_tsip_driver_parameters parameters = {0}; // dummy, for now
    IotcDdimInterface ddim_interface;
    IotcAuthInterfaceContext auth_context;
    if(rx65n_tsip_create_auth_driver( //
            &(config->auth.data.x509.auth_interface), //
            &ddim_interface, //
            &auth_context, //
            &parameters)) { //
        return false;
    }
    config->auth.data.x509.auth_interface_context = auth_context;

    uint8_t* cert;
    size_t cert_size;

    if (config->auth.data.x509.auth_interface.get_cert(
            auth_context, &cert,&cert_size) != 0)
    {
        printf("Unable to get the certificate from the driver.\r\n");
        rx65n_tsip_release_auth_driver(auth_context);
        return false;
    }
    if (0 == cert_size) {
        printf("Unable to get the certificate from the driver.\r\n");
        rx65n_tsip_release_auth_driver(auth_context);
        return false;
    }
    char* b64_string_buffer = malloc(MAX_RSA_2048BIT_CERTIFICATE_SIZE_IN_BYTES);
    if (NULL == b64_string_buffer) {
        printf("WARNING: Unable to allocate memory to display the base64 encoded certificate contents.\r\n");
    } else {
        size_t b64_string_len = MAX_RSA_2048BIT_CERTIFICATE_SIZE_IN_BYTES; // in/out parameter. See atcacert_encode_pem_cert()
        unsigned int bytes_copied = 0;
        if (_nx_utility_base64_encode(cert, cert_size, b64_string_buffer, b64_string_len, &bytes_copied))
        {
            printf("Failed to base64 encode the cert\r\n");
        }
        else
        {
            b64_string_buffer[b64_string_len] = 0;
            printf("Device certificate (%u bytes):\r\n%s\r\n", bytes_copied, b64_string_buffer);
        }
        free(b64_string_buffer);
    }

    printf("Obtained device certificate:\r\n\r\n");
    char* operational_cn = ddim_interface.extract_operational_cn(auth_context);
    if (NULL == operational_cn) {
        rx65n_tsip_release_auth_driver(auth_context);
        printf("Unable to get the certificate common name.\r\n");
        return false;
    }
    // Strip the CPID and '-' prefix off of the common name (CN) in the device cert
    strcpy(duid_buffer, (strchr(operational_cn, '-') + 1));
    config->duid = duid_buffer;
    printf("DUID: %s\r\n", config->duid);

    auth_driver_context = auth_context;

#else //ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
    extern const UCHAR sample_device_cert_ptr[];
    extern const UINT sample_device_cert_len;
    extern const UCHAR sample_device_private_key_ptr[];
    extern const UINT sample_device_private_key_len;
    struct sw_der_driver_parameters dp = {0};
    dp.cert = (uint8_t *) (sample_device_cert_ptr);
    dp.cert_size = sample_device_cert_len;
    dp.key = (uint8_t *) (sample_device_private_key_ptr);
    dp.key_size = sample_device_private_key_len;
#ifdef ECC_CRYPTO
    dp.crypto_method = &crypto_method_ec_secp256;
#else //RSA_CRYPTO
    dp.crypto_method = &crypto_method_rsa;
#endif
    if(create_sw_der_auth_driver( //
                &(config->auth.data.x509.auth_interface), //
                &(config->auth.data.x509.auth_interface_context), //
                &dp)) { //
            return false;
        }
    auth_driver_context = config->auth.data.x509.auth_interface_context;
#endif // ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
#endif  // IOTCONNECT_SYMETRIC_KEY
#endif  // SYMMETRIC_KEY_INPUT

    while (true) {
        /* Switch to another thread */
        tx_thread_sleep(1);

#ifdef MEMORY_TEST
        // check for leaks
        memory_test();
#endif //MEMORY_TEST
        if (iotconnect_sdk_init(&azrtos_config)) {
            printf("Unable to establish the IoTConnect connection.\r\n");
            return false;
        }
        // save credentials to a file
#ifdef SYMMETRIC_KEY_INPUT
        if (save_creds_to_file(&creds, sizeof(credentials_t), credentials_filename) != FX_SUCCESS){
        	printf("Failed to save credentials to a file\r\n");
        } else {
        	printf("Successfully saved credentials to a file\r\n");
        }
#endif


        // send telemetry periodically
        for (int i = 0; i < 50; i++) {
            /* Switch to another thread */
            tx_thread_sleep(1);

            if (iotconnect_sdk_is_connected()) {
                publish_telemetry();  // underlying code will report an error
                iotconnect_sdk_poll(5000);
            } else {
                return false;
            }
        }
        iotconnect_sdk_disconnect();
    }
    if (NULL != auth_driver_context) {
#ifdef ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
        rx65n_tsip_release_auth_driver(auth_driver_context);
#else  //ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
        release_sw_der_auth_driver(auth_driver_context);
#endif //ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
    }
    printf("Done.\r\n");
    return true;
}

//
// Allow user more than one chance to type in details
//
bool app_startup_interactive(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr, NX_DNS *dns_ptr) {
    bool status;

    /* Start HS3001 sensor.  */
    UINT status_hs300x_sensor_thread;
    status_hs300x_sensor_thread = tx_thread_create(&hs300x_sensor_thread, "Sensor Thread", hs300x_sensor_thread_entry, 0, hs300x_sensor_thread_memory_stack, 1024, 1, 1, 0, TX_AUTO_START);
    if (TX_SUCCESS != status_hs300x_sensor_thread)
    {
        printf("Failed to create status_hs300x_sensor_thread.\r\n");
    }


#ifdef SYMMETRIC_KEY_INPUT

    // checking if required media was created aleady
    status =  fx_media_open(&ram_disk, media_name, _fx_ram_driver, ram_disk_memory, media_memory, sizeof(media_memory));

        	/* Check the media open status.  */
    if (status != FX_SUCCESS)
    {
    printf("Failed to open filex media status: %d\r\n", status);

    fx_media_format(&ram_disk,
		_fx_ram_driver,         /* Driver entry*/
		ram_disk_memory,        /* RAM disk memory pointer*/
		media_memory,           /* Media buffer pointer*/
		sizeof(media_memory),   /* Media buffer size */
		"RAM_DISK",          /* Volume Name*/
		1,                      /* Number of FATs*/
		32,                     /* Directory Entries*/
		0,                      /* Hidden sectors*/
		256,                    /* Total sectors */
		128,                    /* Sector size  */
		1,                      /* Sectors per cluster*/
		1,                      /* Heads*/
		1);                     /* Sectors per track */

    } else {
		printf("Successfully opened filex media\r\n");
		if (fx_media_close(&ram_disk) != FX_SUCCESS){
			printf("Failed to close open media!\r\n");
			return false;
		}
    }
#endif


        	//status = find_credentials_data();
        	//printf("status from find_credentials_data: %d\r\n", status);


    while(1) {
        char repeat[16] = "n";

        status = app_startup(ip_ptr, pool_ptr, dns_ptr);

#ifdef SYMMETRIC_KEY_INPUT
        printf("Repeat [y/n]?\r\n");
        scanf("%s", repeat);
        printf("\r\n");
#endif
        if(repeat[0] != 'y' && repeat[0] != 'Y') {
            break;
        }
    }
    return status;
}
