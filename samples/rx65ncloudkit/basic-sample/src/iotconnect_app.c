//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
#include "app_config.h"

#include "nx_api.h"
#include "nxd_dns.h"
#include "iotconnect_common.h"
#include "iotconnect.h"
#include "iotconnect_certs.h"
#include "azrtos_ota_fw_client.h"
#include "iotc_auth_driver.h"
#include "sw_auth_driver.h"

#include "hardware_setup.h"

static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context;

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



// On some hardware tested, the behaviour of free() causes crashes if trying to free a null pointer
// the macro avoids this and also sets the pointer to null afterwards
#define FREE(x) if ((x)) { free((void*)x); (x) = NULL; }


typedef enum command_type
{
    COMMAND_UNKNOWN = 0,
    ECHO,
    LED,
    COMMAND_MAX
} command_type_t;

const char *command_strings[] = {
    [ECHO] = "echo ",
    [LED] = "led ",
};

static bool does_command_match(const char* input_str, const command_type_t command_enum)
{
    return (strncmp((input_str), command_strings[(command_enum)], strlen(command_strings[(command_enum)])) == 0);
}


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

static void publish_message(const char* key_str,const char* value_str) {
    IotclMessageHandle msg = iotcl_telemetry_create();
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, key_str, value_str);

    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    printf("Sending: %s\r\n", str);
    iotconnect_sdk_send_packet(str); // underlying code will report an error
    iotcl_destroy_serialized(str);
}

static command_type_t get_command_type(const char* command_str) {

    for (command_type_t i = COMMAND_UNKNOWN; i < COMMAND_MAX; i++)
    {
        if (does_command_match(command_str, i))
        {
            return i;
        }
    }

    printf("Unknown command\r\n");
    return COMMAND_UNKNOWN;
}

static bool command_led(const char* command_str){

    if (!command_str)
    {
        return false;
    }

    const char* setting_ptr = command_str + strlen(command_strings[LED]);
    RX65N_LED_STATE state = (setting_ptr[0] == '1') || (strncmp(setting_ptr, "true", sizeof("true")) == 0 ) ? ON : OFF;
    set_led(LED2, state);
    return true;    
}

// allocates data
static void iotc_create_and_send_ack(IotclEventData data, bool success)
{
    const char* message = success ? "OK" : "Failed_or_not_implemented";

    const char *ack = iotcl_create_ack_string_and_destroy_event(data, success, message);
    printf("Sent CMD ack: %s\r\n", ack);
    iotconnect_sdk_send_packet(ack);
    FREE(ack);
}

static void command_status(IotclEventData data, const char *command_name) {

    bool success = true; // set false by default case
    int command_type = get_command_type(command_name);
    switch (command_type){
    case ECHO:
        printf("%s\r\n", &command_name[strlen(command_strings[ECHO])]);
        break;
    case LED:
        if (!command_led(command_name)){
            success = false;
        }
        break;
    default:
        success = false;
        printf("Unsupported command\r\n");
        break;
    }

    publish_message("last_command", command_name);
    iotc_create_and_send_ack(data,success);
}


static void on_command(IotclEventData data) {
    char *command = iotcl_clone_command(data);
    if(!command)
    {
        iotc_create_and_send_ack(data, false);
        FREE(command);
        return;
    }

    command_status(data, command);
    FREE(command);

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
    IotclMessageHandle msg = iotcl_telemetry_create();

    // Optional. The first time you create a data point, the current timestamp will be automatically added
    // TelemetryAddWith* calls are only required if sending multiple data points in one packet.
    iotcl_telemetry_add_with_iso_time(msg, iotcl_iso_timestamp_now());
    iotcl_telemetry_set_string(msg, "version", APP_VERSION);
//   iotcl_telemetry_set_number(msg, "cpu", 3.123); // test floating point numbers
    // random number 0-100, cast to int so that it removes decimals in json
    iotcl_telemetry_set_number(msg, "random", (int)((double)rand() / (double)RAND_MAX * 100.0));

    iotcl_telemetry_set_number(msg, "button", read_user_switch());

//    sensors_add_telemetry(msg);

    const char *str = iotcl_create_serialized_string(msg, false);
    iotcl_telemetry_destroy(msg);
    if(NULL != str) {
        printf("Sending: %s\r\n", str);
        iotconnect_sdk_send_packet(str); // underlying code will report an error
        iotcl_destroy_serialized(str);
    }
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

#endif  // IOTCONNECT_SYMETRIC_KEY

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
        for (int i = 0; i < 50; i++) {
            if (iotconnect_sdk_is_connected()) {
                publish_telemetry();  // underlying code will report an error
                iotconnect_sdk_poll(5000);
            } else {
                return false;
            }
        }
        iotconnect_sdk_disconnect();
    }
    printf("Done.\r\n");
    return true;
}


