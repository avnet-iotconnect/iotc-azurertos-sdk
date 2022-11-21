//
// Copyright: Avnet, Softweb Inc. 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/15/22.
//

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "nx_crypto_sha2.h"
#include "iotconnect_certs.h"
#include "iotconnect_discovery_v3.h"
#include "iotc_algorithms.h"
#include "iotc_auth_driver.h"
#include "azrtos_https_client.h"
#include "iotconnect_device_identity.h"

#define DDIM_AUTH_PATH "/x509/auth"
#define DDIM_SIGN_PATH "/x509/cert/sign"
#define DDIM_ACK_PATH "/x509/cert/ack"

#define URL_RESOUCE_BUFFER_SIZE 40

// some certs can be more than 512 bytes, so we ought to have enough space for all of 
// it the hex characters. 
#define WORK_BUFFER_SIZE (1500) 
// Buffer that we will malloc and free when we are done.
// Buffer will be used for conversions between binary and hex data.
// 1 for null
static uint8_t work_buffer[WORK_BUFFER_SIZE];
static size_t work_buffer_size = 0;
static char * discovery_agent_host = NULL;
static char * discovery_agent_path = NULL;
static char resource_buffer[URL_RESOUCE_BUFFER_SIZE];

static char * bin_to_hex(uint8_t* bin_array, size_t bin_len) {
	work_buffer_size = 0;
	int hex_idx = 0;
	for (size_t i = 0; i < bin_len; i++) {
		work_buffer[hex_idx] = 0; // empty string
		int ret = sprintf((char*)&work_buffer[hex_idx], "%02x", bin_array[i]);
		if (ret != 2) {
			printf("DDIM: bin_to_hex: buffer overflow!\r\n");
			work_buffer[0] = 0;
			return NULL;
		}
		hex_idx += 2;
		if (hex_idx >= WORK_BUFFER_SIZE -1) {
			printf("DDIM: bin_to_hex: buffer overflow!\r\n");
			work_buffer[0] = 0;
			return NULL;
		}
	}
	work_buffer_size = hex_idx + 1; // for null;
	return (char *) work_buffer;
}
static bool is_valid_hex_digit(char ch) {
	return (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <='f') || (ch >= 'A' && ch <= 'F');
}
int hex_digit_to_int(char digit) {
    if (digit >= 'A' && digit <= 'F') {
        return digit - 'A' + 10;
    } else if (digit >= 'a' && digit <= 'f') {
        return digit - 'a' + 10;
    } else if (digit >= '0' && digit <= '9') {
        return digit - '0';
    }

    return -1; // Bad input.
}
static uint8_t* hex_to_bin(char* hex_str) {
	int bin_idx = 0;
	int hex_idx = 0;
	if (hex_str == NULL){
		return NULL;
	}
	size_t hex_len = strlen(hex_str);

	work_buffer_size = 0;

	for (size_t i = 0; i < hex_len; i++) {
		hex_str[i] = tolower(hex_str[i]);
	}
	while (hex_str[hex_idx] != 0) {
		if (!is_valid_hex_digit(hex_str[hex_idx]) || !is_valid_hex_digit(hex_str[hex_idx+1])) {
			printf("DDIM: hex_to_bin: Error: encountered non-hex characters or early end of string \"%c%c\"\r\n", hex_str[hex_idx], hex_str[hex_idx+1]);
			return NULL;
		}
		int dmsb = hex_digit_to_int(hex_str[hex_idx]);
		int dlsb = hex_digit_to_int(hex_str[hex_idx+1]);
		if (dmsb == -1 || dlsb == -1) {
			printf("DDIM: hex_to_bin: unable to scan hex string byte %c%c! at string index %d\r\n", hex_str[hex_idx], hex_str[hex_idx + 1], hex_idx);
			return NULL;
		}
		work_buffer[bin_idx] = (uint8_t)(dmsb * 16 + dlsb);
		if (bin_idx >= WORK_BUFFER_SIZE -2 /* 1 for the next byte */) {
			printf("DDIM: hex_to_bin: buffer overflow!\r\n");
			return NULL;
		}
		bin_idx++;
		hex_idx += 2;
	}
	work_buffer_size = bin_idx;
	return work_buffer;
}

static void dump_response(const char *message, IotConnectHttpRequest *response) {
    printf("DDIM: %s", message);
    if (response->response) {
        printf("DDIM:  Response was:\r\n----\r\n%s\r\n----\r\n", response->response);
    } else {
        printf("DDIM:  Response was empty\r\n");
    }
}

static void setup_request(IotConnectHttpRequest* r, const char* path_suffix) {
	strcpy(resource_buffer, discovery_agent_path);
	strcat(resource_buffer, path_suffix);
	r->resource = resource_buffer;
	r->host_name = discovery_agent_host;
	r->tls_cert = (unsigned char*) IOTCONNECT_GODADDY_G2_ROOT_CERT;
	r->tls_cert_len = IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;
}

static int ddim_call_auth(//
		IotConnectAzrtosConfig* azrtos_config, //
		IotclDdimAuthRequest *request, //
		IotclDdimAuthResponse **resp) {
	IotConnectHttpRequest http_req = {0};
	setup_request(&http_req, DDIM_AUTH_PATH);
	http_req.azrtos_config = azrtos_config;
	http_req.payload = (char *)iotcl_ddim_auth_request_create_serialized_string(request);

	//Sensitive data & only for debugging
	//printf("DDIM: Calling: https://%s%s\r\n", http_req.host_name, http_req.resource);
	//printf("DDIM: Payload: %s\r\n", http_req.payload);

	*resp = NULL;

    UINT status = iotconnect_https_request(&http_req);

    if (status != NX_SUCCESS) {
        printf("DDIM: Auth HTTP request error code: %x data: %s\r\n", status, http_req.response);
        return -1;
    }

    if (NULL == http_req.response || 0 == strlen(http_req.response)) {
        dump_response("DDIM: Auth HTTP response was empty", &http_req);
        return -2;
    }

    IotclDdimAuthResponse* r = iotcl_ddim_parse_auth_response(http_req.response);
    if (!r) {
    	dump_response("DDIM: Unable to parse auth HTTP response", &http_req);
    	return -3;
    }
    if (r->ec != 0) {
    	printf("DDIM : Received error code %d from auth HTTP request. Message was \"%s\"\r\n",r->ec, r->message ? r->message : "<empty message>");
    	iotcl_ddim_free_auth_response(r);
    	return -4;
    }
    if (!r->cid || !r->cn || !r->rn) {
    	if (!r->cid) {
        	printf("DDIM: Auth CID is empty\r\n");
    	}
    	if (!r->cn) {
        	printf("DDIM: Auth Common Name is empty\r\n");
    	}
    	if (!r->rn) {
        	printf("DDIM: Auth Random Number is empty\r\n");
    	}
    	iotcl_ddim_free_auth_response(r);
    	return -5;
    }
    *resp = r;
    return 0;
}

static int ddim_call_sign( //
		IotConnectAzrtosConfig* azrtos_config, //
		IotclDdimSignRequest *request, //
		IotclDdimSignResponse **resp) {
	IotConnectHttpRequest http_req = {0};
	setup_request(&http_req, DDIM_SIGN_PATH);
	http_req.azrtos_config = azrtos_config;
	http_req.payload = (char *)iotcl_ddim_sign_request_create_serialized_string(request);

	//Sensitive data & only for debugging
	//printf("DDIM: Calling: https://%s%s\r\n", http_req.host_name, http_req.resource);
	//printf("DDIM: Payload: %s\r\n", http_req.payload);

	*resp = NULL;

    UINT status = iotconnect_https_request(&http_req);

    if (status != NX_SUCCESS) {
        printf("DDIM: Sign HTTP request error code: %x data: %s\r\n", status, http_req.response);
        return -1;
    }

    if (NULL == http_req.response || 0 == strlen(http_req.response)) {
        dump_response("DDIM: sign HTTP response was empty", &http_req);
        return -2;
    }

    IotclDdimSignResponse *r = iotcl_ddim_parse_sign_response(http_req.response);
    if (!r) {
    	dump_response("DDIM: Unable to parse sign HTTP response", &http_req);
		iotcl_ddim_free_sign_response(r);
    	return -3;
    }

    if (r->ec != 0) {
		printf("DDIM : Received error code %d from sign HTTP request. Message was \"%s\"\r\n",r->ec, r->message ? r->message : "<empty message>");
		iotcl_ddim_free_sign_response(r);
		return -4;
    }
    *resp = r;
    return 0;
}

static int ddim_call_ack( //
		IotConnectAzrtosConfig* azrtos_config, //
		IotclDdimAckRequest *request, //
		IotclDdimAckResponse **resp) {
	IotConnectHttpRequest http_req = {0};
	setup_request(&http_req, DDIM_ACK_PATH);
	http_req.azrtos_config = azrtos_config;
	http_req.payload = (char *)iotcl_ddim_ack_request_create_serialized_string(request);

	//Sensitive data & only for debugging
	//printf("DDIM: Calling: https://%s%s\r\n", http_req.host_name, http_req.resource);
	//printf("DDIM: Payload: %s\r\n", http_req.payload);

	*resp = NULL;

	UINT status = iotconnect_https_request(&http_req);

    if (status != NX_SUCCESS) {
        printf("DDIM: Ack HTTP request error code: %x data: %s\r\n", status, http_req.response);
        return -1;
    }

    if (NULL == http_req.response || 0 == strlen(http_req.response)) {
        dump_response("DDIM: Ack HTTP response was empty", &http_req);
        return -2;
    }

    IotclDdimAckResponse *r = iotcl_ddim_parse_ack_response(http_req.response);
    if (!r) {
    	dump_response("DDIM: Unable to parse the ack HTTP response", &http_req);
		iotcl_ddim_free_ack_response(r);
    	return -3;
    }

    if (r->ec != 0) {
		printf("DDIM : Received error code %d from sign HTTP request. Message was \"%s\"\r\n",r->ec, r->message ? r->message : "<empty message>");
		iotcl_ddim_free_ack_response(r);
		return -4;
    }
    *resp = r;
    return 0;
}
static int parse_discovery_host_and_path(char * discovery_agent_url) {
	if (NULL == strstr(discovery_agent_url, "https://")) {
		printf("DDIM : Expected https:// in agent URL \"%s\"\r\n", discovery_agent_url);
		return -1;
	}
	char *host_ptr = &discovery_agent_url[strlen("https://")];
	char *path = strstr( host_ptr, "/" );
	discovery_agent_path = malloc(strlen(path) + 1);
	if (!discovery_agent_path) {
		printf("DDIM : unable to allocate discovery host buffer\r\n");
		free(discovery_agent_path);
		return -2;
	}
	memcpy(discovery_agent_path, path, strlen(path) + 1);

	path[0] = 0; // terminate the host string at path
	discovery_agent_host = malloc(strlen(host_ptr) + 1);
	if (!discovery_agent_host) {
		printf("DDIM : unable to allocate discovery host buffer\r\n");
		free(discovery_agent_path);
		discovery_agent_path = NULL;
		return -3;
	}
	memcpy(discovery_agent_host, host_ptr, strlen(host_ptr) + 1);
	return 0;
}

static int discovery_call( //
		IotConnectAzrtosConfig* azrtos_config, //
		const char *env,
		char* cn) { //
	IotConnectHttpRequest http_req = {0};
	http_req.host_name = DISCOVERY_V3_HOST_NAME;
	http_req.resource = NULL;
	http_req.azrtos_config = azrtos_config;
	http_req.payload = NULL;
	http_req.tls_cert = (unsigned char*) IOTCONNECT_GODADDY_G2_ROOT_CERT;
	http_req.tls_cert_len = IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;

	char* cpid = NULL;
	char* duid = NULL;
	int ret = split_cn_to_cpid_and_duid(cn, &cpid, &duid);
	if (ret) {
		printf("DDIM: Unable split common name\r\n");
		return ret;
	}
	char* discovery_path = malloc(strlen(DISCOVERY_V3_RESOURCE_FORMAT) + strlen(env) + strlen(cpid) /* null space and some slack included in format */);
	if (!discovery_path) {
		printf("DDIM: Unable allocate discovery path buffer\r\n");
		return -1;
	}
	sprintf(discovery_path, DISCOVERY_V3_RESOURCE_FORMAT, cpid, env);
	http_req.resource = discovery_path;

	printf("DDIM: Performing discovery\r\n");
    UINT status = iotconnect_https_request(&http_req);

    if (status != NX_SUCCESS) {
        printf("DDIM: HTTP request error code: %x data: %s\r\n", status, http_req.response);
        free(discovery_path);
        return -1;
    }

    if (NULL == http_req.response || 0 == strlen(http_req.response)) {
        dump_response("HTTP response was empty\r\n", &http_req);
        free(discovery_path);
        return -2;
    }

    IotclDiscoveryV3Response *r= iotcl_parse_discovery_v3_response(http_req.response);
    if (!r) {
    	dump_response("Unable to parse discovery HTTP response\r\n", &http_req);
    	free(discovery_path);
    	return -3;
    }
	free(discovery_path);
	if (!r->bu) {
    	dump_response("Warning: Unable to obtain agent URL\r\n", &http_req);
    	iotcl_free_discovery_v3_response(r);
    	return -4;
    }

	if (discovery_agent_host) {
    	free(discovery_agent_host);
    	discovery_agent_host = NULL;
    }
    if (discovery_agent_path) {
    	free(discovery_agent_path);
    	discovery_agent_path = NULL;
    }

    char * agent_url = malloc(strlen(r->bu) + 1);
    if (!agent_url) {
    	printf("DDIM: Unable to allocate agent URL string\r\n");
    	iotcl_free_discovery_v3_response(r);
    	return -5;
    }
    memcpy(agent_url, r->bu, strlen(r->bu) + 1);
    if (parse_discovery_host_and_path(agent_url)) {
    	if (discovery_agent_host) {
    		free(discovery_agent_host);
    		discovery_agent_host = NULL;
    	}
    	if (discovery_agent_path) {
    		free(discovery_agent_path);
    		discovery_agent_path = NULL;
    	}
    	free(agent_url);
    	iotcl_free_discovery_v3_response(r);
    	return -6;
    }
	
    printf("DDIM: Discovery result: %s %s\r\n", discovery_agent_host, discovery_agent_path);
	free(agent_url);
    iotcl_free_discovery_v3_response(r);
	return 0;
}

int iotcdi_obtain_operational_identity(IotConnectAzrtosConfig* azrtos_config, IotcDdimInterface* ddim_interface, IotcAuthInterfaceContext auth_interface_context, const char* env) {
	int ret = 0;
	IotclDdimAuthRequest auth_req = {0};
	IotclDdimSignRequest sign_req = {0};
	IotclDdimAckRequest ack_req = {0};
	IotclDdimAuthResponse * auth_rsp = NULL;
	IotclDdimSignResponse * sign_rsp = NULL;
	IotclDdimAckResponse * ack_rsp = NULL;

	if (!discovery_agent_host || !discovery_agent_path) {
		char* bootstrap_cn = ddim_interface->extract_bootstrap_cn(auth_interface_context);
		if (!bootstrap_cn) {
			ret = -1; goto cleanup;; // called function  will report error
		}
		if (discovery_call(azrtos_config, env, bootstrap_cn)) {
			printf("DDIM: Unable to discover agent URL\r\n");
			ret = -2; goto cleanup;; // called function  will report error
		}
	}

	{  // scope block
		uint8_t* cert_bin;
		size_t cert_len;
		if (ddim_interface->get_bootstrap_cert(auth_interface_context, &cert_bin, &cert_len)) {
			ret = -3; goto cleanup; // called function with print errors
		}
		auth_req.bcert = bin_to_hex(cert_bin, cert_len);
		if (!auth_req.bcert) {
			printf("DDIM: Unable to convert bootstrap cert to hex\r\n");
			ret = -4; goto cleanup;; // called function  will report error
		}
	}
	auth_req.fmt = "hex";


	printf("DDIM: Performing authentication\r\n");
	if (ddim_call_auth(azrtos_config, &auth_req, &auth_rsp)) {
		ret = -5; goto cleanup; // called function with print errors
	}

	{  // scope block
		uint8_t* csr_bin;
		size_t csr_len;
		//Sensitive data & only for debugging
		//printf("DDIM: Received new CN: %s\r\n", auth_rsp->cn);
		printf("DDIM: Generating CSR...\r\n");
		if (ddim_interface->generate_csr(auth_interface_context, auth_rsp->cn, &csr_bin, &csr_len)) {
			ret = -6; goto cleanup; // called function with print errors
		}

		//  --- SIGN HASH ---
		NX_CRYPTO_SHA256 ctx;
		//printf("----DEBUG Sizeof NX_CRYPTO_SHA256: %lu\r\n", sizeof(ctx));
		UINT status;
		status = _nx_crypto_sha256_initialize(&ctx, NX_CRYPTO_HASH_SHA256);
		if (status) {
			printf("DDIM: Unable to initialize sha256 hash function\r\n");
			ret = -7; goto cleanup;
		}
		status |= _nx_crypto_sha256_update(&ctx, (UCHAR*)auth_rsp->rn, strlen(auth_rsp->rn));
		status |= _nx_crypto_sha256_update(&ctx, (UCHAR*)auth_rsp->cid, strlen(auth_rsp->cid));
		status |= _nx_crypto_sha256_update(&ctx, csr_bin, csr_len);
		if (status) {
			printf("DDIM: Unable to update the hash\r\n");
			ret = -8; goto cleanup;
		}
		//Sensitive data & only for debugging
		//printf("DDIM: Signing: %s + %s + hex_to_binary(\"%s\")\r\n", auth_rsp->rn, auth_rsp->cid, bin_to_hex(csr_bin, csr_len));
		uint8_t sha_hash[IOTC_SHA256_HASH_SIZE];
		status = _nx_crypto_sha256_digest_calculate(&ctx, sha_hash, NX_CRYPTO_HASH_SHA256);
		if (status) {
			printf("DDIM: Unable to compute the hash\r\n");
			ret = -9; goto cleanup;
		}
		uint8_t s_r_buffer[IOTC_256_BIT_SIGNATURE_SIZE];
		if (ddim_interface->sign_hash(auth_interface_context, sha_hash, s_r_buffer)) {
			printf("DDIM: Unable to sign the hash\r\n");
			ret = -10; goto cleanup; // called function with print errors
		}

		// --- CONVERT SIG TO DER ----
		size_t signature_size = 0;
		uint8_t asn1_sig[IOTC_256_BIT_SIGNATURE_SIZE + 10]; // 2 bytes for header + 2 for each integer header + 4 bytes slack];
		status = sig_to_asn1_der(
				asn1_sig, //
				sizeof(asn1_sig),  //
				&signature_size, //
				&s_r_buffer[0], &s_r_buffer[IOTC_256_BIT_SIGNATURE_SIZE/2], //
                IOTC_256_BIT_KEY_SIZE * 8 //
				);
		if (status) {
			printf("DDIM: Unable to convert signature S&R to der format\r\n");
			ret = -11; goto cleanup;
		}

		// we can't reuse the work buffer below because we have two hex strings to return. We need to make a copy
		char *sig_hex = bin_to_hex(asn1_sig, signature_size);
		if (!sig_hex) {
			printf("DDIM: Unable to convert signature to hex\r\n");
			ret = -12; goto cleanup;
		}

		sign_req.sig = (char*)malloc(work_buffer_size); // work_buffer_size will be +1 for null
		if (!sign_req.sig) {
			printf("DDIM: Unable to allocate the signature string buffer\r\n");
			ret = -13; goto cleanup;
		}
		memcpy(sign_req.sig, work_buffer, work_buffer_size);

		sign_req.csr = bin_to_hex(csr_bin, csr_len);
		if (!sign_req.csr) {
			printf("DDIM: Unable to convert CSR to hex\r\n");
			ret = -14; goto cleanup;
		}

		sign_req.fmt = "hex";

		printf("DDIM: Requesting operational certificate.\r\n");
		if (ddim_call_sign(azrtos_config, &sign_req, &sign_rsp)) {
			ret = -15; goto cleanup; // called function with print errors
		}
	}
	//Sensitive data & only for debugging
	//printf("DDIM: Received cert: %s\r\n", sign_rsp->cert);
	hex_to_bin(sign_rsp->cert);
	if (0 == work_buffer_size) {
		printf("DDIM: Unable to convert cert to bin\r\n");
		ret = -16; goto cleanup;
	}

	if(ddim_interface->store_operational_cert(auth_interface_context, &work_buffer[0], work_buffer_size)) {
		ret = -17; goto cleanup; // called function with print errors
	}

	ack_req.cid = auth_rsp->cid;
	//Sensitive data & only for debugging
	//printf("DDIM: Acknowledging cert: %s\r\n", sign_rsp->cert);
	if (ddim_call_ack(azrtos_config, &ack_req, &ack_rsp)) {
		ret = -18; goto cleanup; // called function with print errors
	}

	// pass through on success with ret = 0;
	cleanup:
	if (auth_rsp) {
		iotcl_ddim_free_auth_response(auth_rsp);
	}
	if (sign_rsp) {
		iotcl_ddim_free_sign_response(sign_rsp);
	}
	if (ack_rsp) {
		iotcl_ddim_free_ack_response(ack_rsp);
	}
	if (sign_req.sig) {
		free(sign_req.sig); // the malloc'd copy
	}
	return ret;
}

