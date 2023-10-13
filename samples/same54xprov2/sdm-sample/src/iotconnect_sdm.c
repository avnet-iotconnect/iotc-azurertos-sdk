//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 10/2/23.
//

#include "atcacert/atcacert.h"
#include "atcacert/atcacert_def.h"
#include "atcacert/atcacert_pem.h"
#include "atcacert/atcacert_client.h"

// TODO remove atca* dependency and migrate it into sdm app

#include "tng_root_cert.h"
#include "tngtls_cert_def_1_signer.h"
#include "tngtls_cert_def_3_device.h"

#include "iotconnect_certs.h"
#include "azrtos_https_client.h"
#include "iotconnect_sdm.h"

#include "cJSON.h"
#include "iotconnect_common.h" // for strdup

//#define SDM_HOST "uatpartnerservice.iotconnect.io"
//#define SDM_HOST "avnetiotpartnerprogram.azure-api.net"
#define SDM_HOST "192.168.38.212"

// this path is for "uatpartnerservice.iotconnect.io"
#define SDM_API_PREFIX "/uat-device-auth/api/v1"
//#define SDM_API_PREFIX "/uat-device-auth/api/v1"
#define SDM_CHALLENGE_PATH  SDM_API_PREFIX "/auth/challenge"
#define SDM_RESPONSE_PATH   SDM_API_PREFIX "/auth/response"
#define SDM_INFO_PATH       SDM_API_PREFIX "/info"

#define SDM_TOKEN_PREFIX "Bearer "

#define CERT_BUF_MAX_SIZE 1024
#define HEX_PREFIX "hex."

extern ATCAIfaceCfg atecc608_0_init_data;

static char* sdm_challenge;
static char* sdm_alias;
static char* sdm_token;

static int sdm_atca_get_serial_as_string(char* serial_buffer);
//static void sdm_atca_print_root_ca_cert(void);
static int sdm_atca_print_signer_cert(void);
static int sdm_atca_print_device_cert(void);
static bool sdm_parse_connection_info(SdmInfoResponse* sir, const char* response, size_t conn_index);

static void sdm_initialize_request(IotConnectHttpRequest* r,
        IotConnectAzrtosConfig* azrtos_config,
        const char* path
) {
    memset(r, 0, sizeof(IotConnectHttpRequest));
    r->azrtos_config = azrtos_config;
	r->host_name = (char *) SDM_HOST;
	r->resource = (char *) path;
	//r->tls_cert = (unsigned char*) IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2;
	//r->tls_cert_len = IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2_SIZE;
    r->tls_cert_len = 8080; // port specified in HTTP test mode
}

static void add_request_header(IotConnectHttpRequest* r,
        const char* name,
        const char* value
) {
    size_t idx = r->request_headers_size;
    r->request_headers[idx].name = name;
    r->request_headers[idx].value = value;
    r->request_headers_size++;
}

static char *clone_header_str(CHAR *str, UINT len) {
    char *ret = malloc(len + 1);

    if (NULL == ret) {
        printf("SDM: OOM Error while cloning response headers\r\n");
        return NULL;
    }
    strncpy(ret, str, len);
    ret[len] = 0; // terminate the copied string at end

    return ret;
}

static VOID on_rsp_header_record_token(
    CHAR *field_name, UINT field_name_length, CHAR *field_value, UINT field_value_length
) {
    if (0 == strncmp("token", field_name, field_name_length)) {
        sdm_token = clone_header_str(field_value, field_value_length);
    }
    // else ignore the header
}

static VOID on_rsp_header_record_challenge_and_alias(
    CHAR *field_name, UINT field_name_length, CHAR *field_value, UINT field_value_length
) {
    if (0 == strncmp("challenge", field_name, field_name_length)) {
        sdm_challenge = clone_header_str(field_value, field_value_length);
    } else if (0 == strncmp("alias", field_name, field_name_length)) {
        sdm_alias = clone_header_str(field_value, field_value_length);
    }
    // else ignore the header
}

bool iotc_sdm_test(IotConnectAzrtosConfig *azrtos_config, SdmInfoResponse *sir) {
    UINT http_status;
    ATCA_STATUS atca_status;
    int ret = true;
	IotConnectHttpRequest http_req = {0};
    uint8_t sha256_digest[IOTC_SHA256_HASH_SIZE];
    uint8_t signature[IOTC_256_BIT_SIGNATURE_SIZE];
    char signature_str[sizeof(HEX_PREFIX) + IOTC_256_BIT_SIGNATURE_SIZE * 2];
	http_req.azrtos_config = azrtos_config;

    sdm_challenge = NULL;
    sdm_alias = NULL;
    sdm_token = NULL;
    memset(sir, 0, sizeof(SdmInfoResponse));


    char* serial_str = malloc(2 * ATCA_SERIAL_NUM_SIZE + 1);
    sdm_atca_get_serial_as_string(serial_str);
    printf("ATECC608 Serial: %s\r\n", serial_str);

    
    printf("==== Device certificate chain. Device cert, followed intermediate cert:\r\n");
    sdm_atca_print_device_cert();
    sdm_atca_print_signer_cert();
    // sdm_atca_print_root_ca_cert();
    printf("==== END Device certificate chain.\r\n");

	sdm_initialize_request(&http_req, azrtos_config, SDM_CHALLENGE_PATH);
    add_request_header(&http_req, "KeystoreId", serial_str);

    http_req.custom_response_header_cb = on_rsp_header_record_challenge_and_alias;

    http_status = iotconnect_https_request(&http_req);

    if (http_status != NX_SUCCESS) {
        printf("SDM: Auth request error code: 0x%x data: %s\r\n", http_status, http_req.response);
        goto cleanup;
    }

    printf("----\r\n");
    if (NULL != sdm_challenge) {
        printf("SDM: challenge: %s\r\n", sdm_challenge);
    } else {
        printf("SDM: Failed to get challenge\r\n");
        ret = false;
    }

    if (NULL != sdm_alias) {
        printf("SDM: alias: %s\r\n", sdm_alias);
    } else {
        printf("SDM: Failed to get alias\r\n");
        ret = false;
    }
    printf("----\r\n");

    if (false == ret) {
        goto cleanup;
    }

    atca_status = atcab_init(&atecc608_0_init_data);
    if (atca_status != ATCA_SUCCESS) {
        printf("SDM: atcab_init() failed: 0x%x\r\n", atca_status);
        ret = false;
        goto cleanup;
    }

    atca_status = atcab_sha((uint16_t)strlen(sdm_challenge), (uint8_t*)sdm_challenge, sha256_digest);
    if (atca_status != ATCA_SUCCESS) {
        printf("SDM: atcab_sha() failed: 0x%x\r\n", atca_status);
        ret = false;
        goto cleanup;
    }


    atca_status = atcab_sign(g_tngtls_cert_def_3_device.private_key_slot, sha256_digest, signature);
    if (atca_status != ATCA_SUCCESS) {
        printf("SDM: atcab_sign() failed: 0x%x\r\n", atca_status);
        ret = false;
        goto cleanup;
    }

    size_t signature_str_size = sizeof(signature_str) - strlen(HEX_PREFIX); // when placing after "hex." ...
    atca_status = atcab_bin2hex_(signature, sizeof(signature), &signature_str[strlen(HEX_PREFIX)], &signature_str_size, false, false, false);
    if (atca_status != ATCA_SUCCESS) {
        printf("SDM: atcab_bin2hex_() failed: 0x%x\r\n", atca_status);
        ret = false;
        goto cleanup_atca;
    }
    memcpy(signature_str, HEX_PREFIX, strlen(HEX_PREFIX));
    printf("signature: %s\r\n", signature_str);

    free(sdm_alias); // don't need anymore
    sdm_alias = NULL;

    sdm_initialize_request(&http_req, azrtos_config, SDM_RESPONSE_PATH);
    add_request_header(&http_req, "Challenge", sdm_challenge);
    add_request_header(&http_req, "Response", signature_str);

    http_req.custom_response_header_cb = on_rsp_header_record_token;

    http_status = iotconnect_https_request(&http_req);

    if (http_status != NX_SUCCESS) {
        printf("SDM: Auth response error code: 0x%x data: %s\r\n", http_status, http_req.response);
        ret = false;
        goto cleanup_atca;
    }

    free(sdm_challenge);
    sdm_challenge = NULL; // don't need anymore

    if (NULL == sdm_token) {
        printf("SDM: Failed to get auth token\r\n");
        ret = false;
        goto cleanup_atca;
    }

    printf("Token: %s\r\n", sdm_token);

    { // scope block
        char bearer_str[strlen(sdm_token) + sizeof(SDM_TOKEN_PREFIX)];
        strcpy(bearer_str, SDM_TOKEN_PREFIX);
        strcat(bearer_str, sdm_token);

        sdm_initialize_request(&http_req, azrtos_config, SDM_INFO_PATH);
        add_request_header(&http_req, "Authorization", bearer_str);

        http_req.custom_response_header_cb = on_rsp_header_record_token;

        http_status = iotconnect_https_request(&http_req);

        if (http_status != NX_SUCCESS) {
            printf("SDM: Info error code: 0x%x data: %s\r\n", http_status, http_req.response);
            ret = false;
            goto cleanup_atca;
        }
     }

    printf("Raw Info: %s\r\n", http_req.response);

    if (sdm_parse_connection_info(sir, http_req.response, 0)) {
        printf("Info duid:%s cpid:%s env:%s\r\n", sir->duid, sir->cpid, sir->env);
    }

cleanup_atca:
    atcab_release();

cleanup:
    if(NULL != sdm_token) {
        free(sdm_token);
        sdm_token = NULL;
    }
    if(NULL != sdm_challenge) {
        free(sdm_challenge);
        sdm_challenge = NULL;
    }
    if (NULL != sdm_alias) {
        free(sdm_alias);
        sdm_alias = NULL;
    }
    if (NULL != sdm_token) {
        free(sdm_token);
        sdm_token = NULL;
    }
    if (NULL != serial_str) {
        free(serial_str);
    }
    return ret;
}

// Buffer have extra element for null, so 2*ATCA_SERIAL_NUM_SIZE+1
static int sdm_atca_get_serial_as_string(char* serial_str) {
    int status;
    uint8_t serial_buffer[ATCA_SERIAL_NUM_SIZE];
    status = atcab_init(&atecc608_0_init_data);
    if (status != ATCA_SUCCESS) {
        printf("atcab_init() failed: %02x\r\n", status);
        return status;
    }

    // a tricky way to convert numbers to hex
    // we push the numbers to the second half of the buffer and
    // then print the hex value for each into the same buffer from the beginning
    status = atcab_read_serial_number(serial_buffer);
    if (status != ATCA_SUCCESS) {
        printf("atcab_read_serial_number() failed: %02x\r\n", status);
        atcab_release();
        return status;
    }

    for (int i = 0; i < ATCA_SERIAL_NUM_SIZE; i++) {
        sprintf(&serial_str[i * 2], "%02X", serial_buffer[i]);
    }
    serial_str[ATCA_SERIAL_NUM_SIZE * 2] = 0;

    atcab_release();
    return 0;
}

#if 0
static void sdm_atca_print_root_ca_cert(void) {
    char displaystr[CERT_BUF_MAX_SIZE];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert(g_cryptoauth_root_ca_002_cert, g_cryptoauth_root_ca_002_cert_size, displaystr, &displaylen);
    printf("%s", displaystr);
}
#endif

static int sdm_atca_print_signer_cert(void) {
    int ret;
    uint8_t signer_cert[CERT_BUF_MAX_SIZE];
    size_t  signer_cert_size;

    ret = atcab_init(&atecc608_0_init_data);
    if (ret != ATCA_SUCCESS) {
        printf("SDM: atcab_init() failed: %02x\r\n", ret);
        return ret;
    }

    signer_cert_size = sizeof(signer_cert);
    ret = atcacert_read_cert(
            &g_tngtls_cert_def_1_signer,
            &g_cryptoauth_root_ca_002_cert[CRYPTOAUTH_ROOT_CA_002_PUBLIC_KEY_OFFSET],
            signer_cert,
            &signer_cert_size
    );
    if(ATCACERT_E_SUCCESS != ret) {
        printf("SDM: atcacert_read_cert failed with an error %d\r\n", ret);
        atcab_release();
        return ret;
    }

    char displaystr[CERT_BUF_MAX_SIZE];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert(signer_cert, signer_cert_size, displaystr, &displaylen);
    printf("%s", displaystr);

    atcab_release();
    return 0;
}

static int sdm_atca_print_device_cert(void) {
    int ret;
    uint8_t device_cert[CERT_BUF_MAX_SIZE];
    uint8_t signer_public_key[64] = {0};
    size_t  device_cert_size = sizeof(device_cert);

    ret = atcab_init(&atecc608_0_init_data);
    if (ret != ATCA_SUCCESS) {
        printf("SDM: atcab_init() failed: %02x\r\n", ret);
        return ret;
    }

    ret = atcab_read_pubkey(g_tngtls_cert_def_3_device.ca_cert_def->public_key_dev_loc.slot, signer_public_key);
    if (ATCA_SUCCESS != ret) {
        printf("atcab_read_pubkey failed with an error 0x%x\r\n", ret);
        atcab_release();
        return ret;
    }

    device_cert_size = sizeof(device_cert);
    ret = atcacert_read_cert(&g_tngtls_cert_def_3_device, signer_public_key, device_cert, &device_cert_size);
    if(ATCACERT_E_SUCCESS != ret) {
        printf("SDM: atcacert_read_cert failed with an error %d\r\n", ret);
        atcab_release();
        return ret;
    }

    char displaystr[CERT_BUF_MAX_SIZE];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert(device_cert, device_cert_size, displaystr, &displaylen);
    printf("%s", displaystr);
    atcab_release();
    return ret;
}
static inline bool is_valid_string(const cJSON *json) {
    return (NULL != json && cJSON_IsString(json) && json->valuestring != NULL);
}

static bool sdm_parse_connection_info(SdmInfoResponse* sir, const char* response, size_t conn_index) {
    cJSON *root = cJSON_Parse(response);
    if (NULL == root) {
        printf("SDM: Response is not a valid JSON\r\n");
        return false;
    }

    cJSON *connections = cJSON_GetObjectItemCaseSensitive(root, "connections");
    if (NULL == connections || !cJSON_IsArray(connections)) {
        printf("SDM: Missing connection object \r\n");
        cJSON_Delete(root);
        return false;
    }

    cJSON *connection = cJSON_GetArrayItem(connections, conn_index);
    if (NULL == connection) {
        printf("SDM: Missing connection object entry %u\r\n", (unsigned int)conn_index);
        cJSON_Delete(root);
        return false;
    }

    cJSON *duid = cJSON_GetObjectItem(connection, "deviceId");
    cJSON *cpid = cJSON_GetObjectItem(connection, "cpId");
    cJSON *env = cJSON_GetObjectItem(connection, "code");
    if (!is_valid_string(duid) || !is_valid_string(cpid) || !is_valid_string(env)) {
        printf("SDM: Some items missing from the connection object\r\n");
        cJSON_Delete(root);
        return false;
    }

    memset(sir, 0, sizeof(SdmInfoResponse));
    sir->duid = iotcl_strdup(cJSON_GetStringValue(duid));
    sir->cpid = iotcl_strdup(cJSON_GetStringValue(cpid));
    sir->env = iotcl_strdup(cJSON_GetStringValue(env));

    cJSON_Delete(root);
    return (NULL != sir->duid && NULL != sir->cpid && NULL != sir->env);
}
