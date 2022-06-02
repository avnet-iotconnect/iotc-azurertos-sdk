/* include/TO_cfg.h.  Generated from TO_cfg.h.in by configure.  */
/*
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Copyright (C) 2016-2018 Trusted Objects. All rights reserved.
 */

/**
 * @file TO_cfg.h
 * @brief This file provides a way to configure libTO build.
 *
 * Please read the library configuration documentation chapter before modifying
 * this file.
 */

#ifndef _TO_CFG_H_
#define _TO_CFG_H_

/*
 * ------------------------
 * Global settings
 * ------------------------
 */

#ifndef TO_LOG_LEVEL_MAX
/* log level max */
#define TO_LOG_LEVEL_MAX 2
#endif

/*
 * Endianness settings
 * By default runtime detection is performed if none of the options below is
 * enabled.
 */
#ifndef TO_BIG_ENDIAN
/* Force big endian */
/* #undef TO_BIG_ENDIAN */
#endif
#ifndef TO_LITTLE_ENDIAN
/* (or) Force little endian */
/* #undef TO_LITTLE_ENDIAN */
#endif

/*
 * ------------------------
 * Features macro. settings
 * ------------------------
 */

#ifndef TO_DISABLE_ADMIN
/* disable administration APIs */
/* #undef TO_DISABLE_ADMIN */
#endif

#ifndef TO_DISABLE_AES_ENCRYPT
/* disable AES encrypt APIs */
/* #undef TO_DISABLE_AES_ENCRYPT */
#endif

#ifndef TO_DISABLE_API_GET_RANDOM
/* disable get_random API */
/* #undef TO_DISABLE_API_GET_RANDOM */
#endif

#ifndef TO_DISABLE_CAPI
/* disable cumulative APIs */
/* #undef TO_DISABLE_CAPI */
#endif

#ifndef TO_DISABLE_CERT_MGMT
/* disable certificates APIs */
/* #undef TO_DISABLE_CERT_MGMT */
#endif

#ifndef TO_DISABLE_CMAC
/* disable CMAC APIs */
#define TO_DISABLE_CMAC 1
#endif

#ifndef TO_DISABLE_FINGERPRINT
/* disable fingerprint APIs */
#define TO_DISABLE_FINGERPRINT 1
#endif

#ifndef TO_DISABLE_HMAC
/* disable HMAC APIs */
/* #undef TO_DISABLE_HMAC */
#endif

#ifndef TO_DISABLE_KEYS_MGMT
/* disable keys APIs */
/* #undef TO_DISABLE_KEYS_MGMT */
#endif

#ifndef TO_DISABLE_LORA
/* disable LoRa APIs */
#define TO_DISABLE_LORA 1
#endif

#ifndef TO_DISABLE_LORA_OPTIMIZED
/* disable LoRa optimized APIs */
#define TO_DISABLE_LORA_OPTIMIZED 1
#endif

#ifndef TO_DISABLE_NVM
/* disable NVM storage APIs */
#define TO_DISABLE_NVM 1
#endif

#ifndef TO_DISABLE_SEC_MSG
/* disable secure messaging APIs */
#define TO_DISABLE_SEC_MSG 1
#endif

#ifndef TO_DISABLE_SHA256
/* disable SHA256 APIs */
/* #undef TO_DISABLE_SHA256 */
#endif

#ifndef TO_DISABLE_SIGNING
/* disable signing APIs */
/* #undef TO_DISABLE_SIGNING */
#endif

#ifndef TO_DISABLE_STATUS_PIO_CONFIG
/* disable Secure Element status notification PIO configuration */
#define TO_DISABLE_STATUS_PIO_CONFIG 1
#endif

#ifndef TO_DISABLE_TLS
/* disable TLS */
/* #undef TO_DISABLE_TLS */
#endif

#ifndef TO_DISABLE_TLS_OPTIMIZED
/* disable TLS optimized APIs */
/* #undef TO_DISABLE_TLS_OPTIMIZED */
#endif

#ifndef TO_DISABLE_TO_INFO
/* disable Secure Element info APIs */
/* #undef TO_DISABLE_TO_INFO */
#endif

#ifndef TO_DISABLE_LOADER
/* disable Secure Element secure bootloader APIs */
#define TO_DISABLE_LOADER 1
#endif

#ifndef TO_DISABLE_MEASURE
/* disable measured boot APIs */
#define TO_DISABLE_MEASURE 1
#endif

#ifndef TO_DISABLE_IPP
/* disable IPP */
#define TO_DISABLE_IPP 1
#endif

/*
 * -----------------------------
 * Features microscopic settings
 * -----------------------------
 */

/*
 * To disable (do not build) a specific API
 * Replace <API_NAME> by the uppercase API name.
 */
//#define TO_DISABLE_API_<API_NAME>
#ifdef TO_DISABLE_CAPI
#ifndef TO_DISABLE_API_HELPER_TLS_SECURE_MESSAGE
#define TO_DISABLE_API_HELPER_TLS_SECURE_MESSAGE 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_UNSECURE_MESSAGE
#define TO_DISABLE_API_HELPER_TLS_UNSECURE_MESSAGE 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_SECURE_PAYLOAD_CBC
#define TO_DISABLE_API_HELPER_TLS_SECURE_PAYLOAD_CBC 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_SECURE_PAYLOAD_AEAD
#define TO_DISABLE_API_HELPER_TLS_SECURE_PAYLOAD_AEAD 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_UNSECURE_PAYLOAD_CBC
#define TO_DISABLE_API_HELPER_TLS_UNSECURE_PAYLOAD_CBC 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_UNSECURE_PAYLOAD_AEAD
#define TO_DISABLE_API_HELPER_TLS_UNSECURE_PAYLOAD_AEAD 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_HANDLE_SERVER_CERTIFICATE
#define TO_DISABLE_API_HELPER_TLS_HANDLE_SERVER_CERTIFICATE 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_HANDLE_SERVER_KEY_EXCHANGE
#define TO_DISABLE_API_HELPER_TLS_HANDLE_SERVER_KEY_EXCHANGE 1
#endif
#ifndef TO_DISABLE_API_HELPER_TLS_GET_CERTIFICATE
#define TO_DISABLE_API_HELPER_TLS_GET_CERTIFICATE 1
#endif
#ifndef TO_DISABLE_API_SECURE_MESSAGE_INIT_UPDATE_FINAL
#define TO_DISABLE_API_SECURE_MESSAGE_INIT_UPDATE_FINAL 1
#endif
#ifndef TO_DISABLE_API_UNSECURE_MESSAGE_INIT_UPDATE_FINAL
#define TO_DISABLE_API_UNSECURE_MESSAGE_INIT_UPDATE_FINAL 1
#endif
#ifndef TO_DISABLE_API_SECURE_PAYLOAD_INIT_UPDATE_FINAL
#define TO_DISABLE_API_SECURE_PAYLOAD_INIT_UPDATE_FINAL 1
#endif
#ifndef TO_DISABLE_API_UNSECURE_PAYLOAD_INIT_UPDATE_FINAL
#define TO_DISABLE_API_UNSECURE_PAYLOAD_INIT_UPDATE_FINAL 1
#endif
#ifndef TO_DISABLE_API_VERIFY_CHAIN_CERTIFICATE_AND_STORE
#define TO_DISABLE_API_VERIFY_CHAIN_CERTIFICATE_AND_STORE 1
#endif
#ifndef TO_DISABLE_API_VERIFY_CHAIN_CA_CERTIFICATE_AND_STORE
#define TO_DISABLE_API_VERIFY_CHAIN_CA_CERTIFICATE_AND_STORE 1
#endif
#ifndef TO_DISABLE_API_SET_CERTIFICATE_X509_INIT_UPDATE_FINAL
#define TO_DISABLE_API_SET_CERTIFICATE_X509_INIT_UPDATE_FINAL 1
#endif
#endif

#endif /* _TO_CFG_H_ */