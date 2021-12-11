/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

#ifndef IOTCONNECT_TELEMETRY_H
#define IOTCONNECT_TELEMETRY_H

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    // Can be copied at the device page. Can be obtained via REST Sync call.
    // Device template GUID required to send telemetry data.
    const char *dtg;
} IotclTelemetryConfig;


typedef struct IotclMessageHandleTag *IotclMessageHandle;

/*
 * Create a message handle given IoTConnect configuration.
 * This handle can be used to add data to the message.
 * The handle cannot be re-used and should be destroyed to free up resources, once the message is sent.
 */
IotclMessageHandle iotcl_telemetry_create();

/*
 * Destroys the IoTConnect message handle.
 */
void iotcl_telemetry_destroy(IotclMessageHandle message);

/*
 * Creates a new telemetry data set with a given timestamp in ISO 8601 Date and time UTC format:
 * eg. "2020-03-31T21:18:05.000Z"
 * The user can omit calling this function if single data point would be sent.
 * In that case, the current system timestamp will be used
 * @see iotcl_iso_timestamp_now, iotcl_to_iso_timestamp
 */
bool iotcl_telemetry_add_with_iso_time(IotclMessageHandle message, const char *time);

/*
 * Creates a new telemetry data set with a given timestamp in unix time format.
 * The user can omit calling this function if single data point would be sent.
 * In that case, the current system timestamp will be used.
 * NOTE: WARNING - IoTConnect may not support epoch timetamps in past or future implementations
 * @see iotcl_iso_timestamp_now, iotcl_to_iso_timestamp
 */
bool iotcl_telemetry_add_with_epoch_time(IotclMessageHandle message, time_t time);

/*
 * Sets a number value in in the last created data set. Creates one with current time if none were created
 * previously with TelemetryAddWith*Time() call.
 * Path is an (optional) dotted notation that can be used to set values in nested objects.
 */
bool iotcl_telemetry_set_number(IotclMessageHandle message, const char *path, double value);

/*
 * Sets a string value in in the last created data set. Creates one with current time if none were created
 * previously with TelemetryAddWith*Time() call.
 * Path is an (optional) dotted notation that can be used to set values in nested objects.
 */
bool iotcl_telemetry_set_string(IotclMessageHandle message, const char *path, const char *value);

/*
 * Sets a boolean value in in the last created data set. Creates one with current time if none were created
 * previously with TelemetryAddWith*Time() call.
 * Path is an (optional) dotted notation that can be used to set values in nested objects.
 */
bool iotcl_telemetry_set_bool(IotclMessageHandle message, const char *path, bool value);

/*
 * Sets the field value to null in the last created data set. Creates one with current time if none were created
 * previously with TelemetryAddWith*Time() call.
 * Null values can be used to indicate lack of avaialable data to the cloud.
 * Path is an (optional) dotted notation that can be used to set values in nested objects.
 */
bool iotcl_telemetry_set_null(IotclMessageHandle message, const char *path);

const char *iotcl_create_serialized_string(IotclMessageHandle message, bool pretty);

void iotcl_destroy_serialized(const char *serialized_string);

#ifdef __cplusplus
}
#endif

#endif /* IOTCONNECT_TELEMETRY_H */
