
/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

//
// Copyright: Avnet, Softweb Inc. 2020
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 6/15/20.
//  - Devised an algorithm to construct a DER based on nx_crypto_ecdsa.c

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static int validate_sig_to_asn1_der(
		uint8_t *sig_buf, // where to put the signature
		size_t sig_buff_len, //
		uint8_t* s, // large integer #1
		uint8_t* r, // large integer #2
		size_t key_size_in_bits // number of (curve?) bits
) {
	if (!sig_buf || !s || !r) {
		printf("sig_to_asn1_der: Error: Pointers need to be valid\r\n");
		return -1;
	}
	size_t ln_size = key_size_in_bits / 8;
	if (sig_buff_len < 2 * ln_size + 10 /* TODO: Refine this */ ) {
		printf("sig_to_asn1_der: Error: Crypto buffer too small\r\n");
	}
	return 0;
}
int sig_to_asn1_der(
		uint8_t *signature, // where to put the signature
		size_t sig_buff_len, //
		size_t* signature_length, //
		uint8_t* signature_r, // large integer #1
		uint8_t* signature_s, // large integer #2
		size_t key_size_in_bits // number of (curve?) bits
) {
	if (validate_sig_to_asn1_der(signature, sig_buff_len, signature_r, signature_r, key_size_in_bits)) {
		return -1;
	}
    /* Signature format follows ASN1 DER encoding as per RFC 4492, section 5.8:
     * Size: 1   | 1 or 2 | 1   |   1   | 0 or 1 | N |  1  |  1   | 0 or 1 | M
     * Data: SEQ |  Size  | INT |  Size | 0x00   | r | INT | Size | 0x00   | s  */
	size_t large_int_size = key_size_in_bits / 8;
	size_t r_size = large_int_size;
	size_t s_size = large_int_size;
	size_t pad_zero_r = (signature_r[0] & 0x80) ? 1 : 0;
	size_t pad_zero_s = (signature_s[0] & 0x80) ? 1 : 0;
	size_t sequence_size = large_int_size * 2 + pad_zero_r + pad_zero_s + 4;
	signature[0] = 0x30; /* SEQUENCE */
	if (sequence_size < 0x80) {
		signature[1] = (uint8_t) sequence_size;
		signature += 2;
		*signature_length = sequence_size + 2;
	} else {
		signature[1] = 0x81;
		signature[2] = (uint8_t) sequence_size;
		signature += 3;
		*signature_length = sequence_size + 3;
	}

	signature[0] = 0x02;
	signature[1] = (uint8_t) (r_size + pad_zero_r);
	if (pad_zero_r) {
		signature[2] = 0;
	}
	memcpy(&signature[2 + pad_zero_r], signature_r, r_size);
	signature += 2 + pad_zero_r +  r_size;

	signature[0] = 0x02;
	signature[1] = (uint8_t) (s_size + pad_zero_s);
	if (pad_zero_s) {
		signature[2] = 0;
	}
	memcpy(&signature[2 + pad_zero_s], signature_s, s_size);
	// done here so no need for: signature += 2 + pad_zero_s +  s_size;

	return 0;
}

int split_cn_to_cpid_and_duid(char* cn, char** cpid, char** duid) {
	if (!cn || !cpid || !duid) {
		return -1;
	}
	for (int i = 0; i < strlen(cn); i++) {
		if (cn[i] == '-') {
			*cpid = cn;
			cn[i] = 0;
			*duid = &cn[i+1];
			return 0;
		}
	}
	return -3; // dash not found

}
