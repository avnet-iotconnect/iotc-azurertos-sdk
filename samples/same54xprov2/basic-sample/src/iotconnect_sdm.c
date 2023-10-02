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

#define SDM_HOST "uatpartnerservice.iotconnect.io"
#define SDM_INFO_PATH "/partner-api-deviceauth/api/v1/auth/challenge"

        
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

extern const atcacert_def_t g_cert_def_1_signer;
extern const uint8_t g_cert_ca_public_key_1_signer[];
extern ATCAIfaceCfg atecc608_0_init_data;

#define CERT_BUF_MAX_SIZE           1024

UCHAR g_signer_cert[CERT_BUF_MAX_SIZE];
UINT  g_signer_cert_size = sizeof(g_signer_cert);

int sdm_atca_get_signer_cert(void)
{
    int ret;
    ATCA_STATUS status;

    status = atcab_init(&atecc608_0_init_data);
    if (status != ATCA_SUCCESS) {
        printf("atcab_init() failed: %02x\r\n", status);
        return((int)status);
    }

    g_signer_cert_size = sizeof(g_signer_cert);
    ret = atcacert_read_cert(&g_cert_def_1_signer, g_cert_ca_public_key_1_signer, g_signer_cert, &g_signer_cert_size);
    if(ATCACERT_E_SUCCESS != ret) {
        printf("atcacert_read_cert failed with an error %d\r\n", ret);
        atcab_release();
        return ret;        
    }

    char displaystr[CERT_BUF_MAX_SIZE];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert(g_signer_cert, g_signer_cert_size, displaystr, &displaylen);
    printf("CLIENT: Rebuilt Signer Certificate: \r\n%s\r\n", displaystr);        

    atcab_release();
    return ret;
}

static VOID sdm_response_header_callback(CHAR *field_name,
    UINT field_name_length, CHAR *field_value, UINT field_value_length) {
    printf("%s=%s\r\n", field_name, field_value);
}
        
static void setup_request(IotConnectHttpRequest* r, const char* path) {
	r->resource = (char *) path;
	r->host_name = (char *) SDM_HOST;
	r->tls_cert = (unsigned char*) IOTCONNECT_GODADDY_G2_ROOT_CERT;
	r->tls_cert_len = IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;
}

int iotc_sdm_test(IotConnectAzrtosConfig* azrtos_config) {
	IotConnectHttpRequest http_req = {0};
	setup_request(&http_req, SDM_INFO_PATH);
	http_req.azrtos_config = azrtos_config;
    
    http_req.request_headers[0].name = "keystoreId";
    http_req.request_headers[0].value = "23DEE57F64FAD5C4597952C88224957A6A0BA63DCABD13AF889CA9A857F1C3B5";
    http_req.request_headers_size = 1;
    
    http_req.custom_response_header_cb = sdm_response_header_callback;
    
    UINT status = iotconnect_https_request(&http_req);

    if (status != NX_SUCCESS) {
        printf("SDM: HTTP request error code: 0x%x data: %s\r\n", status, http_req.response);
        return -1;
    }
    return 0;
}
#if 0
static VOID iotc_https_response_header_callback(NX_WEB_HTTP_CLIENT *client_ptr, CHAR *field_name,
    UINT field_name_length, CHAR *field_value, UINT field_value_length) {
    if (!current_request) {
		printf("iotc_https_response_header_callback internal error!\r\n"); // should never happen
        return;
    }
    for (size_t i = 0; i < IOTC_HTTP_CLIENT_MAX_HEADERS; i++) {
        const char* name = current_request->response_header_names[i];
        if (0 == strcmp(field_name, name)) {
            current_request->response_headers[last_response_header_index] = malloc(sizeof(IotConnectHttpHeader));            
            IotConnectHttpHeader* rsp_hdr_ptr = current_request->response_headers[last_response_header_index];
            if (NULL == rsp_hdr_ptr) {                
                printf("iotc_https_response_header_callback out of memory!\r\n");
                return;                
            }

            rsp_hdr_ptr->name = (char*) malloc(field_name_length + 1);
            rsp_hdr_ptr->value = (char*) malloc(field_value_length + 1);
            if (NULL == rsp_hdr_ptr->name || NULL == rsp_hdr_ptr->value) {
                if (rsp_hdr_ptr->name) {
                    free(rsp_hdr_ptr->name);
                }
                free(rsp_hdr_ptr);
                printf("iotc_https_response_header_callback out of memory!\r\n");
                return;                
            }
            memcpy(rsp_hdr_ptr->name, field_name, field_name_length);
            rsp_hdr_ptr->name[field_name_length] = 0;
            memcpy(rsp_hdr_ptr->value, field_value, field_value_length);
            rsp_hdr_ptr->value[field_value_length] = 0;
            
            last_response_header_index++;
            // make sure to always terminate the list
            current_request->response_headers[last_response_header_index] = NULL;
        }
    }
}
#endif