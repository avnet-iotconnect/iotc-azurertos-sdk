#ifndef JSON_PARSER
#define JSON_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "iotconnect.h"
#include "cJSON.h"

#undef COMMAND_FUNCTION_PTR_PARSING

typedef enum file_read_mode {
    FMODE_ASCII = 0,
    FMODE_BIN = 1,
    FMODE_END
} file_read_mode_t;

typedef union reading_un
{
    int reading_int;
    char* reading_str;
} reading_un_t;

typedef struct sensor_info {
    char* s_name;
    char* s_path;
    file_read_mode_t mode;
    void* reading;
    
} sensor_info_t;

typedef struct sensors_data {

    __uint8_t size;
    sensor_info_t *sensor;

} sensors_data_t;

#ifdef COMMAND_FUNCTION_PTR_PARSING
#define COMMANDS_STRUCT_ELEM_COUNTER 3
typedef struct commands {

    char* name;
    void* f_ptr; // don't use atm?
    char* private_data;

} commands_t;
#else
#define COMMANDS_STRUCT_ELEM_COUNTER 2
typedef struct commands {

    char* name;
    char* private_data;

} commands_t;
#endif

typedef struct commands_data {

    int counter;
    commands_t *commands;

} commands_data_t;

int parse_json_config(const char* json_str, IotConnectClientConfig* iotc_config, commands_data_t *local_commadns, sensors_data_t *local_sensors, char** board);


#endif