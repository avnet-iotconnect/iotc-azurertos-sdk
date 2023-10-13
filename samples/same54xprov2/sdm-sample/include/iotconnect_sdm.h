//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 10/12/23.
//

// challenge is a null terminated string
// signature_size needs to be signature_buff max size
// Typically this is IOTC_256_BIT_SIGNATURE_SIZE, but could vary based on the private key algorithm
typedef int (*IotConnectSdmSign) (const char* challenge, uint8_t* signature_buff, size_t* signature_size);

typedef struct IotConnectSdmConfig {
    char *keystore_id;
    char *alias;
    IotConnectSdmSign* sign_challenge_function;
} IotConnectSdmConfig;

typedef struct SdmInfoResponse {
    char* duid;
    char* cpid;
    char* env;
} SdmInfoResponse;

// the token returned will be malloc-ed. It's the responsibility of the caller to free it.
int iotc_sdm_get_token(IotConnectAzrtosConfig* azrtos_config, IotConnectSdmConfig sdm_config, char **sdm_token);

// the info response returned will contain malloec-ed strings. It's the responsibility of the caller to free them.
int iotc_sdm_get_info(SdmInfoResponse* sir, const char* sdm_token);