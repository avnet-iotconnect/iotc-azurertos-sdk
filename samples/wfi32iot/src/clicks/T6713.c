#include "T6713.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

//This function reads the CO2 data from
//the sensor and updates the data array parameter
void T6713_readData(int data[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Send command to initiate measurement
    uint8_t send_data[5] = {T6713_CMD_GET_PPM, 0x13, 0x8B, 0x00, 0x01};
    APP_SENSORS_write(T6713_DEV_ADDR , send_data, 5);
    
    //Prepare container for received data
    uint8_t rec_data[4] = {0, 0, 0, 0};
    
    //delay thread for 100ms so sensor can update data
    tx_thread_sleep(100);
    
    //Send command to read the sensor values
    APP_SENSORS_justRead(T6713_DEV_ADDR, 4);
    
    //Read the i2C buffer into the receive data container
    rec_data[0] = APP_SENSORS_data.i2c.rxBuffBytes[0];
    rec_data[1] = APP_SENSORS_data.i2c.rxBuffBytes[1];
    rec_data[2] = APP_SENSORS_data.i2c.rxBuffBytes[2];
    rec_data[3] = APP_SENSORS_data.i2c.rxBuffBytes[3];
    
    
    //Extracting CO2 data
    uint16_t CO2 = 0;
    CO2 = (rec_data[ 2 ] * 256) + rec_data[ 3 ];
    data[0] = (int)CO2;
    
}

//This function is used to calibrate the sensor if the readings are wrong.
//YOU DO NOT NEED TO CHANGE THE SENT VALUE, it automatically knows what to do.
void T6713_calibrate()
{
    APP_SENSORS_data.i2c.rxBuffBytes[0] = 0;
    
    uint8_t send_data[5] = {0x05, 0x03, 0xEC, 0xFF, 0x00};
    
    APP_SENSORS_write(T6713_DEV_ADDR , send_data, 5);
    
    tx_thread_sleep(420000);
    
}

