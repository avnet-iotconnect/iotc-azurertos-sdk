//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 6/7/23.
//

#include <malloc.h>
#include <stdio.h>
#include "tx_api.h"
#include "iotconnect_malloc_tx_byte_pool.h"

#ifdef IOTC_ROUTE_MALLOC_TO_TX_BYTE_POOL

#ifndef IOTC_MALLOC_BUFFER_SIZE
#define IOTC_MALLOC_BUFFER_SIZE (6 * 1024)
#endif

static UCHAR malloc_pool_buff[IOTC_MALLOC_BUFFER_SIZE];
static TX_BYTE_POOL malloc_pool;

UINT malloc_byte_pool_allocate(void) {
	UINT ret = tx_byte_pool_create(&malloc_pool,
		"Heap Memory Pool",
		(void *)malloc_pool_buff,
		sizeof(malloc_pool_buff)
	);
	if (TX_SUCCESS != ret) {
            printf("malloc_byte_pool_allocate: pool allocation failed!\r\n");
    }
    return ret;
}


void * malloc(size_t size) {
    void * ptr = NULL;

    if(size > 0) {
        UINT status = tx_byte_allocate(&malloc_pool, &ptr, size, TX_NO_WAIT);
        if(status != TX_SUCCESS)  {
            ptr = NULL;
        }
    }
    return ptr;
}

void free(void * ptr) {
    if(ptr) {
        UINT status = tx_byte_release(ptr);
        if (status != TX_SUCCESS) {
        	printf("tx_byte_release: error 0x%x\r\n", status);
        }
    }
}

void *calloc(size_t nmemb, size_t size) {
	void* ret = malloc(nmemb * size);
	if (!ret) {
		return NULL;
	}
	memset(ret, 0, nmemb * size);
	return ret;
}

void *realloc(void *ptr, size_t size) {
	if (!ptr)  {
		return NULL;
	}
	void* ret = malloc(size);
	if (ret) {
	    // NOTE that this is technically unsafe. See the header file!
		memcpy(ret, ptr, size);

	}
	// else return ret below, which will be null
	free(ptr);
	return ret;

}
#endif // IOTCONNECT_ROUTE_MALLOC_TO_TX_BYTE_POOL
