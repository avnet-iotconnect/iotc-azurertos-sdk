#include "altitude4.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

// ------------------------------------------------ PUBLIC FUNCTION DEFINITIONS

void ALTITUDE4_readData(float data[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Create array to hold pressure and temperature values extracted from I2C
    uint16_t response[] = {0, 0};
    
    //Send measure command
    APP_SENSORS_writeByte(ALTITUDE4_SLAVE_ADDRESS, 0xAC);
    
    //delay thread for 20ms so sensor can update data
    tx_thread_sleep(20);
    
    //Holds byte currently being read
    uint8_t byte_holder = 0;
    
    //Hold values for raw pressure and temperature readings
    uint16_t press = 0;
    uint16_t temp = 0;

    //Read the measurement registers and puts the data into rx.BuffBytes
    APP_SENSORS_writeReadBytes(ALTITUDE4_SLAVE_ADDRESS, ALTITUDE4_SLAVE_ADDRESS, 5);
    
    //Extract bytes and bit-shift them to create 2-byte pressure reading
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    press = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[2];
    press = press << 8;
    press = press | byte_holder;
    response[0] = press;

    //Extract bytes and bit-shift them to create 2-byte temperature reading
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[3];
    temp = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[4];
    temp = temp << 8;
    temp = temp | byte_holder;
    response[1] = temp;
    
    //Convert I2C pressure reading to a float and apply multipliers
    float pressure_flt = (float)response[0]; 
    float corrected_pressure = (pressure_flt / 65.535) + 260;
    
    //Calculate altitude from pressure measurement
    float corrected_altitude = 44330*(1-pow(corrected_pressure/1013.25, 0.19029495718363463368220742150333));
    
    //Convert I2C temperature reading to a float and apply multipliers
    float temperature_flt = (float)response[1];
    float corrected_temperature = (temperature_flt / 524.28) - 40;
   
    //Update the values of the passed-in array
    data[0] = corrected_pressure;
    data[1] = corrected_altitude;
    data[2] = corrected_temperature;
    
}