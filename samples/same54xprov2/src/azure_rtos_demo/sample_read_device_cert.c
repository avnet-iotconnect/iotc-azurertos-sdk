#include "cryptoki.h"
#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <string.h>
#include <stdlib.h>
#include "definitions.h"                // SYS function prototypes
#include "cryptoauthlib.h"              //must be manually added for now
#include "atca_test.h"
#include "nx_crypto.h"
#include "nx_crypto_ec.h"

#include "atcacert/atcacert.h"
#include "atcacert/atcacert_def.h"
#include "atcacert/atcacert_pem.h"
#include "atcacert/atcacert_host_hw.h"
#include "atcacert/atcacert_client.h"

#include "../cust_def_1_signer.h"
#include "../cust_def_2_device.h"
#include "tng/tngtls_cert_def_1_signer.h"
#include "tng/tflxtls_cert_def_4_device.h"
#include "sample_azure_iot_embedded_sdk/nx_azure_iot_cert.h"
#include "sample_azure_iot_embedded_sdk/nx_azure_iot_ciphersuites.h"
#include "sample_azure_iot_embedded_sdk/sample_config.h"


/** \brief global storage for signer certificate */
#define REGISTRATION_ID_MAX_LEN     32
#define CERT_BUF_MAX_SIZE           1024

static char pkcs11_trust_device_label[] = "device";

UCHAR g_device_cert[CERT_BUF_MAX_SIZE];
UINT  g_device_cert_size = sizeof(g_device_cert);

UCHAR g_registration_id[REGISTRATION_ID_MAX_LEN] = {0};
UINT g_registration_id_length;

#if 0
extern UCHAR *sample_device_cert_ptr;
extern UINT sample_device_cert_len;
extern UCHAR *sample_device_private_key_ptr;
extern UINT sample_device_private_key_len;
extern ATCAIfaceCfg atecc608_0_init_data;

CK_RV nx_crypto_find_pkcs11_object(CK_SESSION_HANDLE session_handle, CK_ATTRIBUTE_PTR template,
                CK_OBJECT_HANDLE_PTR obj_handle_ptr);

int atca_get_device_cert(const atcacert_def_t *cert_def_1_signer,
                                const atcacert_def_t *cert_def_2_device)
{
    int ret;
    uint8_t signer_public_key[64] = {0};
    ATCA_STATUS status;

    status = atcab_init(&atecc608_0_init_data);
    if (status != ATCA_SUCCESS)
    {
        printf("atcab_init() failed: %02x\r\n", status);
        return((int)status);
    }

    do
    {
        status = atcab_read_pubkey(cert_def_1_signer->public_key_dev_loc.slot, signer_public_key);
        if (status)
        {
            printf("atcab_read_pubkey failed with an error %d\r\n", status);
            ret = (int)status;
            break;
        }
        g_device_cert_size = sizeof(g_device_cert);
        ret = atcacert_read_cert(cert_def_2_device, signer_public_key, g_device_cert, &g_device_cert_size);
        if(ATCACERT_E_SUCCESS != ret)
        {
            printf("atcacert_read_cert failed with an error %d\r\n", ret);
            break;
        }

#ifdef ENABLE_PRINT_CERTIFICATE
        char displaystr[CERT_BUF_MAX_SIZE];
        size_t displaylen;
        displaylen = sizeof(displaystr);
        atcacert_encode_pem_cert(g_device_cert, g_device_cert_size, displaystr, &displaylen);
        printf("CLIENT: Rebuilt Device Certificate: \r\n%s\r\n", displaystr);
#endif

        sample_device_cert_ptr = g_device_cert;
        sample_device_cert_len = g_device_cert_size;
        printf("device cert size is %u\n", sample_device_cert_len);
        
    }while(0);

    atcab_release();
    return ret;
}

CK_RV pkcs11_get_device_cert(void)
{
    CK_C_INITIALIZE_ARGS  cinit_args;
    CK_RV   rv;
    CK_OBJECT_HANDLE device_cert;

    memset( &cinit_args, 0x0, sizeof(cinit_args) );
    cinit_args.flags = CKF_OS_LOCKING_OK;

    rv = C_Initialize( &cinit_args );
    if (rv != CKR_OK && rv != CKR_CRYPTOKI_ALREADY_INITIALIZED)
    {
        printf("C_Initialize: rv = 0x%.8lX\n", rv);
        return(rv);
    }

    /* There is only one device slot-id 0 on atecc608b*/
    CK_SLOT_ID slotID = 0;

    /* Open a session on the slot found */
    CK_SESSION_HANDLE hSession;

	rv = C_OpenSession(slotID, CKF_SERIAL_SESSION, NULL_PTR, NULL_PTR,
	    &hSession);

	if (rv != CKR_OK)
    {
		printf("C_OpenSession: rv = 0x%.8lX\n", rv);
		goto exit_program;
	}
  
	/* create a privateKey handle for slot 0 */
    CK_OBJECT_CLASS class_device_cert_val = CKO_CERTIFICATE;
    


    CK_ATTRIBUTE device_cert_template[] = {
            {CKA_CLASS,     &class_device_cert_val, sizeof(CK_OBJECT_CLASS)},
            {CKA_LABEL, pkcs11_trust_device_label, strlen(pkcs11_trust_device_label)}
    };

    rv = nx_crypto_find_pkcs11_object(hSession, device_cert_template, &device_cert);
    if (rv != CKR_OK) {
		printf("find_object for device_cert: rv = 0x%.8lX\n", rv);
		goto exit_session;
	}
    
    CK_ATTRIBUTE device_cert_template_1[] = {
            {CKA_VALUE, g_device_cert, g_device_cert_size}
    };

    rv = C_GetAttributeValue(hSession, device_cert, device_cert_template_1, 1);
    if (rv != CKR_OK) {
		printf("C_GetAttributeValue for device_cert: rv = 0x%.8lX\n", rv);
		goto exit_session;
	}

    g_device_cert_size = device_cert_template_1[0].ulValueLen;
    sample_device_cert_ptr = g_device_cert;
    sample_device_cert_len = g_device_cert_size;

#ifdef ENABLE_PRINT_CERTIFICATE
    char displaystr[1024];
    size_t displaylen;
    displaylen = sizeof(displaystr);
    atcacert_encode_pem_cert(g_device_cert, g_device_cert_size, displaystr, &displaylen);
    printf("CLIENT: Rebuilt Device Certificate: \r\n%s\r\n", displaystr);
#endif

exit_session:
	(void) C_CloseSession(hSession);

exit_program:
    //(void) C_Finalize(NULL_PTR);
    return(rv);
}

UINT get_subject_common_name(void)
{
NX_SECURE_X509_CERT dev_certificate;
UINT nx_status;

    nx_status = nx_secure_x509_certificate_initialize(&dev_certificate,
                                (UCHAR *)sample_device_cert_ptr, (USHORT)sample_device_cert_len,
                                NX_NULL, 0,
                                (UCHAR *)sample_device_private_key_ptr, (USHORT)sample_device_private_key_len,
                                DEVICE_KEY_TYPE);
    if (nx_status)
    {
        printf("nx_secure_x509_certificate_initialize failed with status %d\r\n", nx_status);
        return(nx_status);
    }
    
    g_registration_id_length = dev_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name_length;
    if (g_registration_id_length < REGISTRATION_ID_MAX_LEN)
    {
        NX_CRYPTO_MEMCPY(g_registration_id, dev_certificate.nx_secure_x509_distinguished_name.nx_secure_x509_common_name, g_registration_id_length);
    }
    else
    {
        return(NX_SIZE_ERROR);
    }
    printf("Device Certificate Subject Common Name is %.*s, length is %u\r\n\n",
                g_registration_id_length,
                g_registration_id,
                g_registration_id_length);
    return(NX_SUCCESS);
}

UINT sample_read_device_cert(void)
{
    /*
     * To connect this device with Azure IoT Hub and DPS, the device_id must be
     * the same as certificate subject Common Name (CN). Also, there must have no space
     * in CN.
     * 
     * Some old chip's cert CN has space, then the customer need to use cryptoauth_trustplatform_designsuite
     * to re-provision a new device cert based on slot-0's private key, and store it to device slot-10
     * 
     * Currently this customized certificate can only be obtained by using native atcacert apis.
     * The factory provisoned cert can be obtained by both PKCS11 and native APIs.
     * 
     * Usually we should first try to read factory provisoned device certificate,
     * If it doesn't exist, then try to re-provison it and then get customer re-provisioned certificate.
     * 
     * But the cryptoauthlib native apis and PKCS11 apis cannot be used mixedly.
     * 
     * So by default, we try to get factory provisioned certificate with PKCS11 APIs.
     * If you need to use customer re-provisioned API, pls define GET_CUSTOMER_RE_PROVISONED_CERTIFICATE
     * and rebuild the demo. 
     */

#ifndef GET_CUSTOMER_RE_PROVISONED_CERTIFICATE
    CK_RV rv = pkcs11_get_device_cert();
    if (rv)
    {
        printf("Failed to get device certificate\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
#else
    /* Customer re-provisoned cert can only be obtained by native APIs. */
    int ret = atca_get_device_cert(&g_cert_def_1_signer, &g_cert_def_2_device);
    if (ret)
    {
        printf("Failed to get device certificate\r\n");
        return(NX_NOT_SUCCESSFUL);
    }
#endif
    
    /* Get common name, which is used as DPS Registration_ID and IoT Hub Device_ID*/
    if (get_subject_common_name() != NX_SUCCESS)
    {
        printf("Failed to subject_common_name\r\n");
        return(NX_NOT_SUCCESSFUL);
    }

    return(NX_SUCCESS);
}
#endif