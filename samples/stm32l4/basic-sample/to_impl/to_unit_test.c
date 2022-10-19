/**
 * @file    main_mqtt_aws_app.c
 * @brief   Checks for libTO basic features.
 * @copyright Copyright (C) Avnet. All rights reserved.
 *            Developed at Avnet EMG France, 16 Av Carnot, 91300 MASSY France.
 *            Reproduction, copy, modification in whole or part is prohibited
 *            without the written permission of the copyright owner.
 * @author  Laurent Lagosanto
 */


// ==============================================================================
//                   STANDARD HEADER FILES
// ==============================================================================
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

// ==============================================================================
//                   PROJECT HEADER FILES
// ==============================================================================
#include "TO.h"
#include "TO_helper.h"

#define MAX_CERT_NUMBER (1 << TO_INDEX_SIZE * 8)

static const char hex_digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

static size_t hexdump (char *out, size_t out_len, const uint8_t *bytes, size_t len)
{
    if (out == NULL)
    {
        TO_LOG_ERR("hexdump expects a non-NULL output buffer");
        return 0;
    }
    if (out_len < ((2 * len) + 1))
    {
        TO_LOG_ERR("hexdump expects a big enough output buffer (out_len >= %d) ", ((2 * len) + 1));
        return 0;
    }

    if ((len == 0) || (bytes == NULL))
    {
        out[0] = 0;

        return 0;
    }
    else
    {
        uint32_t pos = 0;

        while (out_len && len)
        {
            *out = hex_digits[(*bytes & 0xF0) >> 4];
            ++out;
            ++pos;
            --out_len;

            *out = hex_digits[*bytes & 0x0F];
            ++out;
            ++pos;
            --out_len;

            --len;
            ++bytes;
        }
        *out = '\0';

        return pos;
    }
}

void check_libTO(void)
{
    TO_ret_t     ret;
    TOSE_ctx_t * se_ctx;

    // Retrieve driver instance
    se_ctx = TODRV_HSE_get_ctx();

    // change this if required
    TO_set_log_level(TO_log_get_ctx(), TO_LOG_LEVEL_INF, TO_log);

    // Initialize
    ret = TOSE_init(se_ctx);

    if (ret != TO_OK)
    {
        TO_LOG_ERR("TOSE_init <error %04X>\n", ret);
        return;
    }

    // Read TO136 software version
    {
        uint8_t  major;
        uint8_t  minor;
        uint8_t  revision;

        ret = TOSE_get_software_version(se_ctx, &major, &minor, &revision);

        if (ret != TORSP_SUCCESS)
        {
            TO_LOG_ERR("TO136  FW: <error %04X>\n", ret);
        }
        else
        {
            TO_LOG_INF("TO136  FW: %d.%d.%d\n", major, minor, revision);
        }
    }

    // Read TO136 CSN
    {
        uint8_t  serial_number[TO_SN_SIZE];

        ret = TOSE_get_serial_number(se_ctx, serial_number);

        if (ret != TORSP_SUCCESS)
        {
            TO_LOG_ERR("TO136 CSN: <error %04X>\n", ret);
        }
        else
        {
            char hexstr[1 + 2 * TO_SN_SIZE];
            hexdump(hexstr, sizeof(hexstr), serial_number, TO_SN_SIZE);
            TO_LOG_INF("TO136 CSN: %s\n", hexstr);
        }
    }

    {
        uint8_t  hardware_serial_number[TO_HW_SN_SIZE];

        // Read TO136 HSN
        ret = TOSE_get_hardware_serial_number(se_ctx, hardware_serial_number);

        if (ret == TORSP_UNKNOWN_CMD)
        {
            TO_LOG_WRN("TO136 HSN: <not supported>\n");
        }
        else if (ret != TORSP_SUCCESS)
        {
            TO_LOG_ERR("TO136 HSN: <error %04X>\n", ret);
        }
        else
        {
            char hexstr[1 + 2 * TO_HW_SN_SIZE];
            hexdump(hexstr, sizeof(hexstr), hardware_serial_number, TO_HW_SN_SIZE);
            TO_LOG_INF("TO136 HSN: %s\n", hexstr);
        }
    }

    // Read CN for each slot
    {
        uint8_t cn_index = 0;
        while(cn_index < MAX_CERT_NUMBER && TORSP_ARG_OUT_OF_RANGE != ret)
        {
            char cn[TO_CERT_SUBJECT_CN_MAXSIZE + 1];

            ret = TOSE_get_certificate_subject_cn(se_ctx, cn_index, cn);
            if (ret == TO_NOT_IMPLEMENTED)
            {
                TO_LOG_ERR("TO136 CN%i: <not implemented>\n", cn_index);
                break;
            }
            else if (ret == TORSP_ARG_OUT_OF_RANGE)
            {
                break;
            }
            // Nik: Added these
            else if (ret == TO_DEVICE_WRITE_ERROR)
            {
            	TO_LOG_ERR("TO136 CN%i: TO_DEVICE_WRITE_ERROR\n", cn_index);
                break;
            }
            else if (ret == TO_DEVICE_READ_ERROR)
            {
            	TO_LOG_ERR("TO136 CN%i:TO_DEVICE_WRITE_ERROR\n", cn_index);
                break;
            }
            else if (ret != TORSP_SUCCESS)
            {
                TO_LOG_ERR("TO136 CN%i: <error %04X>\n", cn_index, ret);
                break;
            }
            else
            {
                TO_LOG_INF("TO136 CN%i: %s\n", cn_index, cn);
            }

            cn_index++;
        }
    }


#if 1
    // Read x509 Cert for each slot
    for (uint8_t i = 0; i < sizeof(i) << 8; i++)
    {
        static uint8_t cert[TO_CERT_X509_MAXSIZE];
        uint16_t       len = TO_CERT_X509_MAXSIZE;

        ret = TOSE_helper_get_certificate_x509_and_sign(se_ctx, i, NULL, 0, cert, &len, NULL);

        if (ret == TORSP_ARG_OUT_OF_RANGE)
        {
            break;
        }
        else if (ret != TO_OK)
        {
            TO_LOG_ERR("TO136 CERT%d: <error %04X>\n", i, ret);
            break;
        }
        else
        {
            static char hexstr[1 + 2 * TO_CERT_X509_MAXSIZE];
            hexdump(hexstr, sizeof(hexstr), cert, len);

            TO_LOG_INF("TO136 CERT%i: %s\n", i, hexstr);
        }
    }
#endif

    // Read Cert short for each slot
    for (uint8_t i = 0; i < sizeof(i) << 8; i++)
    {
        static TO_cert_short_t cert;

        ret = TOSE_get_certificate(se_ctx, i, TO_CERTIFICATE_SHORT, (uint8_t *) &cert);

        if (ret == TORSP_ARG_OUT_OF_RANGE)
        {
            break;
        }
        else if (ret != TORSP_SUCCESS)
        {
            TO_LOG_ERR("TO136 CERT%i: <error %04X>\n", i, ret);
            break;
        }
        else
        {
            static char hexstr[2 * TO_ECC_PUB_KEYSIZE + 1];
            size_t      pos = 0;
            pos = hexdump(hexstr, sizeof(hexstr), cert.ca_id, TO_SN_CA_ID_SIZE);
            hexdump(hexstr + pos, sizeof(hexstr) - pos, cert.serial_number, TO_SN_NB_SIZE);
            TO_LOG_INF("TO136 CERT%i: serial=%s\n", i, hexstr);
            hexdump(hexstr, sizeof(hexstr), cert.public_key, TO_ECC_PUB_KEYSIZE);
            TO_LOG_INF("TO136 CERT%i: pubkey=%s\n", i, hexstr);
        }
    }

    ret = TOSE_fini(se_ctx);

    if (ret != TO_OK)
    {
        TO_LOG_ERR("TOSE_fini <error %04X>\n", ret);
    }
}
