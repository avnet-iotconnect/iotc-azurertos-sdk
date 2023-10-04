
// START ATCA definitions
#include "definitions.h"                // SYS function prototypes
#include "cryptoauthlib.h"              //must be manually added for now
#include "nx_crypto.h"
#include "nx_crypto_ec.h"

#include "atcacert/atcacert.h"
#include "atcacert/atcacert_def.h"
#include "atcacert/atcacert_pem.h"
#include "atcacert/atcacert_host_hw.h"
#include "atcacert/atcacert_client.h"
// END ATCA definitions

#include "iotconnect_certs.h"
#include "azrtos_https_client.h"


#if 0
python ./auth.py -D -ks 23DEE57F64FAD5C4597952C88224957A6A0BA63DCABD13AF889CA9A857F1C3B5
# Authenticating step1: receiving challenge and alias
response = requests.get(
    'https://uatpartnerservice.iotconnect.io/partner-api-deviceauth/api/v1/auth/challenge',
    params={},
    headers={'keystoreId': ks},
)
if response.status_code != 204:
    if options.verbose: print(f"challenge Status: {response.status_code}")
    exit(1)
response_headers = response.headers
# print(f'response_headers: {response_headers}')

challenge = response_headers['challenge']
if not challenge:
    if options.verbose or options.debug: print("No challenge received.")
    exit(1)
if options.debug: print(f'challenge: {challenge}')

alias = response_headers['alias']
if not alias:
    if options.verbose or options.debug: print("No alias received.")
    exit(1)
if options.debug: print(f'alias: {alias}')

#endif

//#define SDM_HOST "uatpartnerservice.iotconnect.io"
//#define SDM_HOST "avnetiotpartnerprogram.azure-api.net"
#define SDM_HOST "192.168.38.212"

// this path is for "uatpartnerservice.iotconnect.io"
//#define SDM_INFO_PATH "/partner-api-deviceauth/api/v1/auth/challenge"
#define SDM_CHALLENGE_PATH "/uat-device-auth/api/v1/auth/challenge"
#define SDM_RESPONSE_PATH "/uat-device-auth/api/v1/auth/response"
#define SDM_INFO_PATH "/uat-device-auth/api/v1/info"


extern const atcacert_def_t g_cert_def_1_signer;
extern const uint8_t g_cert_ca_public_key_1_signer[];
extern ATCAIfaceCfg atecc608_0_init_data;

static char* hdr_challenge;
static char* hdr_alias;
static char* hdr_token;

static int sdm_atca_print_signer_cert(void);

static void setup_request_common_field(IotConnectHttpRequest* r, const char* path) {
	r->resource = (char *) path;
	r->host_name = (char *) SDM_HOST;
	//r->tls_cert = (unsigned char*) IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2;
	//r->tls_cert_len = IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2_SIZE;
    r->tls_cert_len = 8080;
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
        hdr_token = clone_header_str(field_value, field_value_length);
    }
    // else ignore the header
}

static VOID on_rsp_header_record_challenge_and_alias(
    CHAR *field_name, UINT field_name_length, CHAR *field_value, UINT field_value_length
) {
    if (0 == strncmp("challenge", field_name, field_name_length)) {       
        hdr_challenge = clone_header_str(field_value, field_value_length);
    } else if (0 == strncmp("alias", field_name, field_name_length)) {
        hdr_alias = clone_header_str(field_value, field_value_length);
    }
    // else ignore the header
}

#define HEX_PREFIX "hex."
bool iotc_sdm_test(IotConnectAzrtosConfig* azrtos_config) {
    UINT http_status;
    ATCA_STATUS atca_status;
    int ret = true;
	IotConnectHttpRequest http_req = {0};    
    uint8_t sha256_digest[IOTC_SHA256_HASH_SIZE];
    uint8_t signature[IOTC_256_BIT_SIGNATURE_SIZE];
    char signature_str[sizeof(HEX_PREFIX) + IOTC_256_BIT_SIGNATURE_SIZE * 2];
	http_req.azrtos_config = azrtos_config;

    hdr_challenge = NULL;
    hdr_alias = NULL;
    hdr_token = NULL;

	setup_request_common_field(&http_req, SDM_CHALLENGE_PATH);


    sdm_atca_print_signer_cert();

    http_req.request_headers[0].name = "KeystoreId";
    http_req.request_headers[0].value = "01231C3A79D721D101";
    http_req.request_headers_size = 1;
    
    http_req.custom_response_header_cb = on_rsp_header_record_challenge_and_alias;
        
    http_status = iotconnect_https_request(&http_req);

    if (http_status != NX_SUCCESS) {
        printf("SDM: Auth request error code: 0x%x data: %s\r\n", http_status, http_req.response);
    }
    
    printf("----\r\n");
    if (NULL != hdr_challenge) {
        printf("SDM: challenge: %s\r\n", hdr_challenge);
    } else {
        printf("SDM: Failed to get challenge\r\n");
        ret = false;
    }

    if (NULL != hdr_alias) {
        printf("SDM: alias: %s\r\n", hdr_alias);
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
    
    /*
    size_t challenge_bin_buffer_size = sizeof(challenge_bin_buffer);
    atca_status = atcab_hex2bin(hdr_challenge, strlen(hdr_challenge), challenge_bin_buffer, &challenge_bin_buffer_size);
    if (atca_status != ATCA_SUCCESS) {
        printf("SDM: atcab_hex2bin() failed: 0x%x\r\n", atca_status);
        goto cleanup;
    }
    */
    
    atca_status = atcab_sha((uint16_t)strlen(hdr_challenge), (uint8_t*)hdr_challenge, sha256_digest);
    if (atca_status != ATCA_SUCCESS) {
        printf("SDM: atcab_sha() failed: 0x%x\r\n", atca_status);
        ret = false;        
        goto cleanup;
    }

    
    atca_status = atcab_sign(0 /* slot */, sha256_digest, signature);
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
    
    free(hdr_alias); // don't need anymore
    hdr_alias = NULL;
    
    setup_request_common_field(&http_req, SDM_RESPONSE_PATH);

    http_req.request_headers[0].name = "challenge";
    http_req.request_headers[0].value = hdr_challenge;
    http_req.request_headers[1].name = "response";
    http_req.request_headers[1].value = signature_str;
    http_req.request_headers_size = 2;
    
    http_req.custom_response_header_cb = on_rsp_header_record_token;

    http_status = iotconnect_https_request(&http_req);

    if (http_status != NX_SUCCESS) {
        printf("SDM: Auth response error code: 0x%x data: %s\r\n", http_status, http_req.response);
        ret = false;
        goto cleanup_atca;        
    }
    
    free(hdr_challenge);
    hdr_challenge = NULL; // don't need anymore
    
    if (NULL == hdr_token) {
        printf("SDM: Failed to get auth token\r\n");
        ret = false;
        goto cleanup_atca;        
    }
    
    printf("Token: %s\r\n", hdr_token);
    
cleanup_atca:
    atcab_release();
    
cleanup:
    if(NULL != hdr_challenge) {
        free(hdr_challenge);
        hdr_challenge = NULL;        
    }
    if (NULL != hdr_alias) {
        free(hdr_alias);
        hdr_alias = NULL;
    }
    if (NULL != hdr_token) {
        free(hdr_token);
        hdr_token = NULL;
    }
    return ret;
}

static int sdm_atca_print_signer_cert(void) {
    int ret;
    ATCA_STATUS status;
    #define CERT_BUF_MAX_SIZE           1024
    UCHAR signer_cert[CERT_BUF_MAX_SIZE];
    UINT  signer_cert_size = sizeof(signer_cert);

    status = atcab_init(&atecc608_0_init_data);
    if (status != ATCA_SUCCESS) {
        printf("SDM: atcab_init() failed: %02x\r\n", status);
        return((int)status);
    }

    signer_cert_size = sizeof(signer_cert);
    ret = atcacert_read_cert(&g_cert_def_1_signer, g_cert_ca_public_key_1_signer, signer_cert, &signer_cert_size);
    if(ATCACERT_E_SUCCESS != ret) {
        printf("SDM: atcacert_read_cert failed with an error %d\r\n", ret);
        atcab_release();
        return ret;
    }

    char displaystr[CERT_BUF_MAX_SIZE];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert(signer_cert, signer_cert_size, displaystr, &displaylen);
    printf("SIGNER CERTIFICATE: \r\n%s\r\n SIGNER CERTIFICATE END\r\n", displaystr);

    atcab_release();
    return ret;
}
