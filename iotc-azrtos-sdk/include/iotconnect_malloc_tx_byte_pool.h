//
// Copyright: Avnet 2023
// Created by Nik Markovic <nikola.markovic@avnet.com> on 6/7/23.
//

#ifndef IOTCONNECT_MALLOC_TX_BYTE_POOL
#define IOTCONNECT_MALLOC_TX_BYTE_POOL
#include "tx_port.h"
/*
This is a slightly rough implementation of routing malloc to tx_byte_pool.
In order to use this implementation:
 o The byte pool of IOTCONNECT_MALLOC_BUFFER_SIZE (6k currently)
 but the user can override the define with the same compile-time define.
 o Ensure that no malloc calls are being executed before you call malloc_byte_pool_allocate().
 This part can be tricky because many standard C library functions may call malloc under the hood.
 An example of such function is rand() or srand();
 o Call malloc_byte_pool_allocate() early during startup.

 IMPORTANT NOTE:
 The realloc implementation has no information about the original block size.
 Therefore, it will copy the data from the original block with new block size. That means
 that if the original block was smaller than the new block, the malloc will copy garbage
 that it encounters past the limit of the original block and put it into the new block.
 This condition in the current code is never triggered, however this may change if other code
 is introduced. cJSON is the only current user or realloc and it always uses it to
 increase the size of the allocated block.
 This issue is "generally" still safe, but under certain conditions, it could cause problems:
  o If old block was at the end of addressable memory, the copy could extend past the limit of
  the addressable memory and trigger a hardware exception.
  o Though highly unlikely to be exploitable, this issue could technically be used as a clever
  security exploit for one bit of code to gain access to other bit of code's data.
*/

UINT malloc_byte_pool_allocate(void);

#endif // IOTCONNECT_MALLOC_TX_BYTE_POOL


