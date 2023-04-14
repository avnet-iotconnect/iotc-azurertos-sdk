#include "vavpress.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

//Registers for VAV press have signed values, so conversion function is necessary
int16_t VAVPRESS_2sCompToDecimal(uint16_t twos_compliment_val)
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

//Initializes the sensor and returns its calibration ID to prove it is connected
uint16_t VAVPRESS_init()
{
    //Clear the i2c buffer
    for (int index = 0; index < 54; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Pull the calibration ID from the sensor
    uint16_t calID = 0;
    uint8_t byte_holder = 0;
    APP_SENSORS_writeReadBytes(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_RETRIEVE_ELECTRONIC_SIGNATURE, 54);
    tx_thread_sleep(10);
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[25];
    calID = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[26];
    calID = calID << 8;
    calID = calID | byte_holder;
    
    //Reset the sensor
    APP_SENSORS_writeByte(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_RESET_FIRMWARE);
    
    tx_thread_sleep(10);
    
    //Begin the pressure conversion process (only needs to be done this once)
    APP_SENSORS_writeByte(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_START_PRESSURE_CONVERSION);
    
    return calID;
}

//This function reads the temperature and pressure
//data from the sensor and uses calibration values
//to scale the data and update the data array parameter
void VAVPRESS_getSensorReadings(float data[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    int16_t press_data;
    int16_t temp_data;
    float tmp;
    uint8_t byte_holder = 0;

    //Perform a read to get the pressure and temperature data
    APP_SENSORS_justRead(VAVPRESS_I2CADDR_0, EXTENDED_READOUT_NUMBYTES);

    //Extract and scale the pressure value
    uint16_t raw_press = 0;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    raw_press = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[0];
    raw_press = raw_press << 8;
    raw_press = raw_press | byte_holder;
    press_data = VAVPRESS_2sCompToDecimal(raw_press);
    tmp = ( float ) press_data;
    tmp /= 1200.0;
    data[0] = tmp;
  
    //Extract and scale the temperature value
    uint16_t raw_temp = 0;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[3];
    raw_temp = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[2];
    raw_temp = raw_temp << 8;
    raw_temp = raw_temp | byte_holder;
    temp_data = VAVPRESS_2sCompToDecimal(raw_temp);
    tmp = ( float ) temp_data;
    tmp -= 105.0;
    tmp /= 72.0;
    tmp += 23.1; 
    data[1] = tmp;

}

// ------------------------------------------------------------------------- END
