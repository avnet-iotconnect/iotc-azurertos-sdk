/**
 * @file    wrapped_malloc.c
 * @brief   Checks for malloc/free functions and alikes..
 * @copyright Copyright (C) Avnet. All rights reserved.
 *            Developed at Avnet EMG France, 16 Av Carnot, 91300 MASSY France.
 *            Reproduction, copy, modification in whole or part is prohibited
 *            without the written permission of the copyright owner.
 * @author  Laurent Lagosanto
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

volatile int malloc_count = 0;
volatile int malloc_level = 0;

void *mallocs[1000];

void __checked_alloc(const char* op, void *ptr, size_t len)
{
	malloc_count++;
	if (ptr) {
		if (mallocs[malloc_level] != NULL) {
			printf("MEM: CHECK FAILED\r\n");
			while(1);
		}
		mallocs[malloc_level++] = ptr;
		printf("MEM: %s(%u): %p count=%d, level=%d\r\n", op, len, ptr, malloc_count, malloc_level);
	} else {
		printf("MEM: %s(%u): FAILED count=%d\r\n", op, len, malloc_count);
	}
}

void __checked_free(void *ptr)
{
	if (ptr == NULL) {
		printf("MEM: free(NULL)\r\n");
		return;
	}
	if (mallocs[--malloc_level] != ptr) {
		printf("MEM: NOT FREEING THE LAST MALLOC\r\n");
	}
	printf("MEM: free(%p) level=%d\r\n", ptr, malloc_level);

	int found = 0;
	for(int i=0; i<sizeof(mallocs); i++) {
		if (mallocs[i] == ptr) {
			size_t n = sizeof(mallocs) - (i + 1)*sizeof(mallocs[0]) ;
			memmove(&mallocs[i], &mallocs[i+1], n);
			found = 1;
			break;
		}
	}
	if (!found) {
		printf("MEM: FREEING SOMETHING THAT WAS NOT MALLOCED\r\n");
		while(1);
	}
}

extern void *__real_malloc(size_t len);
void *__wrap_malloc(size_t len)
{
	void *buf = __real_malloc(len);
	__checked_alloc("malloc", buf, len);
	return buf;
}

extern void *__real_calloc(size_t len);
void *__wrap_calloc(size_t len)
{
	void *buf = __real_calloc(len);
	__checked_alloc("calloc", buf, len);
	return buf;
}

extern void *__real_realloc(void *ptr, size_t len);
void *__wrap_realloc(void *ptr, size_t len)
{
	void *buf = __real_realloc(ptr, len);
	if (ptr == NULL) {
		__checked_alloc("realloc(NULL)", buf, len);
		return buf;
	}
	if (len == 0) {
		__checked_free(ptr);
		return NULL;
	}
	if (buf != ptr) {
		// handle this as a free + malloc
		__checked_free(ptr);
		__checked_alloc("realloc", buf, len);
	}
	return buf;
}

extern void __real_free(void *ptr);
void __wrap_free(void *ptr)
{
	__checked_free(ptr);
	__real_free(ptr);
}
