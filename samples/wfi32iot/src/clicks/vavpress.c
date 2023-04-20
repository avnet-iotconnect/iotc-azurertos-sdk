#include "vavpress.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

//Registers for VAV press have signed values, so conversion function is necessary
int16_t VAVPRESS_2sCompToDecimal(uint16_t twos_compliment_val)
{
    uint16_t sign_mask = 0x8000;

    // if positive
    if ( (twos_compliment_val & sign_mask) == 0 ) {
        return twos_compliment_val;
    } 
    
    //  if negative
    else {
        // invert all bits, add one, and make negative
        return -(~twos_compliment_val + 1);
    }
}

//Initializes the sensor and returns its calibration ID to prove it is connected
uint16_t VAVPRESS_init()
{
    //Clear the i2c buffer
    for (int index = 0; index < 54; index++) {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Pull the calibration ID from the sensor
    uint16_t calID = 0;
    APP_SENSORS_writeReadBytes(VAVPRESS_I2CADDR_0, VAVPRESS_SET_CMD_RETRIEVE_ELECTRONIC_SIGNATURE, 54);
    tx_thread_sleep(10);
    calID = APP_SENSORS_data.i2c.rxBuffBytes[25];
    calID = calID << 8;
    calID = calID | APP_SENSORS_data.i2c.rxBuffBytes[26];
    
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
void VAVPRESS_getSensorReadings(vav_data_struct *vav_data)
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++) {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }

    //Perform a read to get the pressure and temperature data
    APP_SENSORS_justRead(VAVPRESS_I2CADDR_0, EXTENDED_READOUT_NUMBYTES);

    //Extract and scale the pressure value
    uint16_t raw_pressure = 0;
    raw_pressure = APP_SENSORS_data.i2c.rxBuffBytes[1];
    raw_pressure = raw_pressure << 8;
    raw_pressure = raw_pressure | APP_SENSORS_data.i2c.rxBuffBytes[0];
    int16_t converted_pressure = VAVPRESS_2sCompToDecimal(raw_pressure);
    float corrected_pressure = ( float ) converted_pressure;
    corrected_pressure /= 1200.0;
    vav_data->pressure = corrected_pressure;
  
    //Extract and scale the temperature value
    uint16_t raw_temperature = 0;
    raw_temperature = APP_SENSORS_data.i2c.rxBuffBytes[3];
    raw_temperature = raw_temperature << 8;
    raw_temperature = raw_temperature | APP_SENSORS_data.i2c.rxBuffBytes[2];
    int16_t converted_temperature = VAVPRESS_2sCompToDecimal(raw_temperature);
    float corrected_temperature = ( float ) converted_temperature;
    corrected_temperature -= 105.0;
    corrected_temperature /= 72.0;
    corrected_temperature += 23.1; 
    vav_data->temperature = corrected_temperature;

}
// ------------------------------------------------------------------------- END
