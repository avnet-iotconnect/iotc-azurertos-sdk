//
// Copyright: Avnet 2022
// Created by Nik Markovic <nikola.markovic@avnet.com> on 4/9/21.
//

#ifndef IOTC_ALGORITHMS_H
#define IOTC_ALGORITHMS_H


#include <nx_crypto.h>
#include "iotconnect.h"

#ifdef __cplusplus
extern   "C" {
#endif

int sig_to_asn1_der(
		uint8_t *signature, // where to put the signature
		size_t sig_buff_len, //
		size_t* signature_length, //
		uint8_t* signature_r, // large integer #1
		uint8_t* signature_s, // large integer #2
		size_t key_size_in_bits // number of (curve?) bits
);

// this function will MODIFY the common name string to break the common name on the first dash into CPID and Device Unique ID
int split_cn_to_cpid_and_duid(char* cn, char** cpid, char** duid);

#ifdef __cplusplus
}
#endif

#endif // IOTC_ALGORITHMS_H
