//
// Copyright: Avnet 2021
// Created by Nik Markovic <nikola.markovic@avnet.com> on 5/24/21.
//
#include "nx_web_http_client.h"
#include "azrtos_https_client.h"
#include "iotconnect.h"
#include "azrtos_download_client.h"


#ifndef IOTC_DL_DATA_BUFFER_SIZE
#define IOTC_DL_DATA_BUFFER_SIZE 4096
#endif

#ifndef IOTC_DL_NUM_RETRIES
#define IOTC_DL_NUM_RETRIES 3
#endif


#define HDR_CONTENT_LENGTH_STR "Content-Length"
#define HDR_CONTENT_TYPE_STR "Content-Type"
#define HDR_CONTENT_TYPE_BNARY_STR "application/octet-stream"
#define HDR_RANGE_STR "Range"
#define HDR_RANGE_FORMAT "bytes=%u-%u"
#define HDR_CONNECTION "Connection"
#define HDR_CONNECTION_KEEPALIVE "keep-alive"
#define MAX_DATA_LENGTH_DIGITS 10 // 10 gigabytes.. Not that we really need this much, but to be empyrical about it...


static UCHAR buffer[IOTC_DL_DATA_BUFFER_SIZE];
static size_t data_length;
static size_t file_size;
static size_t file_bytes_received;

// event_cb and do_resume recorded from the main entry point and used by request_handler
static IotConnectDownloadHandler event_cb = NULL; // initialize so that first check works properly
static bool do_resume;
// ------

static UINT add_header(NX_WEB_HTTP_CLIENT *http_client, const char* name, const char *value) {
    return nx_web_http_client_request_header_add(http_client,
            (CHAR*) name, strlen(name),
            (CHAR*) value, strlen(value),
            NX_WAIT_FOREVER);
}

static UINT add_range_header(NX_WEB_HTTP_CLIENT *http_client, size_t start, size_t end) {
    char range_str_buff[7 + MAX_DATA_LENGTH_DIGITS * 2]; // length should support qute large ranges
    sprintf(range_str_buff, HDR_RANGE_FORMAT, start, end); // terminate after "bytes=";
    return add_header(http_client, HDR_RANGE_STR, range_str_buff);
}

static VOID header_range_callback(NX_WEB_HTTP_CLIENT *client_ptr, CHAR *field_name, UINT field_name_length,
                            CHAR *field_value, UINT field_value_length)
{
    // Don't do anything for range requests
    return;
}
static VOID header_file_size_callback(NX_WEB_HTTP_CLIENT *client_ptr, CHAR *field_name, UINT field_name_length,
                            CHAR *field_value, UINT field_value_length)
{
    // shortcut.. likely not the field we need
    if (field_name_length != sizeof(HDR_CONTENT_LENGTH_STR) - 1 /* String has null in it */ ) {
        return;
    }
    if (0 != memcmp(field_name, HDR_CONTENT_LENGTH_STR, field_name_length)) {
        return;
    }

    // make sure that value string is null terminated properly
    char content_length_buff[MAX_DATA_LENGTH_DIGITS + 1];
    memcpy(content_length_buff, field_value, field_value_length);
    content_length_buff[field_value_length] = 0;

    // workaround size_t reading as an int using %i
    int dummy;
    sscanf(content_length_buff, "%i",  &dummy);
    file_size = (size_t) dummy;
}

static UINT get_response(IotConnectHttpRequest *r, NX_WEB_HTTP_CLIENT *http_client) {
    /* Receive response data from the server. Loop until all data is received. */
    UINT status;
    NX_PACKET *receive_packet = NULL;
    UINT get_status = NX_SUCCESS;
    data_length = 0;
    while (get_status != NX_WEB_HTTP_GET_DONE) {
        get_status = nx_web_http_client_response_body_get(http_client, &receive_packet, NX_WAIT_FOREVER);

        /* Check for error.  */
        if (get_status != NX_SUCCESS && get_status != NX_WEB_HTTP_GET_DONE) {
            printf("download client: get packet failed, error: 0x%x\r\n", get_status);
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
                printf("download client: packet length was zero. Aborting the receive loop!");
            }
            break; // in both cases
        }
        ULONG bytes_received = 0;
        status = nx_packet_data_extract_offset( //
                receive_packet, // packet
                0, // offset in the packet
                &(buffer[data_length]), // where to put data
                IOTC_DL_DATA_BUFFER_SIZE - data_length /* 1 for null */, // bytes left in buffer
                &bytes_received);
        if (status) {
            printf("download client: Packet data extraction error: 0x%x\r\n", status);
            break;
        }
        if (bytes_received > IOTC_DL_DATA_BUFFER_SIZE - data_length) {
            printf("download client: Receive buffer empty! Increase IOTCONNECT_HTTP_RECEIVE_BUFFER_SIZE\r\n");
            // just return bad status so that the user can print what we have so far with += bytes_received below
            status = NX_OVERFLOW;
        }

        nx_packet_release(receive_packet);
        receive_packet = NULL;
        data_length += bytes_received;
#ifdef IOTC_DOWNLOAD_CLIENT_DEBUG
        printf("download client: %lu bytes received.\r\n", bytes_received);
#endif
    }
    if (receive_packet) {
        nx_packet_release(receive_packet);
    }
    return status;
}

static UINT get_bytes_in_range(IotConnectHttpRequest *r ,NX_WEB_HTTP_CLIENT *http_client, size_t start, size_t end) {
    UINT status;

    status = nx_web_http_client_request_initialize(http_client,
            NX_WEB_HTTP_METHOD_GET, /* GET, PUT, DELETE, POST, HEAD */
            r->resource, r->host_name, 0, /* PUT and POST need an input size. */
            NX_FALSE, /* If true, input_size is ignored. */
            NULL,
            NULL,
            NX_WAIT_FOREVER);
    if (status != NX_SUCCESS) {
        printf("download client: Error in HTTP HEAD request initialization: 0x%x\r\n", status);
        return status;
    }

    status = add_header(http_client, HDR_CONTENT_TYPE_STR, HDR_CONTENT_TYPE_BNARY_STR);
    if (status != NX_SUCCESS) {
        printf("download client: Error in HTTP request type headers setup: 0x%x\r\n", status);
        return status;
    }

    status = add_range_header(http_client, start, end);
    if (status != NX_SUCCESS) {
        printf("download client: Error in HTTP request range headers setup: 0x%x\r\n", status);
        return status;
    }

    // common for both GET and POST
    status = nx_web_http_client_request_send(http_client, NX_WAIT_FOREVER);
    if (status) {
        printf("download client: Error in HTTP request send: 0x%x\r\n", status);
        return status;
    }

    status = get_response(r, http_client); // we ignore response, but we want to get header callbacks

    return status;
}

static UINT get_file_size(IotConnectHttpRequest *r ,NX_WEB_HTTP_CLIENT *http_client) {
    UINT status;

    file_size = 0;
    status = nx_web_http_client_request_initialize(http_client,
            NX_WEB_HTTP_METHOD_HEAD, /* GET, PUT, DELETE, POST, HEAD */
            r->resource, r->host_name, 0, /* PUT and POST need an input size. */
            NX_FALSE, /* If true, input_size is ignored. */
            NULL,
            NULL,
            NX_WAIT_FOREVER);
    if (status != NX_SUCCESS) {
        printf("download client: Error in HTTP HEAD request initialization: 0x%x\r\n", status);
        return status;
    }

    status = add_header(http_client, HDR_CONNECTION, HDR_CONNECTION_KEEPALIVE);
    if (status != NX_SUCCESS) {
        printf("download client: Error in HTTP request connection headers setup: 0x%x\r\n", status);
        return status;
    }


    // common for both GET and POST
    status = nx_web_http_client_request_send(http_client, NX_WAIT_FOREVER);
    if (status) {
        printf("download client: Error in HTTP request send: 0x%x\r\n", status);
        return status;
    }

    // we ignore response data which should be empty, but we want to get header callbacks
    // to process file length
    status = get_response(r, http_client);

    return status;
}

static UINT request_handler(IotConnectHttpRequest *r ,NX_WEB_HTTP_CLIENT *http_client) {
    UINT status;

    // any failures will trickle down to iotc_download(). We don't worry about reporting these
    // This is just for reporting file size and data
    IotConnectDownloadEvent evt;

    status = nx_web_http_client_response_header_callback_set(http_client, header_file_size_callback);
    if (status) {
        printf("download client: Error in setting file size header callback: 0x%x\r\n", status);
        return status;
    }
    status = get_file_size(r, http_client);

    if (status != NX_SUCCESS) {
        return status;
    }
    printf("download client: Downloading %i bytes...\r\n", file_size);
    evt.type = IOTC_DL_FILE_SIZE;
    evt.file_size = file_size;
    if (false == event_cb(&evt)) {
        // NOTE: boolean return
        // the user failed somewhere and wants us to abort
        printf("Aborting download due to user request\r\n");
        return NX_DOWNLOAD_ABORTED_BY_USER; // NOTE: Custom error code

    }

    if (data_length > 0) {
        printf("download client: Warning: received actual data in HTTP HEAD response. Length: %i\r\n", data_length);
    }

    nx_web_http_client_response_header_callback_set(http_client, header_range_callback);
    if (status) {
        printf("download client: Error in setting range header callback: 0x%x\r\n", status);
        return status;
    }

    if (!do_resume) {
        // user wants to resume from where we left off
        file_bytes_received = 0;
    } else if (file_bytes_received >= file_size) {
        printf("download client: Invalid resume state!");
        return NX_INVALID_PARAMETERS;
    }

    do {
        size_t size;
        if (file_bytes_received + IOTC_DL_DATA_BUFFER_SIZE < file_size) {
            size = IOTC_DL_DATA_BUFFER_SIZE;
        } else{
            size = file_size - file_bytes_received;
        }
        size_t end = file_bytes_received + size - 1;
        for (int i = IOTC_DL_NUM_RETRIES; i > 0; i--) {
            status = get_bytes_in_range(r, http_client, file_bytes_received, end);
            if (NX_SUCCESS == status) {
#ifdef IOTC_DOWNLOAD_CLIENT_DEBUG
                printf("data: %u-%u\r\n", file_bytes_received, end);
#endif
                evt.type = IOTC_DL_DATA;
                evt.data.data_ptr = buffer;
                evt.data.data_size = size;
                evt.data.offset = file_bytes_received;
                evt.data.file_size = file_size;
                if (false == event_cb(&evt)) {
                    // NOTE: boolean return
                    // the user failed somewhere and wants us to abort
                    printf("Aborting download due to user request\r\n");
                    status = NX_DOWNLOAD_ABORTED_BY_USER;  // NOTE: Custom error code
                    break;
                }
                break;
            } else {
#ifdef IOTC_DOWNLOAD_CLIENT_DEBUG
                printf("failed to get range: %u-%u. tries left: %d\r\n", file_bytes_received, end, i - 1);
#endif
                ;
            }
        }
        if (status) {
            printf("download client: Failed to get bytes range: 0x%x\r\n", status);
            break;
        }
        file_bytes_received += size;
    } while (status == NX_SUCCESS && file_bytes_received < file_size);

    printf("download client: Total bytes received: %i\r\n", file_bytes_received);
    return status;
}

UINT iotc_download(IotConnectHttpRequest *r, IotConnectDownloadHandler event_callback, bool resume) {
    IotConnectDownloadEvent evt;
    evt.type = IOTC_DL_STATUS;

    // iotconnect_https_request() will do more checking,
    // but we need to do some basic check in order to print debug message below
    if (!r ||  !r->host_name || !r->resource || NULL == event_callback) {
        printf("download client: Invalid arguments\r\n");
        evt.status = NX_INVALID_PARAMETERS;
        if (event_callback) {
            event_callback(&evt);
        }
        return evt.status;
    }

    //NOTE: Global assignment. Can only have one download running at the time.
    if (event_cb) {
        printf("download client: Error: A download is already in progress!\r\n");
        evt.status = NX_NOT_SUPPORTED; // unable to support multiple downloads
        event_callback(&evt);
        return evt.status;
    }
    event_cb = event_callback;
    do_resume = resume;
    r->custom_handler_cb = request_handler;

#ifdef IOTC_DOWNLOAD_CLIENT_DEBUG
    printf("download client: Download started for host:%s resource:%s\r\n", r->host_name, r->resource);
#endif
    evt.status = iotconnect_https_request(r);
    event_cb(&evt);
    event_cb = NULL;
    return evt.status;

}
