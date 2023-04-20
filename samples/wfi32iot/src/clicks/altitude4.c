#include "altitude4.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

// ------------------------------------------------ PUBLIC FUNCTION DEFINITIONS

void ALTITUDE4_readData(alt4_data_struct *alt4_data)
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++) {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Send measure command
    APP_SENSORS_writeByte(ALTITUDE4_SLAVE_ADDRESS, 0xAC);
    
    //delay thread for 20ms so sensor can update data
    tx_thread_sleep(20);

    //Read the measurement registers and puts the data into rx.BuffBytes
    APP_SENSORS_writeReadBytes(ALTITUDE4_SLAVE_ADDRESS, ALTITUDE4_SLAVE_ADDRESS, 5);
    
    uint16_t raw_pressure = APP_SENSORS_data.i2c.rxBuffBytes[1];
    raw_pressure = raw_pressure << 8;
    raw_pressure = raw_pressure | APP_SENSORS_data.i2c.rxBuffBytes[2];
    
    uint16_t raw_temperature = APP_SENSORS_data.i2c.rxBuffBytes[3];
    raw_temperature = raw_temperature << 8;
    raw_temperature = raw_temperature | APP_SENSORS_data.i2c.rxBuffBytes[4];
    
    //Convert I2C pressure reading to a float and apply multipliers
    float corrected_pressure = ((float)raw_pressure / 65.535) + 260;
    
    //Calculate altitude from pressure measurement
    float corrected_altitude = 44330*(1-pow(corrected_pressure/1013.25, 0.19029495718363463368220742150333));
    
    //Convert I2C temperature reading to a float and apply multipliers
    float corrected_temperature = ((float)raw_temperature / 524.28) - 40;
   
    //Update the values of the passed-in struct
    alt4_data->pressure = corrected_pressure;
    alt4_data->altitude = corrected_altitude;
    alt4_data->temperature = corrected_temperature;
    
}