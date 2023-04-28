
/* Copyright (C) 2020 Avnet - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Authors: Nikola Markovic <nikola.markovic@avnet.com> et al.
 */

#ifndef IOTCONNECT_LIB_CONFIG_H
#define IOTCONNECT_LIB_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_IOTCONNECT_ENV_MAX_LEN 20

#define CONFIG_IOTCONNECT_CLIENTID_MAX_LEN 128

#define CONFIG_IOTCONNECT_DUID_MAX_LEN 64

// IoTHub max device id is 128, which is "<CPID>-<DUID>" (with a dash)
#define CONFIG_IOTCONNECT_CPID_MAX_LEN (CONFIG_IOTCONNECT_CLIENTID_MAX_LEN - 1 - CONFIG_IOTCONNECT_DUID_MAX_LEN)

#ifdef CONFIG_IOTCONNECT_LIB_LOG
#define IOTCL_LOG CONFIG_IOTCONNECT_LIB_LOG
#elif !defined(IOTCL_LOG)
#define IOTCL_LOG (void)// noop
#endif

#ifdef CONFIG_IOTCONNECT_LIB_LOG_NEWLINE
#define IOTCL_NL CONFIG_IOTCONNECT_LIB_LOG_NEWLINE
#elif !defined(IOTCL_NL)
#define IOTCL_NL // noop
#endif

#ifndef CONFIG_IOTCONNECT_SDK_NAME
#define CONFIG_IOTCONNECT_SDK_NAME "M_C"
#endif

#ifndef CONFIG_IOTCONNECT_SDK_VERSION
#define CONFIG_IOTCONNECT_SDK_VERSION "2.0"
#endif

#ifdef __cplusplus
}
#endif

#endif //IOTCONNECT_LIB_CONFIG_H