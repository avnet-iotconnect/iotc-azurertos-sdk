#include "temphum14.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

//This function resets the sensor and pulls down
//its serial number to prove that it is connected
uint32_t TEMPHUM14_init ( uint8_t addr )
{
    APP_SENSORS_writeByte(0x40, TEMPHUM14_CMD_RESET);
    tx_thread_sleep(20);
    for (int index = 0; index < TEMPHUM14_SERIAL_NUMBER_BYTES; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }   
    return (TEMPHUM14_getSerialNumber(0x40));
}
//This function triggers a single temperature and 
//humidity conversion with selected resolution
void TEMPHUM14_setConversion ( uint8_t addr, uint8_t hum_osr, uint8_t temp_osr )
{
    uint8_t tmp;
    
    hum_osr &= TEMPHUM14_BIT_MASK_HUM_OSR;
    temp_osr &= TEMPHUM14_BIT_MASK_TEMP_OSR;
    tmp = TEMPHUM14_CMD_CONVERSION;
    tmp |= hum_osr;
    tmp |= temp_osr;

    APP_SENSORS_writeByte(0x40, tmp);
    tx_thread_sleep(10);
}

//This function reads the temperature and humidity
//data from the sensor and uses calibration values
//to scale the data and update the data array parameter
void TEMPHUM14_getTemperatureHumidity ( uint8_t addr, float data[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    uint8_t rx_buf[6];
    uint16_t tmp;
    float temperature;
    float humidity;

    //Send command to read the temperature and relative humidity
    APP_SENSORS_writeReadBytes( addr, TEMPHUM14_CMD_READ_T_AND_RH, 6 );

    for (int index = 0; index < 6; index++)
    {
        rx_buf[ index ] = APP_SENSORS_data.i2c.rxBuffBytes[ index ];
    }
    
    //Scale the temperature data
    tmp = rx_buf[ 0 ];
    tmp <<= 8;
    tmp |= rx_buf[ 1 ];
    temperature = ( float ) tmp;
    temperature /= TEMPHUM14_CALC_16_BIT_DIVIDER;
    temperature *= TEMPHUM14_CALC_TEMP_MULTI_FACT;
    temperature -= TEMPHUM14_CALC_TEMP_SUB_FACT;
    data[0] = temperature;
    
    //Scale the humidity data
    tmp = rx_buf[ 3 ];
    tmp <<= 8;
    tmp |= rx_buf[ 4 ];
    humidity = ( float ) tmp;
    humidity /= TEMPHUM14_CALC_16_BIT_DIVIDER;
    humidity *= TEMPHUM14_CALC_HUM_MULTI_FACT;
    data[1] = humidity;
}

//This function simply retrieves the serial number of the sensor
uint32_t TEMPHUM14_getSerialNumber ( uint8_t addr )
{
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
