/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */
#include <stdlib.h>
#include <string.h>
#include "iotconnect_common.h"

static char timebuf[sizeof "2011-10-08T07:07:01.000Z"];

static const char *to_iso_timestamp(time_t *timestamp) {
    time_t ts = timestamp ? *timestamp : time(NULL);
    strftime(timebuf, (sizeof timebuf), "%Y-%m-%dT%H:%M:%S.000Z", gmtime(&ts));
    return timebuf;
}

const char *iotcl_to_iso_timestamp(time_t timestamp) {
    return to_iso_timestamp(&timestamp);
}

const char *iotcl_iso_timestamp_now() {
    return to_iso_timestamp(NULL);
}

char *iotcl_strdup(const char *str) {
    if (!str) {
        return NULL;
    }
    size_t size = strlen(str) + 1;
    char *p = (char *) malloc(size);
    if (p != NULL) {
        memcpy(p, str, size);
    }
    return p;
}