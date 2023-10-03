
// START ATCA definitions
#include "definitions.h"                // SYS function prototypes
#include "cryptoauthlib.h"              //must be manually added for now
#include "atca_test.h"
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
#define SDM_HOST "192.168.38.212"
#define SDM_INFO_PATH "/partner-api-deviceauth/api/v1/auth/challenge"

extern const atcacert_def_t g_cert_def_1_signer;
extern const uint8_t g_cert_ca_public_key_1_signer[];
extern ATCAIfaceCfg atecc608_0_init_data;

static char* hdr_challenge;
static char* hdr_alias;

static int sdm_atca_print_signer_cert(void);

static void setup_request_common_field(IotConnectHttpRequest* r, const char* path) {
	r->resource = (char *) path;
	r->host_name = (char *) SDM_HOST;
    // when using HTT
	//r->tls_cert = (unsigned char*) IOTCONNECT_GODADDY_G2_ROOT_CERT;
	//r->tls_cert_len = IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;
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

bool iotc_sdm_test(IotConnectAzrtosConfig* azrtos_config) {
    int ret = true;
	IotConnectHttpRequest http_req = {0};
	http_req.azrtos_config = azrtos_config;

    hdr_challenge = NULL;
    hdr_alias = NULL;

	setup_request_common_field(&http_req, SDM_INFO_PATH);


    sdm_atca_print_signer_cert();

    http_req.request_headers[0].name = "keystoreId";
    http_req.request_headers[0].value = "23DEE57F64FAD5C4597952C88224957A6A0BA63DCABD13AF889CA9A857F1C3B5";
    http_req.request_headers_size = 1;
    
    http_req.custom_response_header_cb = on_rsp_header_record_challenge_and_alias;
    
    UINT status = iotconnect_https_request(&http_req);

    if (status != NX_SUCCESS) {
        printf("SDM: HTTP request error code: 0x%x data: %s\r\n", status, http_req.response);
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
    
    if (!ret) {
        goto cleanup;
    }
    
cleanup:
    if(NULL != hdr_challenge) {
        free(hdr_challenge);
        hdr_challenge = NULL;        
    }
    if (NULL != hdr_alias) {
        free(hdr_alias);
        hdr_alias = NULL;
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
