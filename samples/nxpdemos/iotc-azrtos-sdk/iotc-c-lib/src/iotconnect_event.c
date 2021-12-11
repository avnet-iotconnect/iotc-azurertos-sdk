/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>

#include "iotconnect_common.h"
#include "iotconnect_lib.h"

#define CJSON_ADD_ITEM_HAS_RETURN \
    (CJSON_VERSION_MAJOR * 10000 + CJSON_VERSION_MINOR * 100 + CJSON_VERSION_PATCH >= 10713)

#if !CJSON_ADD_ITEM_HAS_RETURN
#error "cJSON version must be 1.7.13 or newer"
#endif

struct IotclEventDataTag {
    cJSON *data;
    cJSON *root;
    IotConnectEventType type;
};

/*
Conversion of boolean to IOT 2.0 specification messages:

For commands:
Table 15 [Possible values for st]
4 Command Failed with some reason
6 Executed successfully

For OTA:
Table 16 [Possible values for st]
4 Firmware command Failed with some reason
7 Firmware command executed successfully
*/
static int to_ack_status(bool success, IotConnectEventType type) {
    int status = 4; // default is "failure"
    if (success == true) {
        switch (type) {
            case DEVICE_COMMAND:
                status = 6;
                break;
            case DEVICE_OTA:
                status = 7;
                break;
            default:; // Can't do more than assume failure if unknown type is used.
        }
    }
    return status;
}


static bool iotc_process_callback(struct IotclEventDataTag *eventData) {
    if (!eventData) return false;

    IotclConfig *config = iotcl_get_config();
    if (!config) return false;

    if (config->event_functions.msg_cb) {
        config->event_functions.msg_cb(eventData, eventData->type);
    }
    switch (eventData->type) {
        case DEVICE_COMMAND:
            if (config->event_functions.cmd_cb) {
                config->event_functions.cmd_cb(eventData);
            }
            break;
        case DEVICE_OTA:
            if (config->event_functions.ota_cb) {
                config->event_functions.ota_cb(eventData);
            }
            break;
        default:
            break;
    }

    return true;
}


/************************ CJSON IMPLEMENTATION **************************/
static inline bool is_valid_string(const cJSON *json) {
    return (NULL != json && cJSON_IsString(json) && json->valuestring != NULL);
}

bool iotcl_process_event(const char *event) {
    cJSON *root = cJSON_Parse(event);
    if (!root) return false;

    { // scope out the on-the fly varialble declarations for cleanup jump
        // root object should only have cmdType
        cJSON *j_type = cJSON_GetObjectItemCaseSensitive(root, "cmdType");
        if (!is_valid_string(j_type)) goto cleanup;

        cJSON *j_ack_id = NULL;
        cJSON *data = NULL; // data should have ackId
        if (!is_valid_string(j_ack_id)) {
            data = cJSON_GetObjectItemCaseSensitive(root, "data");
            if (!data) goto cleanup;
            j_ack_id = cJSON_GetObjectItemCaseSensitive(data, "ackId");
            if (!is_valid_string(j_ack_id)) goto cleanup;
        }

        if (4 != strlen(j_type->valuestring)) {
            // Don't know how to parse it then...
            goto cleanup;
        }

        IotConnectEventType type = (IotConnectEventType) strtol(&j_type->valuestring[2], NULL, 16);

        if (type < DEVICE_COMMAND) {
            goto cleanup;
        }

        // In case we have a supported command. Do some checks before allowing further processing of acks
        // NOTE: "i" in cpId is lower case, but per spec it's supposed to be in upper case
        if (type == DEVICE_COMMAND || type == DEVICE_OTA) {
            if (
                    !is_valid_string(cJSON_GetObjectItem(data, "cpid"))
                    || !is_valid_string(cJSON_GetObjectItemCaseSensitive(data, "uniqueId"))
                    ) {
                goto cleanup;
            }
        }

        struct IotclEventDataTag *eventData = (struct IotclEventDataTag *) calloc(
                sizeof(struct IotclEventDataTag), 1);
        if (NULL == eventData) goto cleanup;

        eventData->root = root;
        eventData->data = data;
        eventData->type = type;
        return iotc_process_callback(eventData);
    }

    cleanup:
    if (root) {
        cJSON_free(root);
    }
    return false;

}

char *iotcl_clone_command(IotclEventData data) {
    cJSON *command = cJSON_GetObjectItemCaseSensitive(data->data, "command");
    if (NULL == command || !is_valid_string(command)) {
        return NULL;
    }

    return iotcl_strdup(command->valuestring);
}

char *iotcl_clone_download_url(IotclEventData data, size_t index) {
    cJSON *urls = cJSON_GetObjectItemCaseSensitive(data->data, "urls");
    if (NULL == urls || !cJSON_IsArray(urls)) {
        return NULL;
    }
    if ((size_t) cJSON_GetArraySize(urls) > index) {
        cJSON *url = cJSON_GetArrayItem(urls, index);
        if (is_valid_string(url)) {
            return iotcl_strdup(url->valuestring);
        } else if (cJSON_IsObject(url)) {
            cJSON *url_str = cJSON_GetObjectItem(url, "url");
            if (is_valid_string(url_str)) {
                return iotcl_strdup(url_str->valuestring);
            }
        }
    }
    return NULL;
}


char *iotcl_clone_sw_version(IotclEventData data) {
    cJSON *ver = cJSON_GetObjectItemCaseSensitive(data->data, "ver");
    if (cJSON_IsObject(ver)) {
        cJSON *sw = cJSON_GetObjectItem(ver, "sw");
        if (is_valid_string(sw)) {
            return iotcl_strdup(sw->valuestring);
        }
    }
    return NULL;
}

char *iotcl_clone_hw_version(IotclEventData data) {
    cJSON *ver = cJSON_GetObjectItemCaseSensitive(data->data, "ver");
    if (cJSON_IsObject(ver)) {
        cJSON *sw = cJSON_GetObjectItem(ver, "hw");
        if (is_valid_string(sw)) {
            return iotcl_strdup(sw->valuestring);
        }
    }
    return NULL;
}

static char *create_ack(
        bool success,
        const char *message,
        IotConnectEventType message_type,
        const char *ack_id) {

    char *result = NULL;

    IotclConfig *config = iotcl_get_config();

    if (!config) {
        return NULL;
    }

    cJSON *ack_json = cJSON_CreateObject();

    if (ack_json == NULL) {
        return NULL;
    }

    // message type 5 in response is the command response. Type 11 is OTA response.
    if (!cJSON_AddNumberToObject(ack_json, "mt", message_type == DEVICE_COMMAND ? 5 : 11)) goto cleanup;
    if (!cJSON_AddStringToObject(ack_json, "t", iotcl_iso_timestamp_now())) goto cleanup;

    if (!cJSON_AddStringToObject(ack_json, "uniqueId", config->device.duid)) goto cleanup;
    if (!cJSON_AddStringToObject(ack_json, "cpId", config->device.cpid)) goto cleanup;

    {
        cJSON *sdk_info = cJSON_CreateObject();
        if (NULL == sdk_info) {
            return NULL;
        }
        if (!cJSON_AddItemToObject(ack_json, "sdk", sdk_info)) {
            cJSON_Delete(sdk_info);
            goto cleanup;
        }
        if (!cJSON_AddStringToObject(sdk_info, "l", CONFIG_IOTCONNECT_SDK_NAME)) goto cleanup;
        if (!cJSON_AddStringToObject(sdk_info, "v", CONFIG_IOTCONNECT_SDK_VERSION)) goto cleanup;
        if (!cJSON_AddStringToObject(sdk_info, "e", config->device.env)) goto cleanup;
    }

    {
        cJSON *ack_data = cJSON_CreateObject();
        if (NULL == ack_data) goto cleanup;
        if (!cJSON_AddItemToObject(ack_json, "d", ack_data)) {
            cJSON_Delete(ack_data);
            goto cleanup;
        }
        if (!cJSON_AddStringToObject(ack_data, "ackId", ack_id)) goto cleanup;
        if (!cJSON_AddStringToObject(ack_data, "msg", message ? message : "")) goto cleanup;
        if (!cJSON_AddNumberToObject(ack_data, "st", to_ack_status(success, message_type))) goto cleanup;
    }

    result = cJSON_PrintUnformatted(ack_json);

    // fall through
    cleanup:
    cJSON_Delete(ack_json);
    return result;
}

char *iotcl_create_ack_string_and_destroy_event(
        IotclEventData data,
        bool success,
        const char *message
) {
    if (!data) return NULL;
    // alrwady checked that ack ID is valid in the messages
    char *ack_id = cJSON_GetObjectItemCaseSensitive(data->data, "ackId")->valuestring;
    char *ret = create_ack(success, message, data->type, ack_id);
    iotcl_destroy_event(data);
    return ret;
}

char *iotcl_create_ota_ack_response(
        const char *ota_ack_id,
        bool success,
        const char *message
) {
    char *ret = create_ack(success, message, DEVICE_OTA, ota_ack_id);
    return ret;
}

void iotcl_destroy_event(IotclEventData data) {
    cJSON_Delete(data->root);
    free(data);
}

