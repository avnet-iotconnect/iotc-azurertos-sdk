#include "T9602.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

void T9602_readData(float data[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    uint8_t rec_data[4] = {0, 0, 0, 0};
    
    //Send command to read the sensor values
    APP_SENSORS_justRead(0x28, 4);
    
    //delay thread for 100ms so sensor can update data
    tx_thread_sleep(100);
    
    //Read the i2c buffer
    rec_data[0] = APP_SENSORS_data.i2c.rxBuffBytes[0];
    rec_data[1] = APP_SENSORS_data.i2c.rxBuffBytes[1];
    rec_data[2] = APP_SENSORS_data.i2c.rxBuffBytes[2];
    rec_data[3] = APP_SENSORS_data.i2c.rxBuffBytes[3];
    
    //Extracting Relative Humidity Data
    uint16_t hum = 0x0000;
    hum += rec_data[0];
    hum &= 63;
    hum *= 256;
    hum += rec_data[1];
    float hum_float = (float)hum;
    hum_float /= 16384;
    hum_float *= 100;
    data[0] = hum_float;
    
    //Extracting Temperature Data
    uint16_t temp = 0x0000;
    temp += rec_data[2];
    temp *= 64;
    temp += (rec_data[3] >> 2);
    float temp_float = (float)temp;
    temp_float /= 16384;
    temp_float *= 165;
    temp_float -= 40;
    data[1] = temp_float;
}
