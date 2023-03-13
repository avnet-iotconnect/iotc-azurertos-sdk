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
// Copyright: Avnet 2021
// Modified by Nik Markovic <nikola.markovic@avnet.com> on 4/19/21.
//

#ifndef NX_AZURE_IOT_CERT_H
#define NX_AZURE_IOT_CERT_H

/* Determine if a C++ compiler is being used.  If so, ensure that standard
   C is used to process the API information.  */

#ifdef __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif


/* Baltimore Root for IoTHub connection and any microsoft blob connections  */
extern const unsigned char IOTCONNECT_BALTIMORE_ROOT_CERT[];
extern const unsigned int IOTCONNECT_BALTIMORE_ROOT_CERT_SIZE;

/* Godaddy Secure G2 for IoTConnect Discovery and Sync REST services */
extern const unsigned char IOTCONNECT_GODADDY_G2_ROOT_CERT[];
extern const unsigned int IOTCONNECT_GODADDY_G2_ROOT_CERT_SIZE;

/* Digicert Global Root G2*/
extern const unsigned char IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2[];
extern const unsigned int IOTCONNECT_DIGICERT_GLOBAL_ROOT_G2_SIZE;

/* Microsoft RSA Root CA 2017 */
extern const unsigned char IOTCONNECT_MICROSOFT_RSA_ROOT_CA_2017[];
extern const unsigned int IOTCONNECT_MICROSOFT_RSA_ROOT_CA_2017_SIZE;

#ifdef __cplusplus
}
#endif

#endif /* NX_AZURE_IOT_CERT_H */
