#include "ultralowpress.h"
    
extern APP_SENSORS_DATA APP_SENSORS_data;

//Registers for VAV press have signed values, so conversion function is necessary
int16_t ULTRALOWPRESS_2sCompToDecimal(uint16_t twos_compliment_val)
{
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

//Returns the sensor's serial number to prove it is connected
uint32_t ULTRALOWPRESS_init()
{
    uint32_t serial_number = 0;

    APP_SENSORS_data.i2c.rxBuffWords[0] = 0;
    APP_SENSORS_data.i2c.rxBuffWords[1] = 0;
    
    // Read the 32-bit serial number from the SM8436
    APP_SENSORS_writeReadWords(ULTRALOWPRESS_I2CADDR, ULTRALOWPRESS_REG_SERIAL_NUM_L, 4);

    serial_number = APP_SENSORS_data.i2c.rxBuffWords[1] << 16;
    serial_number |= APP_SENSORS_data.i2c.rxBuffWords[0];
    
    return (serial_number);
}

//Returns boolean representing if the sensor is ready to make a measurement or not
bool ULTRALOWPRESS_isReady()
{   
    uint16_t status_reg;
    
    APP_SENSORS_writeReadWords(ULTRALOWPRESS_I2CADDR, ULTRALOWPRESS_REG_STATUS, 2);
    status_reg = APP_SENSORS_data.i2c.rxBuffWords[0];
    
    return ( status_reg & ULTRALOWPRESS_STATUS_TEMP_MASK ) && 
            ( status_reg & ULTRALOWPRESS_STATUS_PRESS_MASK );
}

//This function reads the temperature and pressure
//data from the sensor and uses calibration values
//to scale the data and update the data array parameter
void ULTRALOWPRESS_getData(float data[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffWords[index] = 0;
    }
    
    APP_SENSORS_writeWord_LSB_b4_MSB(ULTRALOWPRESS_I2CADDR, ULTRALOWPRESS_REG_STATUS, ULTRALOWPRESS_STATUS_CLEAR);
    tx_thread_sleep(10);
    
    int16_t decimal;
    
    //Send command to read temperature
    APP_SENSORS_writeReadWords(ULTRALOWPRESS_I2CADDR, ULTRALOWPRESS_REG_TEMP, 2);
    //Convert to decimal value
    decimal = ULTRALOWPRESS_2sCompToDecimal(APP_SENSORS_data.i2c.rxBuffWords[0]);
    //Scale the value to actual
    data[0] = ( (decimal - ULTRALOWPRESS_B0) / ULTRALOWPRESS_B1 );

    //Send command to read pressure
    APP_SENSORS_writeReadWords(ULTRALOWPRESS_I2CADDR, ULTRALOWPRESS_REG_PRESS, 2);
    //Convert to decimal value
    decimal = ULTRALOWPRESS_2sCompToDecimal(APP_SENSORS_data.i2c.rxBuffWords[0]);
    //Scale the value to actual
    data[1] = -20.0 + ( ( decimal + 26215.0 ) / ( 26214.0 + 26215.0 ) ) * (520);
}

// ------------------------------------------------------------------------- END
