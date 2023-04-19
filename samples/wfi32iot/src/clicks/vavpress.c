/****************************************************************************
** Copyright (C) 2020 MikroElektronika d.o.o.
** Contact: https://www.mikroe.com/contact
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
** OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
**  USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************************/

/*!
 * @file vavpress.c
 * @brief VAV Press Click Driver.
 */

#include "vavpress.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

vavpress_return_value_t VAVPRESS_status;
vavpress_sensor_param_data_t VAVPRESS_param_data;
vavpress_el_signature_data_t VAVPRESS_el_signature_data;

int16_t VAVPRESS_2sCompToDecimal(uint16_t twos_compliment_val)
{
    // [0x0000; 0x7FFF] corresponds to [0; 32,767]
    // [0x8000; 0xFFFF] corresponds to [-32,768; -1]
    // int16_t has the range [-32,768; 32,767]

    uint16_t sign_mask = 0x8000;

    // if positive
    if ( (twos_compliment_val & sign_mask) == 0 ) {
        return twos_compliment_val;
    //  if negative
    } else {
        // invert all bits, add one, and make negative
        return -(~twos_compliment_val + 1);
    }
}

uint16_t VAVPRESS_reorderBytes(uint16_t word)
{
    uint16_t bytes_swapped;
    
    bytes_swapped = ( ((word << 8 ) & 0xFF00) | ((word >> 8) & 0x00FF) );
    
    return (bytes_swapped);
}

void VAVPRESS_init(void)
{
    vavpress_return_value_t error_code;

    for (int index = 0; index < EL_SIGNATURE_NUMBYTES; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[0] = 0;
    }
    APP_SENSORS_writeByte(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_RESET_FIRMWARE);
    error_code = VAVPRESS_setDefaultConfig();
    if (error_code == VAVPRESS_OK)
    {
        VAVPRESS_status = VAVPRESS_OK;
        //printf("--------------------------------\r\n" );
        //printf(" Firmware Version : %.3f        \r\n", VAVPRESS_el_signature_data.firmware_version);
        //printf(" Pressure Range   : %d Pa       \r\n", VAVPRESS_el_signature_data.pressure_range);
        //printf(" Part #           : %.11s       \r\n", VAVPRESS_el_signature_data.part_number);
        //printf(" Lot #            : %.7s        \r\n", VAVPRESS_el_signature_data.lot_number);
        //printf(" Output Type      : %c          \r\n", VAVPRESS_el_signature_data.output_type);
        //printf(" Scale Factor     : %d          \r\n", VAVPRESS_el_signature_data.scale_factor);
        //printf(" Calibration ID   : %.2s        \r\n", VAVPRESS_el_signature_data.calibration_id);
        //printf(" Week Number      : %d          \r\n", VAVPRESS_el_signature_data.week_number);
        //printf(" Year Number      : %d          \r\n", VAVPRESS_el_signature_data.year_number);
        //printf(" Sequence Number  : %d          \r\n", VAVPRESS_el_signature_data.sequence_number);
        //printf("--------------------------------\r\n" );
        VAVPRESS_param_data.scale_factor_temp = 72;
        VAVPRESS_param_data.scale_factor_press = VAVPRESS_el_signature_data.scale_factor;
        VAVPRESS_param_data.readout_at_known_temperature = 50;
        VAVPRESS_param_data.known_temperature_c = 24.0;
    }
    else
    {
        VAVPRESS_status = VAVPRESS_ERROR;
        printf("[VAV Click] LMIS025B was not found during initialization\r\n");
    }
    APP_SENSORS_writeByte(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_START_PRESSURE_CONVERSION);
}

vavpress_return_value_t VAVPRESS_setDefaultConfig(void)
{
    vavpress_return_value_t error_code;
    vavpress_sensor_param_data_t param_data;
    
    VAVPRESS_param_data.scale_factor_temp = 72;
    VAVPRESS_param_data.scale_factor_press = 1200;
    VAVPRESS_param_data.readout_at_known_temperature = 50;
    VAVPRESS_param_data.known_temperature_c = 24.0;
    
    error_code = VAVPRESS_setDefaultSensorParams(&param_data);
    return error_code;
}

vavpress_return_value_t VAVPRESS_setDefaultSensorParams(vavpress_sensor_param_data_t *param_data)
{
    vavpress_return_value_t error_flag = VAVPRESS_getElectronicSignature(&VAVPRESS_el_signature_data);
    
    param_data->scale_factor_temp = 72;
    param_data->scale_factor_press = VAVPRESS_el_signature_data.scale_factor;
    param_data->readout_at_known_temperature = 105;
    param_data->known_temperature_c = 23.1;
    
    return error_flag;
}

vavpress_return_value_t VAVPRESS_getSensorReadings(vavpress_sensor_param_data_t *param_data, float *diff_press, float *temperatureC)
{
    int16_t word_data, pressure, tempC;
    float tmp;

    APP_SENSORS_justRead(VAVPRESS_I2CADDR_0, EXTENDED_READOUT_NUMBYTES);

    word_data = ( (APP_SENSORS_data.i2c.rxBuffBytes[1] << 8) | APP_SENSORS_data.i2c.rxBuffBytes[0] );
    pressure = VAVPRESS_2sCompToDecimal(word_data);
    tmp = ( float ) pressure;
    tmp /= ( float ) param_data->scale_factor_press;
    printf("w:%u p:%u s:%u f:%f \r\n", word_data, pressure, param_data->scale_factor_press, *diff_press);
    *diff_press = tmp;

    word_data = ( (APP_SENSORS_data.i2c.rxBuffBytes[3] << 8) | APP_SENSORS_data.i2c.rxBuffBytes[2] );
    tempC = VAVPRESS_2sCompToDecimal(word_data);
    tmp = ( float ) tempC;
    tmp -= ( float ) param_data->readout_at_known_temperature;
    tmp /= ( float ) param_data->scale_factor_temp;
    tmp += param_data->known_temperature_c;
    *temperatureC = tmp;

    return VAVPRESS_OK;
}

vavpress_return_value_t VAVPRESS_getElectronicSignature(vavpress_el_signature_data_t *el_signature_data)
{
    uint8_t rx_buf[EL_SIGNATURE_NUMBYTES];
    uint16_t tmp = 0;
    float tmp_f;

    APP_SENSORS_writeReadBytes(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_RETRIEVE_ELECTRONIC_SIGNATURE, EL_SIGNATURE_NUMBYTES);
    memcpy(rx_buf, APP_SENSORS_data.i2c.rxBuffBytes, EL_SIGNATURE_NUMBYTES);

    if ( rx_buf[ 1 ] < 10 ) {
        tmp_f = ( float ) rx_buf[ 0 ] + ( ( float ) rx_buf[ 1 ] / 10 );
    } else if ( rx_buf[ 1 ] < 100 ) {
        tmp_f = ( float ) rx_buf[ 0 ] + ( ( float ) rx_buf[ 1 ] / 100 );
    } else {
        tmp_f = ( float ) rx_buf[ 0 ] + ( ( float ) rx_buf[ 1 ] / 1000 );
    }
    el_signature_data->firmware_version = tmp_f;

    for ( uint8_t n_cnt = 0; n_cnt < 11; n_cnt++ ) {
        el_signature_data->part_number[ n_cnt ] = rx_buf[ n_cnt + 2 ];
    }

    for ( uint8_t n_cnt = 0; n_cnt < 7; n_cnt++ ) {
        el_signature_data->lot_number[ n_cnt ] = rx_buf[ n_cnt + 13 ];
    }

    tmp = rx_buf[ 20 ];
    tmp <<= 8;
    tmp |= rx_buf[ 21 ];
    el_signature_data->pressure_range = tmp;

    el_signature_data->output_type = rx_buf[ 22 ];

    tmp = rx_buf[ 23 ];
    tmp <<= 8;
    tmp |= rx_buf[ 24 ];
    el_signature_data->scale_factor = tmp;

    el_signature_data->calibration_id[ 0 ] = rx_buf[ 25 ];
    el_signature_data->calibration_id[ 1 ] = rx_buf[ 26 ];

    el_signature_data->week_number = rx_buf[ 27 ];

    el_signature_data->year_number = rx_buf[ 28 ];

    tmp = rx_buf[ 29 ];
    tmp <<= 8;
    tmp |= rx_buf[ 30 ];
    el_signature_data->sequence_number = tmp;

    if ( (tmp == 0) &&
         (el_signature_data->pressure_range == 0) &&
         (el_signature_data->scale_factor == 0) &&
         (el_signature_data->week_number == 0) &&
         (el_signature_data->year_number == 0) &&
         (el_signature_data->sequence_number == 0)
       )
    {
        return VAVPRESS_ERROR;
    }
    else
    {
        return VAVPRESS_OK;
    }
}

// ------------------------------------------------------------------------- END