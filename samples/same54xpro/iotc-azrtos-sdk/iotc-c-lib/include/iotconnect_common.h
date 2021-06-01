/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

#ifndef IOTCONNECT_COMMON_H
#define IOTCONNECT_COMMON_H


#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

char *iotcl_strdup(const char *str);

// NOTE: This function is not thread-safe
const char *iotcl_to_iso_timestamp(time_t timestamp);

// NOTE: This function is not thread-safe
const char *iotcl_iso_timestamp_now();

// Internal function
void iotcl_oom_error();

#ifdef __cplusplus
}
#endif
#endif //IOTCONNECT_LIB_H
