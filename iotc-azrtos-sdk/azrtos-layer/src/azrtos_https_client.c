//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
// Modified by Shu Liu <shu.liu@avnet.com> on 4/19/21.
//

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "nx_secure_tls.h"
#include "nx_secure_tls_api.h"
#include "nx_web_http_client.h"
#include "iotconnect_certs.h"
#include "iotconnect.h"
#include "azrtos_https_client.h"
#include "nx_azure_iot_ciphersuites.h"

#ifndef NX_WEB_HTTP_TCP_WINDOW_SIZE
#define NX_WEB_HTTP_TCP_WINDOW_SIZE     1000
#endif

#ifndef IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE
#define IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE 1000
#endif

#ifndef IOTCONNECT_TLS_PACKET_BUFFER_SIZE
#define IOTCONNECT_TLS_PACKET_BUFFER_SIZE  6000
#endif

// Used when not IOTC_HTTP_SHARED_MEMORY_HACK
#ifndef IOTCONNECT_HTTPS_TLS_BUFFERSIZE
#define IOTCONNECT_HTTPS_TLS_BUFFERSIZE  17856
#endif

#ifndef IOTCONNECT_HTTPS_CERT_BUFFERSIZE
#define IOTCONNECT_HTTPS_CERT_BUFFERSIZE  4000
#endif
#define HDR_CT_NAME "Content-Type"
#define HDR_CT_VALUE "application/json" // for content type


extern const NX_SECURE_TLS_CRYPTO nx_crypto_tls_ciphers;

static UCHAR tls_packet_buffer[IOTCONNECT_TLS_PACKET_BUFFER_SIZE];

// For stm32u5 we simply cannot squeeze enough of RAM
// When IOTC_HTTP_SHARED_MEMORY_HACK is enabled in the build,
// We will share the metadata buffer with MQTT
// This breaks OTA, so OTA must not be used along with this
#ifdef IOTC_HTTP_RAM_USAGE_HACK
extern UCHAR nx_azure_iot_tls_metadata_buffer[];
static CHAR* crypto_client_metadata = (CHAR *)nx_azure_iot_tls_metadata_buffer;
#else
static CHAR crypto_client_metadata[IOTCONNECT_HTTPS_TLS_BUFFERSIZE];
#endif
static char response_buffer[IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE];

//static UCHAR tls_packet_buffer[IOTCONNECT_HTTPS_TLS_BUFFERSIZE];
//static CHAR crypto_client_metadata[IOTCONNECT_HTTPS_TLS_BUFFERSIZE];
static NX_SECURE_X509_CERT trusted_certificate;
static NX_SECURE_X509_CERT remote_certificate, remote_issuer;
static UCHAR remote_cert_buffer[IOTCONNECT_HTTPS_CERT_BUFFERSIZE];
static UCHAR remote_issuer_buffer[IOTCONNECT_HTTPS_CERT_BUFFERSIZE];
static NX_SECURE_X509_DNS_NAME dns_name;

// single request only support. Record current request for tls callback
static IotConnectHttpRequest *current_request = NULL;
static size_t last_response_header_index = 0;

static UINT tls_setup_callback(NX_WEB_HTTP_CLIENT *client_ptr, NX_SECURE_TLS_SESSION *tls_session);

static VOID iotc_https_response_header_callback(
    NX_WEB_HTTP_CLIENT *client_ptr,
    CHAR *field_name,
    UINT field_name_length,
    CHAR *field_value,
    UINT field_value_length
) {
    (void) client_ptr;
    current_request->custom_response_header_cb(field_name, field_name_length, field_value, field_value_length);
}

UINT iotconnect_https_request(IotConnectHttpRequest *r) {
    UINT status;
    NX_WEB_HTTP_CLIENT http_client;
    NXD_ADDRESS server_ip_address;
    
    last_response_header_index = 0;

    if (!r ||  !r->azrtos_config || !r->host_name || !r->tls_cert || 0 == r->tls_cert_len) {
        printf("HTTP: Invalid arguments\r\n");
        return NX_INVALID_PARAMETERS;
    }

    r->response = response_buffer;
    r->response[0] = 0; // null terminate

    status = nx_web_http_client_create(
            &http_client, "IoTConnect Client",
            r->azrtos_config->ip_ptr,
            r->azrtos_config->pool_ptr,
            NX_WEB_HTTP_TCP_WINDOW_SIZE);

    if (status) {
        printf("HTTP: Client create failed: 0x%x\r\n", status);
        return status;
    }

    status = nx_dns_host_by_name_get(
            r->azrtos_config->dns_ptr,
            (UCHAR*) r->host_name,
            &server_ip_address.nxd_ip_address.v4,
            5 * NX_IP_PERIODIC_RATE
    ); // give it 5 seconds to resolve

    if (status) {
        printf("HTTP: Host DNS resolution failed 0x%x\r\n", status);
        goto cleanup;
    }

    current_request = r;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V4;
    status = nx_web_http_client_secure_connect(
            &http_client, //
            &server_ip_address,//
            NX_WEB_HTTPS_SERVER_PORT,//
            tls_setup_callback,//
            NX_WAIT_FOREVER);

    if (status) {
        printf("HTTP: Error in HTTP Connect: 0x%x\r\n", status);
        goto cleanup;
    }


    // PROVIDED HANDLER CALLBACK
    if (r->custom_handler_cb) {
        status = r->custom_handler_cb(r, &http_client);
        if (status != NX_SUCCESS) {
            printf("HTTP: Error in HTTP PUT request initialization: 0x%x\r\n", status);
            goto cleanup;
        }
    }

    if (!r->resource) {
        printf("HTTP: Resource needs to be provided\r\n");
        status = NX_INVALID_PARAMETERS;
        goto cleanup;
    }

    if (r->payload) {
        status = nx_web_http_client_request_initialize(&http_client,
                NX_WEB_HTTP_METHOD_POST,
                r->resource, r->host_name,
                strlen(r->payload),          //  POST input size needed here
                NX_FALSE,
                NULL,
                NULL,
                NX_WAIT_FOREVER);


        if (status != NX_SUCCESS) {
            printf("HTTP: Error in HTTP PUT request initialization: 0x%x\r\n", status);
            goto cleanup;
        }

    } else {
        status = nx_web_http_client_request_initialize(&http_client,
                NX_WEB_HTTP_METHOD_GET, /* GET, PUT, DELETE, POST, HEAD */
                r->resource, r->host_name, 0, /* PUT and POST need an input size. */
                NX_FALSE, /* If true, input_size is ignored. */
                NULL,
                NULL,
                NX_WAIT_FOREVER);
        if (status != NX_SUCCESS) {
            printf("HTTP: Error in HTTP GET request initialization: 0x%x\r\n", status);
            goto cleanup;
        }
    }

    if (0 == r->request_headers_size) {
        status = nx_web_http_client_request_header_add(&http_client,
                HDR_CT_NAME, strlen(HDR_CT_NAME),
                HDR_CT_VALUE, strlen(HDR_CT_VALUE),
                NX_WAIT_FOREVER);
        if (status != NX_SUCCESS) {
            printf("HTTP: Error in HTTP request headers setup: 0x%x\r\n", status);
            goto cleanup;
        }
    } else {
        if (r->request_headers_size > IOTC_HTTP_CLIENT_MAX_HEADERS) {
            printf("HTTP: HTTP request header size is greater than maximum supported!\r\n"); 
            status = NX_INVALID_PARAMETERS;
            goto cleanup;            
        }
        for (int req_hdr_idx = 0; req_hdr_idx < r->request_headers_size; req_hdr_idx++) {
            IotConnectHttpHeader * req_hdr = &r->request_headers[req_hdr_idx];
            if (NULL == req_hdr->name || 0 == strlen(req_hdr->name)
                    || NULL == req_hdr->value || 0 == strlen(req_hdr->value)) {
                printf("HTTP: HTTP request header is not valid!\r\n"); 
                goto cleanup;
            }
            printf("Adding header: %s=%s\r\n", req_hdr->name, req_hdr->value);
            status = nx_web_http_client_request_header_add(&http_client,
                    req_hdr->name, strlen(req_hdr->name),
                    req_hdr->value, strlen(req_hdr->value),
                    NX_WAIT_FOREVER);            
            if (status != NX_SUCCESS) {
                printf("HTTP: Error in HTTP request headers setup: 0x%x\r\n", status);
                goto cleanup;
            }
        }
    }
    
    if (r->custom_response_header_cb) {        
        status = nx_web_http_client_response_header_callback_set(&http_client, iotc_https_response_header_callback);
        if (status != NX_SUCCESS) {
            printf("HTTP: Error in HTTP response headers setup: 0x%x\r\n", status);
            goto cleanup;
        }        
    }

    // common for both GET and POST
    status = nx_web_http_client_request_send(&http_client, NX_WAIT_FOREVER);
    if (status) {
        printf("HTTP: Error in HTTP request send: 0x%x\r\n", status);
        goto cleanup;
    }

    if (r->payload) {
        NX_PACKET *packet_ptr;
        /* Create a new data packet request on the HTTP(S) client instance. */
        nx_web_http_client_request_packet_allocate(&http_client, &packet_ptr, NX_WAIT_FOREVER);
        if (status != NX_SUCCESS) {
            printf("HTTP: Error while allocating packet: 0x%x\r\n", status);
            goto cleanup;
        }

        status = nx_packet_data_append(packet_ptr, (VOID *) r->payload, strlen(r->payload),
                                       packet_ptr -> nx_packet_pool_owner,
                                       NX_WAIT_FOREVER);
        if (status) {
            printf("HTTP: Error while appending packet data: 0x%x\r\n", status);
            goto cleanup;
        }

         /* Send data packet request to server. */
        status = nx_web_http_client_request_packet_send(&http_client, packet_ptr, 0, NX_WAIT_FOREVER);
        if (status) {
            printf("HTTP: Error sending packet: 0x%x\r\n", status);
             goto cleanup;
        }
    }

    /* Receive response data from the server. Loop until all data is received. */
    NX_PACKET *receive_packet = NULL;
    UINT get_status = NX_SUCCESS;
    size_t data_length = 0;
    while (get_status != NX_WEB_HTTP_GET_DONE) {
    	get_status = nx_web_http_client_response_body_get(&http_client, &receive_packet, NX_WAIT_FOREVER);

        /* Check for error.  */
        if (get_status != NX_SUCCESS && get_status != NX_WEB_HTTP_GET_DONE) {
            printf("HTTP get packet failed, error: 0x%x\r\n", get_status);
            status = get_status;
            break;
        }

		UINT packet_len = receive_packet->nx_packet_length;
		if (packet_len == 0) {
			if (get_status == NX_WEB_HTTP_GET_DONE) {
				status = NX_SUCCESS;
			} else {
				// not sure how to handle this case. I guess break and send last status.
				status = get_status;
				printf("HTTP packet length was zero. Aborting the receive loop!");
			}
			break; // in both cases
		}
		ULONG bytes_received = 0;
        status = nx_packet_data_extract_offset( //
                receive_packet, // packet
                0, // offset in the packet
                &(r->response[data_length]), // where to put data
                IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE - data_length - 1 /* 1 for null */, // bytes left in buffer
                &bytes_received);

        if (status) {
            printf("HTTP: Packet data extraction error: 0x%x\r\n", status);
            break;
        }
        if (bytes_received >= IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE - data_length) {
            printf("HTTP Receive buffer empty! Increase IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE\r\n");
            // just return bad status so that the user can print what we have so far with += bytes_received below
            status = NX_OVERFLOW;
        }

    	nx_packet_release(receive_packet);
    	receive_packet = NULL;
		data_length += bytes_received;
		printf("HTTP: %lu bytes received.\r\n", bytes_received);
    }
    r->response[data_length] = 0; // terminate the string
    if (receive_packet) {
    	nx_packet_release(receive_packet);
    }
    
cleanup:
    current_request = NULL;

    UINT delete_status = nx_web_http_client_delete(&http_client);
    if (delete_status != NX_SUCCESS) {
        printf("Warning to delete web client: 0x%x\r\n", status);
    }

    return status;
}

static ULONG iotc_https_certificate_verify(NX_SECURE_TLS_SESSION *session, NX_SECURE_X509_CERT* certificate) {
    (void) session; // unused
    UINT status = nx_secure_x509_common_name_dns_check(
    		certificate,
			(const UCHAR *)current_request->host_name,
			(UINT)strlen(current_request->host_name));

    if (status != NX_SUCCESS) {
        printf("HTTP failed to set TLS common name DNS check, error: 0x%x\r\n", status);
        return (status);
    }

    return(status);
}

/* Callback to setup TLS parameters for secure HTTPS. */
static UINT tls_setup_callback(NX_WEB_HTTP_CLIENT *client_ptr, NX_SECURE_TLS_SESSION *tls_session) {
    UINT status;

    NX_PARAMETER_NOT_USED(client_ptr);
#ifdef IOTC_HTTP_RAM_USAGE_HACK
    const size_t metadata_size = NX_AZURE_IOT_TLS_METADATA_BUFFER_SIZE;
#else
    const size_t metadata_size = IOTCONNECT_HTTPS_TLS_BUFFERSIZE;
#endif
    /* Initialize and create TLS session. */
    status = nx_secure_tls_session_create(tls_session, &nx_crypto_tls_ciphers, crypto_client_metadata,
    		metadata_size);

    if (status) {
        printf("HTTP failed to create TLS session, error: 0x%x\r\n", status);
        return (status);
    }

    /* Allocate space for packet reassembly. */
    status = nx_secure_tls_session_packet_buffer_set(tls_session, tls_packet_buffer, sizeof(tls_packet_buffer));

    if (status != NX_SUCCESS) {
        printf("HTTP failed to set TLS buffer size, error: 0x%x\r\n", status);
        return (status);
    }

    /* Add a CA Certificate to our trusted store for verifying incoming server certificates. */
    status = nx_secure_x509_certificate_initialize(&trusted_certificate,
            (UCHAR*) current_request->tls_cert,
            (USHORT) current_request->tls_cert_len,
            NX_NULL,
            0,
            NULL,
            0,
            NX_SECURE_X509_KEY_TYPE_NONE);

    if (status != NX_SUCCESS) {
        printf("HTTP failed to initialize the TLS certificate, error: 0x%x\r\n", status);
        return (status);
    }
    /* Initialize DNS name. */
    status = nx_secure_x509_dns_name_initialize(
    		&dns_name,
			(const UCHAR *)current_request->host_name,
			(UINT)strlen(current_request->host_name));
    if (status != NX_SUCCESS) {
		printf("HTTP failed to initialize the DNS name, error: 0x%x\r\n", status);
		return (status);
    }

    status = nx_secure_tls_session_sni_extension_set(tls_session, &dns_name);
    if (status != NX_SUCCESS) {
		printf("HTTP failed to set SNI extension, error: 0x%x\r\n", status);
		return (status);
    }

    status = nx_secure_tls_session_certificate_callback_set(
    		tls_session,
			iotc_https_certificate_verify);
	if (status != NX_SUCCESS) {
		printf("HTTP failed to set TLS cert callback, error: 0x%x\r\n", status);
		return(status);
	}

    nx_secure_tls_trusted_certificate_add(tls_session, &trusted_certificate);

    /* Need to allocate space for the certificate coming in from the remote host. */
    nx_secure_tls_remote_certificate_allocate(tls_session, &remote_certificate, remote_cert_buffer,
            sizeof(remote_cert_buffer));
    nx_secure_tls_remote_certificate_allocate(tls_session, &remote_issuer, remote_issuer_buffer,
            sizeof(remote_issuer_buffer));

    return (NX_SUCCESS);
}

