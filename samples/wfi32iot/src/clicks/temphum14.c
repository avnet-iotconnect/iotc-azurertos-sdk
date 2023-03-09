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
 * @file temphum14.c
 * @brief Temp Hum 14 Click Driver.
 */

#include "temphum14.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

static temphum14_diagn_t TEMPHUM14_statusData;

uint32_t TEMPHUM14_init ( uint8_t addr )
{
    TEMPHUM14_softReset( addr );
    tx_thread_sleep(5);
    for (int index = 0; index < TEMPHUM14_SERIAL_NUMBER_BYTES; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }   
    return (TEMPHUM14_getSerialNumber( addr ));
}

void temphum14_set_cmd ( uint8_t addr, uint8_t cmd )
{
    APP_SENSORS_writeByte( addr, cmd );
}

void TEMPHUM14_setConversion ( uint8_t addr, uint8_t hum_osr, uint8_t temp_osr )
{
    uint8_t tmp;
    
    hum_osr &= TEMPHUM14_BIT_MASK_HUM_OSR;
    temp_osr &= TEMPHUM14_BIT_MASK_TEMP_OSR;
    
    tmp = TEMPHUM14_CMD_CONVERSION;
    tmp |= hum_osr;
    tmp |= temp_osr;

    temphum14_set_cmd( addr, tmp );
    tx_thread_sleep(10);
}

void TEMPHUM14_getTemperatureHumidity ( uint8_t addr, float *temp, float *hum )
{
    uint8_t rx_buf[ (TEMPHUM14_TEMPERATURE_NUMBER_BYTES+TEMPHUM14_HUMIDITY_NUMBER_BYTES) ];
    uint16_t tmp;
    float temperature;
    float humidity;

    APP_SENSORS_writeReadBytes( addr, TEMPHUM14_CMD_READ_T_AND_RH, 
            (TEMPHUM14_TEMPERATURE_NUMBER_BYTES+TEMPHUM14_HUMIDITY_NUMBER_BYTES) );

    for (int index = 0; index <  (TEMPHUM14_TEMPERATURE_NUMBER_BYTES+TEMPHUM14_HUMIDITY_NUMBER_BYTES); index++)
    {
        rx_buf[ index ] = APP_SENSORS_data.i2c.rxBuffBytes[ index ];
    }
    
    tmp = rx_buf[ 0 ];
    tmp <<= 8;
    tmp |= rx_buf[ 1 ];
    
    temperature = ( float ) tmp;
    temperature /= TEMPHUM14_CALC_16_BIT_DIVIDER;
    temperature *= TEMPHUM14_CALC_TEMP_MULTI_FACT;
    temperature -= TEMPHUM14_CALC_TEMP_SUB_FACT;
    
    *temp = temperature;
    
    tmp = rx_buf[ 3 ];
    tmp <<= 8;
    tmp |= rx_buf[ 4 ];
    
    humidity = ( float ) tmp;
    humidity /= TEMPHUM14_CALC_16_BIT_DIVIDER;
    humidity *= TEMPHUM14_CALC_HUM_MULTI_FACT;
    
    *hum = humidity;
}

void TEMPHUM14_softReset ( uint8_t addr )
{
    temphum14_set_cmd( addr, TEMPHUM14_CMD_RESET );
}

void TEMPHUM14_setHeater ( uint8_t addr, uint8_t en_heater )
{
    if ( en_heater == TEMPHUM14_HEATER_ENABLE )
    {
        temphum14_set_cmd( addr, TEMPHUM14_CMD_HEATER_ON );
    } else
    {
        temphum14_set_cmd( addr, TEMPHUM14_CMD_HEATER_OFF );
    }
}

void TEMPHUM14_displayDiagnostic ( void ) {
    printf( "-----------------------------\r\n" );
    printf( "\r\n NVM Error        :" );
    if ( TEMPHUM14_statusData.nvm_error == TEMPHUM14_STATUS_ON ) {
        printf( " Error \r\n" );
    } else {
        printf( " No Error \r\n" );
    }

    printf( "\r\n Humidity U/O     :" );
    if ( TEMPHUM14_statusData.hum_un_over == TEMPHUM14_STATUS_ON ) {
        printf( " Under/Overrun \r\n" );
    } else {
        printf( " No Error \r\n" );
    }

    printf( "\r\n Humidity Error   :" );
    if ( TEMPHUM14_statusData.hum_h_err == TEMPHUM14_STATUS_ON ) {
        printf( " Below -10%% RH \r\n" );
    } else if ( TEMPHUM14_statusData.hum_l_err == TEMPHUM14_STATUS_ON ) {
        printf( " Above 120%% RH \r\n" );
    } else {
        printf( " No Error \r\n" );
    }
    
    printf( "\r\n Temperature U/O  :" );
    if ( TEMPHUM14_statusData.temp_un_over == TEMPHUM14_STATUS_ON ) {
        printf( " Under/Overrun \r\n" );
    } else {
        printf( " No Error \r\n" );
    }

    printf( "\r\n Temperature Error:" );
    if ( TEMPHUM14_statusData.temp_h_err == TEMPHUM14_STATUS_ON ) {
        printf( " Below -50 C \r\n" );
    } else if ( TEMPHUM14_statusData.temp_l_err == TEMPHUM14_STATUS_ON ) {
        printf( " Above 150 C \r\n" );
    } else {
        printf( " No Error \r\n" );
    }

    printf( "\r\n Heater Status    :" );
    if ( TEMPHUM14_statusData.heater_on == TEMPHUM14_STATUS_ON ) {
        printf( " ON \r\n" );
    } else {
        printf( " OFF \r\n" );
    }

    printf( "-----------------------------\r\n" );
}

void TEMPHUM14_getDiagnostic ( uint8_t addr, temphum14_diagn_t *diag_data )
{
    uint8_t rx_buf[ 1 ];

    APP_SENSORS_writeReadBytes( addr, TEMPHUM14_CMD_READ_DIAGNOSTIC, 1 );
    rx_buf[ 0 ] = APP_SENSORS_data.i2c.rxBuffBytes[ 0 ];
        
    diag_data->nvm_error    = ( rx_buf[ 0 ] & 0x80 ) >> 7;
    diag_data->hum_un_over  = ( rx_buf[ 0 ] & 0x40 ) >> 6;
    diag_data->hum_h_err    = ( rx_buf[ 0 ] & 0x20 ) >> 5;
    diag_data->hum_l_err    = ( rx_buf[ 0 ] & 0x10 ) >> 4;
    diag_data->temp_un_over = ( rx_buf[ 0 ] & 0x08 ) >> 3;
    diag_data->temp_h_err   = ( rx_buf[ 0 ] & 0x04 ) >> 2;
    diag_data->temp_l_err   = ( rx_buf[ 0 ] & 0x02 ) >> 1;
    diag_data->heater_on    = rx_buf[ 0 ] & 0x01;
}

uint32_t TEMPHUM14_getSerialNumber ( uint8_t addr ) {
    uint8_t rx_buf[ TEMPHUM14_SERIAL_NUMBER_BYTES ];
    uint32_t ser_numb;

    APP_SENSORS_writeReadBytes( addr, TEMPHUM14_CMD_READ_SERIAL_NUMBER, TEMPHUM14_SERIAL_NUMBER_BYTES );
    for (int index = 0; index < TEMPHUM14_SERIAL_NUMBER_BYTES; index++)
    {
        rx_buf[ index ] = APP_SENSORS_data.i2c.rxBuffBytes[ index ];
    }
    
    ser_numb = rx_buf[ 0 ];
    ser_numb <<= 8;
    ser_numb |= rx_buf[ 2 ];
    ser_numb <<= 8;
    ser_numb |= rx_buf[ 4 ];
    
    return ser_numb;
}
// ------------------------------------------------------------------------- END
