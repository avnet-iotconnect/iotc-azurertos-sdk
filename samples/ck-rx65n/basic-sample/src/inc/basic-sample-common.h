#ifndef BASIC_SAMPLE_COMMON
#define BASIC_SAMPLE_COMMON

#define CLI_MODE
#define USB_DEMO_SUPPORT

#ifdef CLI_MODE

// be careful with changing these constants
#define IOTC_CONFIG_BUFF_LEN_GENERIC 64
#define IOTC_CONFIG_BUFF_LEN_SYMMKEY 256
typedef struct credentials {
  char cpid[IOTC_CONFIG_BUFF_LEN_GENERIC];
  char env[IOTC_CONFIG_BUFF_LEN_GENERIC];
  char duid[IOTC_CONFIG_BUFF_LEN_GENERIC];
  char symmkey[IOTC_CONFIG_BUFF_LEN_SYMMKEY];
} credentials_t;

#ifdef USB_DEMO_SUPPORT

#define JSON_BUFFER_LEN_MULTIPLIER 3
#define JSON_BUFFER_LEN (sizeof(credentials_t) * JSON_BUFFER_LEN_MULTIPLIER)

#endif

#endif // CLI_MODE

#endif // BASIC_SAMPLE_COMMON