//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//
#include "app_config.h"
#include <stdlib.h>

#include "azrtos_ota_fw_client.h"
#include "fx_api.h"
#include "iotc_auth_driver.h"
#include "iotconnect.h"
#include "iotconnect_certs.h"
#include "iotconnect_common.h"
#include "iotconnect_lib_config.h"
#include "nx_api.h"
#include "nxd_dns.h"
#include "rx65n_tsip_auth_driver.h"
#include "sw_auth_driver.h"

#include "hardware_setup.h"
#include "hs300x_sensor_thread_entry.h"

#include "basic-sample-common.h"

#ifdef CLI_MODE
#include "demo_scanf.h"
#include "r_flash_rx_if.h"

#ifdef USB_DEMO_SUPPORT
#include "json_parser.h"
#include "usb_thread_entry.h"
#endif

#define TX_SECONDS_TO_TICKS(x) (x * TX_TIMER_TICKS_PER_SECOND)

#endif

static IotConnectAzrtosConfig azrtos_config;
static IotcAuthInterfaceContext auth_driver_context;

#define MAX_RSA_2048BIT_CERTIFICATE_SIZE_IN_BYTES                              \
  (2048) // FIXME - Reduce this to max possible size of 2048bit RSA cert

#ifdef ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
static char
    duid_buffer[IOTC_COMMON_NAME_MAX_LEN + 1]; // from ATECC608 common name
#endif
#define APP_VERSION "01.00.00"

/***********************************************************************************************************************
 * ThreadX object control blocks definitions
 **********************************************************************************************************************/
TX_THREAD hs300x_sensor_thread;

/***********************************************************************************************************************
 * Stacks definitions
 **********************************************************************************************************************/
UCHAR hs300x_sensor_thread_memory_stack[1024];

#ifdef CLI_MODE

static char iotconnect_cpid[CONFIG_IOTCONNECT_CPID_MAX_LEN + 1] = {0};
static char iotconnect_env[CONFIG_IOTCONNECT_ENV_MAX_LEN + 1] = {0};
static char iotconnect_duid[CONFIG_IOTCONNECT_DUID_MAX_LEN + 1] = {0};
static char iotconnect_symmetric_key[256] = {0};

#endif

//#define MEMORY_TEST
#ifdef MEMORY_TEST
#define TEST_BLOCK_SIZE 10 * 1024
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
  printf("====Allocated %d blocks of size %d (of max %d)===\r\n", i,
         TEST_BLOCK_SIZE, TEST_BLOCK_COUNT);
  for (int j = 0; j < i; j++) {
    free(blocks[j]);
  }
}
#endif /* MEMORY_TEST */

#ifndef CLI_MODE
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static char *compose_device_id() {
#define PREFIX_LEN (sizeof(DUID_PREFIX) - 1)
  uint8_t mac_addr[6] = {0};
  static char duid[PREFIX_LEN + sizeof(mac_addr) * 2 + 1 /* null */] =
      DUID_PREFIX;
  for (int i = 0; i < sizeof(mac_addr); i++) {
    sprintf(&duid[PREFIX_LEN + i * 2], "%02x", mac_addr[i]);
  }
  printf("DUID: %s\r\n", duid);
  return duid;
}
#pragma GCC diagnostic pop
#endif // CLI_MODE

static void on_ota(IotclEventData data) {
  const char *message = NULL;
  bool success = false;

  message = "Firmware OTA not supported by device";
  const char *ack =
      iotcl_create_ack_string_and_destroy_event(data, success, message);
  if (NULL != ack) {
    printf("Sent OTA ack: %s\r\n", ack);
    iotconnect_sdk_send_packet(ack);
    free((void *)ack);
  }
}

static void command_status(IotclEventData data, bool status,
                           const char *command_name, const char *message) {
  const char *ack =
      iotcl_create_ack_string_and_destroy_event(data, status, message);
  printf("command: %s status=%s: %s\r\n", command_name,
         status ? "OK" : "Failed", message);
  printf("Sent CMD ack: %s\r\n", ack);
  iotconnect_sdk_send_packet(ack);
  free((void *)ack);
}

static void on_command(IotclEventData data) {
  char *context = NULL;
  char *command = iotcl_clone_command(data);
  if (NULL != command) {
    char *token = strtok_r(command, " ", &context);
    if (!strncmp(token, "led", 3)) {
      token = strtok_r(NULL, " ", &context);
      RX65N_LED_STATE state =
          (token[0] == '1') || (strncmp(token, "true", 4) == 0) ? ON : OFF;
      set_led(LED2, state);
      set_led(LED1, state);
      command_status(data, true, command, "LED set");
    } else {
      command_status(data, false, command, "Not implemented");
    }
    free((void *)command);
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

  // Optional. The first time you create a data point, the current timestamp
  // will be automatically added TelemetryAddWith* calls are only required if
  // sending multiple data points in one packet.
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

#ifdef CLI_MODE

#define CREDENTIALS_BLOCK_DATA FLASH_DF_BLOCK_24

static flash_err_t save_to_flash(void *data, uint32_t address, size_t size) {

  flash_err_t ret;

  ret = R_FLASH_Erase(address, size);
  if (ret != FLASH_SUCCESS) {
    printf("Failed to erase credentials data before writing\r\n");
    return ret;
  }

  ret = R_FLASH_Write((uint32_t *)data, address, size);

  if (ret != FLASH_SUCCESS) {
    printf("Failed to write credentials data\r\n");
  }

  return ret;
}

static flash_err_t load_from_flash(void **data, uint32_t address) {
  UINT status;

  flash_err_t ret = FLASH_SUCCESS;

  *data = address;
  if (!data) {
    printf("nothing found at the provided address: 0x%x\r\n", address);
    data = NULL;
    ret = FLASH_ERR_FAILURE;
  } else {
    printf("Loaded successfully\r\n");
  }

  return ret;
}

static flash_err_t save_credentials(credentials_t *credentials) {
  return save_to_flash(credentials, CREDENTIALS_BLOCK_DATA,
                       sizeof(credentials_t));
}

static flash_err_t load_credentials(credentials_t *credentials) {
  return load_from_flash(credentials, CREDENTIALS_BLOCK_DATA);
}
static void loop_for_input(char *param) {
  char ready = 0;
  while (1) {
    printf("type 1 when ready to input %s\r\n", param);
    ready = fgetc(stdin);
    if (ready == '1') {
      printf("\r\n");
      break;
    }
    printf("\r\n");
  }
}

#ifdef USB_DEMO_SUPPORT

static bool config_update_required_usb;
void config_update_required_set();

// this function requires iotc config credentials to be freed prior to calling
// it
static int
iotc_app_parse_json_and_update_config(IotConnectClientConfig *config);

void config_update_required_set() { config_update_required_usb = true; }

static int
iotc_app_parse_json_and_update_config(IotConnectClientConfig *config) {
  int ret = 0;

  char json_str[JSON_BUFFER_LEN];

  get_json_str(&json_str);

  if (!json_str) {
    printf("NULL json str?\r\n");
    return -1;
  }

  /*
  IotConnectClientConfig *new_config =
      calloc(1, sizeof(IotConnectClientConfig));
  */

  commands_data_t commands;
  sensors_data_t sensors;
  char board[64];

  ret = parse_json_config(json_str, config, &commands, &sensors, &board);

  return ret;
}

#endif
#endif

/* Include the sample.  */
static bool app_startup(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr,
                        NX_DNS *dns_ptr) {
  printf("Starting App Version %s\r\n", APP_VERSION);
  IotConnectClientConfig *config = iotconnect_sdk_init_and_get_config();
  azrtos_config.ip_ptr = ip_ptr;
  azrtos_config.pool_ptr = pool_ptr;
  azrtos_config.dns_ptr = dns_ptr;

  //
  // If CLI_MODE is defined, then the user
  // will be prompted on a terminal to input values to be used for:
  // - IOTCONNECT_CPID
  // - IOTCONNECT_ENV
  // - IOTCONNECT_DUID
  // - IOTCONNECT_SYMETRIC_KEY
  //
  // X.509 certificate based schemes still require setting values in this file
  // and recompiling without CLI_MODE defined.
  //

#ifdef CLI_MODE

  credentials_t *credentials;
  bool save_req = false;

  char in_buff = 0;
  while (1) {
    printf("to load credentials: type '1' (this will be executed in 5s)\r\nto "
           "enter credentials: type '2'\r\n");

    in_buff = my_sw_charget_function_timeout(TX_SECONDS_TO_TICKS(5));

    printf("\r\n");

    if (in_buff == '1' || in_buff == CLI_NO_INPUT) {
      if (load_credentials(&credentials) == FLASH_SUCCESS) {
        save_req = false;
        break;
      } else {
        printf("Failed to load credentials from file. Try again or input "
               "them\r\n");
      }
    } else if (in_buff == '2') {

      credentials = (credentials_t *)malloc(sizeof(credentials_t));

      if (!credentials) {
        printf("failed to malloc\r\n");
        return false;
      }
      save_req = true;
      printf("Device is setup for interactive input of its symmetric "
             "key:\r\n\r\n");

      printf("Type the CPID:\r\n");
      my_sw_get_string(&credentials->cpid, IOTC_CONFIG_BUFF_LEN_GENERIC);
      printf("\r\n");

      printf("Type the ENV:\r\n");
      my_sw_get_string(credentials->env, IOTC_CONFIG_BUFF_LEN_GENERIC);
      printf("\r\n");

      printf("Type the DUID:\r\n");
      my_sw_get_string(credentials->duid, IOTC_CONFIG_BUFF_LEN_GENERIC);
      printf("\r\n");

      printf("Type the SYMMETRIC_KEY:\r\n");
      my_sw_get_string(credentials->symmkey, IOTC_CONFIG_BUFF_LEN_SYMMKEY);
      printf("\r\n");
      break;
    }
  }

  //
  // Only options for CLI_MODE
  //
  config->cpid = credentials->cpid;
  config->env = credentials->env;
  config->duid = credentials->duid;
  config->auth.type = IOTC_KEY;
  config->auth.data.symmetric_key = credentials->symmkey;

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
  config->duid = (char *)compose_device_id(); // <DUID_PREFIX>-<MAC Address>
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
  if (rx65n_tsip_create_auth_driver(                //
          &(config->auth.data.x509.auth_interface), //
          &ddim_interface,                          //
          &auth_context,                            //
          &parameters)) {                           //
    return false;
  }
  config->auth.data.x509.auth_interface_context = auth_context;

  uint8_t *cert;
  size_t cert_size;

  if (config->auth.data.x509.auth_interface.get_cert(auth_context, &cert,
                                                     &cert_size) != 0) {
    printf("Unable to get the certificate from the driver.\r\n");
    rx65n_tsip_release_auth_driver(auth_context);
    return false;
  }
  if (0 == cert_size) {
    printf("Unable to get the certificate from the driver.\r\n");
    rx65n_tsip_release_auth_driver(auth_context);
    return false;
  }
  char *b64_string_buffer = malloc(MAX_RSA_2048BIT_CERTIFICATE_SIZE_IN_BYTES);
  if (NULL == b64_string_buffer) {
    printf("WARNING: Unable to allocate memory to display the base64 encoded "
           "certificate contents.\r\n");
  } else {
    size_t b64_string_len =
        MAX_RSA_2048BIT_CERTIFICATE_SIZE_IN_BYTES; // in/out parameter. See
                                                   // atcacert_encode_pem_cert()
    unsigned int bytes_copied = 0;
    if (_nx_utility_base64_encode(cert, cert_size, b64_string_buffer,
                                  b64_string_len, &bytes_copied)) {
      printf("Failed to base64 encode the cert\r\n");
    } else {
      b64_string_buffer[b64_string_len] = 0;
      printf("Device certificate (%u bytes):\r\n%s\r\n", bytes_copied,
             b64_string_buffer);
    }
    free(b64_string_buffer);
  }

  printf("Obtained device certificate:\r\n\r\n");
  char *operational_cn = ddim_interface.extract_operational_cn(auth_context);
  if (NULL == operational_cn) {
    rx65n_tsip_release_auth_driver(auth_context);
    printf("Unable to get the certificate common name.\r\n");
    return false;
  }
  // Strip the CPID and '-' prefix off of the common name (CN) in the device
  // cert
  strcpy(duid_buffer, (strchr(operational_cn, '-') + 1));
  config->duid = duid_buffer;
  printf("DUID: %s\r\n", config->duid);

  auth_driver_context = auth_context;

#else // ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
  extern const UCHAR sample_device_cert_ptr[];
  extern const UINT sample_device_cert_len;
  extern const UCHAR sample_device_private_key_ptr[];
  extern const UINT sample_device_private_key_len;
  struct sw_der_driver_parameters dp = {0};
  dp.cert = (uint8_t *)(sample_device_cert_ptr);
  dp.cert_size = sample_device_cert_len;
  dp.key = (uint8_t *)(sample_device_private_key_ptr);
  dp.key_size = sample_device_private_key_len;
#ifdef ECC_CRYPTO
  dp.crypto_method = &crypto_method_ec_secp256;
#else // RSA_CRYPTO
  dp.crypto_method = &crypto_method_rsa;
#endif
  if (create_sw_der_auth_driver(                            //
          &(config->auth.data.x509.auth_interface),         //
          &(config->auth.data.x509.auth_interface_context), //
          &dp)) {                                           //
    return false;
  }
  auth_driver_context = config->auth.data.x509.auth_interface_context;
#endif // ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
#endif // IOTCONNECT_SYMETRIC_KEY
#endif // CLI_MODE

  while (true) {
    /* Switch to another thread */
    tx_thread_sleep(1);

#ifdef MEMORY_TEST
    // check for leaks
    memory_test();
#endif // MEMORY_TEST
    if (iotconnect_sdk_init(&azrtos_config)) {
      printf("Unable to establish the IoTConnect connection.\r\n");
      if (credentials) {
        free(credentials);
        credentials = NULL;
      }
      return false;
    }
    printf("sizeof iotc conf: %d conf var: %d\r\n",
           sizeof(IotConnectClientConfig), sizeof(config));
    // save credentials to a file
#ifdef CLI_MODE
    if (save_req) {
      if (save_credentials(credentials) != FX_SUCCESS) {
        printf("Failed to save credentials to a file\r\n");
      } else {
        printf("Successfully saved credentials to a file\r\n");
      }
    }
#endif

    // send telemetry periodically
    for (int i = 0; i < 50; i++) {
      /* Switch to another thread */
      tx_thread_sleep(1);

      if (iotconnect_sdk_is_connected()) {
        publish_telemetry(); // underlying code will report an error
        iotconnect_sdk_poll(5000);
      } else {
        if (credentials) {
          free(credentials);
          credentials = NULL;
        }
        return false;
      }

#ifdef USB_DEMO_SUPPORT
      if (config_update_required_usb) {
        printf("received new credentials config via USB!\r\n");
        if (credentials) {
          free(credentials);
          credentials = NULL;
        }
        // TODO: check config here
        if (iotc_app_parse_json_and_update_config(config) != 0) {
          printf("failed to parse new json!\r\n");
          return false;
        } else {
          // copy newly received config into local cred structure to write it
          // into flash
          credentials_t new_creds;
          strncpy(&new_creds.cpid, config->cpid, IOTC_CONFIG_BUFF_LEN_GENERIC);
          strncpy(&new_creds.env, config->env, IOTC_CONFIG_BUFF_LEN_GENERIC);
          strncpy(&new_creds.duid, config->duid, IOTC_CONFIG_BUFF_LEN_GENERIC);
          strncpy(&new_creds.symmkey, config->auth.data.symmetric_key,
                  IOTC_CONFIG_BUFF_LEN_SYMMKEY);
          if (save_credentials(&new_creds) != FX_SUCCESS) {
            printf("Failed to save new credentials to a file\r\n");
          } else {
            printf("Successfully saved new credentials to a file\r\n");
          }
        }
        config_update_required_usb = false;
      }
#endif
    }
    iotconnect_sdk_disconnect();
  }
  if (NULL != auth_driver_context) {
#ifdef ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
    rx65n_tsip_release_auth_driver(auth_driver_context);
#else  // ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
    release_sw_der_auth_driver(auth_driver_context);
#endif // ENABLE_RX_TSIP_AUTH_DRIVER_SAMPLE
  }
  if (credentials) {
    free(credentials);
    credentials = NULL;
  }
  printf("Done.\r\n");
  return true;
}

//
// Allow user more than one chance to type in details
//
bool app_startup_interactive(NX_IP *ip_ptr, NX_PACKET_POOL *pool_ptr,
                             NX_DNS *dns_ptr) {
  bool status;

  /* Start HS3001 sensor.  */
  UINT status_hs300x_sensor_thread;
  status_hs300x_sensor_thread = tx_thread_create(
      &hs300x_sensor_thread, "Sensor Thread", hs300x_sensor_thread_entry, 0,
      hs300x_sensor_thread_memory_stack, 1024, 1, 1, 0, TX_AUTO_START);
  if (TX_SUCCESS != status_hs300x_sensor_thread) {
    printf("Failed to create status_hs300x_sensor_thread.\r\n");
  }

  // status = find_credentials_data();
  // printf("status from find_credentials_data: %d\r\n", status);

  while (1) {
    char repeat[16] = "n";

    status = app_startup(ip_ptr, pool_ptr, dns_ptr);

#ifdef CLI_MODE
    repeat[0] = 'y';
    printf("Repeat [y/n]? (auto-repeat in 10s)\r\n");
    repeat[0] = my_sw_charget_function_timeout(TX_SECONDS_TO_TICKS(10));

    printf("\r\n");
#endif
    if (repeat[0] != 'y' && repeat[0] != 'Y' && repeat[0] != CLI_NO_INPUT) {
      break;
    }
  }
  return status;
}
