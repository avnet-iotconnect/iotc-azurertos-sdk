/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

#include <stddef.h>
#include <string.h>
#include <stdlib.h>


#include "iotconnect_common.h"
#include "iotconnect_lib.h"
#include "iotconnect_telemetry.h"


#include <cJSON.h>

/////////////////////////////////////////////////////////
// cJSON implementation

struct IotclMessageHandleTag {
    cJSON *root_value;
    cJSON *telemetry_data_array;
    cJSON *current_telemetry_object; // an object inside the "d" array inside the "d" array of the root object
};

cJSON *json_object_dotset_locate(cJSON *search_object, char **leaf_name, const char *path) {
    static const char *DELIM = ".";
    char *mutable_path = iotcl_strdup(path);
    char *token = strtok(mutable_path, DELIM);
    *leaf_name = NULL;
    if (NULL == token) {
        // PANIC. Should not happen
        free(mutable_path);
        return NULL;
    }
    cJSON *target_object = search_object;
    do {
        free(*leaf_name);
        *leaf_name = iotcl_strdup(token);
        if (!*leaf_name) {
            goto cleanup;
        }
        char *last_token = *leaf_name;
        token = strtok(NULL, DELIM);
        if (NULL != token) {
            // we have more and need to create the nested object
            cJSON *parent_object;
            if (cJSON_HasObjectItem(target_object, last_token)) {
                parent_object = cJSON_GetObjectItem(target_object, last_token);
            } else {
                parent_object = cJSON_AddObjectToObject(target_object, last_token);

                // NOTE: The user should clean up the search object if we return
                // That should free all added objects, so this should be safe to do
                if (!parent_object) goto cleanup;
            }
            target_object = parent_object;
        }
    } while (token != NULL);

    free(mutable_path);
    return target_object;

    cleanup:
    free(mutable_path);
    return NULL;
}

static cJSON *setup_telemetry_object(IotclMessageHandle message) {
    IotclConfig *config = iotcl_get_config();
    if (!config) return NULL;
    if (!message) return NULL;

    cJSON *telemetry_object = cJSON_CreateObject();
    if (!telemetry_object) return NULL;
    cJSON_AddStringToObject(telemetry_object, "id", config->device.duid);
    cJSON_AddStringToObject(telemetry_object, "tg", "");
    cJSON *data_array = cJSON_AddArrayToObject(telemetry_object, "d");
    if (!data_array) goto cleanup_to;
    if (!cJSON_AddItemToArray(message->telemetry_data_array, telemetry_object)) goto cleanup_da;

    // setup the actual telemetry object to be used in subsequent calls
    message->current_telemetry_object = cJSON_CreateObject();
    if (!message->current_telemetry_object) goto cleanup_da;

    if (!cJSON_AddItemToArray(data_array, message->current_telemetry_object)) goto cleanup_cto;

    return telemetry_object; // object inside the "d" array of the the root object

    cleanup_cto:
    cJSON_free(message->current_telemetry_object);

    cleanup_da:
    cJSON_free(data_array);

    cleanup_to:
    cJSON_free(telemetry_object);

    return NULL;
}

IotclMessageHandle iotcl_telemetry_create() {
    cJSON *sdk_array = NULL;
    IotclConfig *config = iotcl_get_config();
    if (!config) return NULL;
    if (!config->telemetry.dtg) return NULL;
    struct IotclMessageHandleTag *msg =
            (struct IotclMessageHandleTag *) calloc(sizeof(struct IotclMessageHandleTag), 1);

    if (!msg) return NULL;

    msg->root_value = cJSON_CreateObject();

    if (!msg->root_value) goto cleanup;

    if (!cJSON_AddStringToObject(msg->root_value, "cpid", config->device.cpid)) goto cleanup;
    if (!cJSON_AddStringToObject(msg->root_value, "dtg", config->telemetry.dtg)) goto cleanup;
    if (!cJSON_AddNumberToObject(msg->root_value, "mt", 0)) goto cleanup; // telemetry message type (zero)
    sdk_array = cJSON_AddObjectToObject(msg->root_value, "sdk");
    if (!sdk_array) goto cleanup;
    if (!cJSON_AddStringToObject(sdk_array, "l", CONFIG_IOTCONNECT_SDK_NAME)) goto cleanup_array;
    if (!cJSON_AddStringToObject(sdk_array, "v", CONFIG_IOTCONNECT_SDK_VERSION)) goto cleanup_array;
    if (!cJSON_AddStringToObject(sdk_array, "e", config->device.env)) goto cleanup_array;

    msg->telemetry_data_array = cJSON_AddArrayToObject(msg->root_value, "d");

    if (!msg->telemetry_data_array) goto cleanup_array;

    return msg;

    cleanup_array:
    cJSON_free(sdk_array);
    cleanup:
    cJSON_free(msg);
    return NULL;
}

bool iotcl_telemetry_add_with_epoch_time(IotclMessageHandle message, time_t time) {
    if (!message) return false;
    cJSON *telemetry_object = setup_telemetry_object(message);
    cJSON_AddNumberToObject(telemetry_object, "ts", time);
    if (!cJSON_HasObjectItem(message->root_value, "ts")) {
        cJSON_AddNumberToObject(message->root_value, "ts", time);
    }
    return true;
}

bool iotcl_telemetry_add_with_iso_time(IotclMessageHandle message, const char *time) {
    if (!message) return false;
    cJSON *const telemetry_object = setup_telemetry_object(message);
    if (!telemetry_object) return false;
    if (!cJSON_AddStringToObject(telemetry_object, "dt", time)) return false;
    if (!cJSON_HasObjectItem(message->root_value, "t")) {
        if (!cJSON_AddStringToObject(message->root_value, "t", time)) return false;
    }
    return true;
}

bool iotcl_telemetry_set_number(IotclMessageHandle message, const char *path, double value) {
    if (!message) return false;
    if (NULL == message->current_telemetry_object) {
        if (!iotcl_telemetry_add_with_iso_time(message, iotcl_iso_timestamp_now())) return false;
    }
    char *leaf_name = NULL;
    cJSON *target = json_object_dotset_locate(message->current_telemetry_object, &leaf_name, path);
    if (!target) goto cleanup; // out of memory

    if (!cJSON_AddNumberToObject(target, leaf_name, value)) goto cleanup_tgt;
    free(leaf_name);
    return true;

    cleanup_tgt:
    cJSON_free(target);

    cleanup:
    free(leaf_name);
    return false;
}

bool iotcl_telemetry_set_bool(IotclMessageHandle message, const char *path, bool value) {
    if (!message) return false;
    if (NULL == message->current_telemetry_object) {
        if (!iotcl_telemetry_add_with_iso_time(message, iotcl_iso_timestamp_now())) return false;
    }
    char *leaf_name = NULL;
    cJSON *target = json_object_dotset_locate(message->current_telemetry_object, &leaf_name, path);
    if (!target) goto cleanup; // out of memory

    if (!cJSON_AddBoolToObject(target, leaf_name, value)) goto cleanup_tgt;
    free(leaf_name);
    return true;

    cleanup_tgt:
    cJSON_free(target);

    cleanup:
    free(leaf_name);
    return false;
}

bool iotcl_telemetry_set_string(IotclMessageHandle message, const char *path, const char *value) {
    if (!message) return false;
    if (NULL == message->current_telemetry_object) {
        iotcl_telemetry_add_with_iso_time(message, iotcl_iso_timestamp_now());
    }
    char *leaf_name = NULL;
    cJSON *const target = json_object_dotset_locate(message->current_telemetry_object, &leaf_name, path);
    if (!target) goto cleanup; // out of memory

    if (!cJSON_AddStringToObject(target, leaf_name, value)) goto cleanup_tgt;
    free(leaf_name);
    return true;

    cleanup_tgt:
    cJSON_free(target);

    cleanup:
    free(leaf_name);
    return false;
}

bool iotcl_telemetry_set_null(IotclMessageHandle message, const char *path) {
    if (!message) return false;
    if (NULL == message->current_telemetry_object) {
        iotcl_telemetry_add_with_iso_time(message, iotcl_iso_timestamp_now());
    }
    char *leaf_name = NULL;
    cJSON *const target = json_object_dotset_locate(message->current_telemetry_object, &leaf_name, path);
    if (!target) goto cleanup; // out of memory

    if (!cJSON_AddNullToObject(target, leaf_name)) goto cleanup_tgt;
    free(leaf_name);
    return true;

    cleanup_tgt:
    cJSON_free(target);

    cleanup:
    free(leaf_name);
    return false;
}

const char *iotcl_create_serialized_string(IotclMessageHandle message, bool pretty) {
    if (!message) return NULL;
    if (!message->root_value) return NULL;
    if (pretty) {
        return cJSON_Print(message->root_value);
    } else {
        return cJSON_PrintUnformatted(message->root_value);
    }
}

void iotcl_destroy_serialized(const char *serialized_string) {
    cJSON_free((char *) serialized_string);
}

void iotcl_telemetry_destroy(IotclMessageHandle message) {
    if (message) {
        cJSON_Delete(message->root_value);
    }
    free(message);
}
