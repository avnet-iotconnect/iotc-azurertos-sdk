#include "altitude2.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

// ------------------------------------------------ PUBLIC FUNCTION DEFINITIONS
//This function simply resets the sensor
void ALTITUDE2_reset()
{
    APP_SENSORS_writeByte(ALTITUDE2_DEVICE_ADDR_1, 0x1E);
}

//This function reads the calibration values from the PROM
//and updates the array parameter with those values
void ALTITUDE2_cal_vals(float values[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Holds byte currently being read
    uint8_t byte_holder = 0;
    
    //Read the pressure sensitivity prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA2, 2);
    tx_thread_sleep(10);
    uint16_t c1 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    c1 = c1 << 8;
    c1 = c1 | byte_holder;
    values[0] = c1;
    
    //Read the pressure offset prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA4, 2);
    tx_thread_sleep(10);
    uint16_t c2 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    c2 = c2 << 8;
    c2 = c2 | byte_holder;
    values[1] = c2;
    
    //Read the temperature coefficient of pressure sensitivity prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA6, 2);
    tx_thread_sleep(10);
    uint16_t c3 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    c3 = c3 << 8;
    c3 = c3 | byte_holder;
    values[2] = c3;
    
    //Read the temperature coefficient of pressure offset prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA8, 2);
    tx_thread_sleep(10);
    uint16_t c4 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    c4 = c4 << 8;
    c4 = c4 | byte_holder;
    values[3] = c4;
    
    //Read the reference temperature prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xAA, 2);
    tx_thread_sleep(10);
    uint16_t c5 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    c5 = c5 << 8;
    c5 = c5 | byte_holder;
    values[4] = c5;
    
    //Read the temperature coefficient prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xAC, 2);
    tx_thread_sleep(10);
    uint16_t c6 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    c6 = c6 << 8;
    c6 = c6 | byte_holder;
    values[5] = c6;
}

//This function reads the temperature and pressure
//data from the sensor and uses the calibration values
//to scale the data and update the data array parameter
void ALTITUDE2_readData(float data[], float cal[])
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++)
    {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Holds byte currently being read
    uint8_t byte_holder = 0;
    
    float c1 = cal[0];
    float c2 = cal[1];
    float c3 = cal[2];
    float c4 = cal[3];
    float c5 = cal[4];
    float c6 = cal[5];
    
    //Hold values for raw pressure and temperature readings
    uint32_t press = 0;
    uint32_t temp = 0;
    
    //Send measure pressure command
    APP_SENSORS_writeByte(ALTITUDE2_DEVICE_ADDR_1, 0x46);
    
    //delay thread for 10ms so registers can update
    tx_thread_sleep(10);
    
    //Read the measurement registers and puts the data into rx.BuffBytes
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0x00, 3);
    
    //delay thread for 10ms so registers can update
    tx_thread_sleep(10);
    
    //Extract bytes and bit-shift them to create 2-byte pressure reading
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[0];
    press = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    press = press << 8;
    press = press | byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[2];
    press = press << 8;
    press = press | byte_holder;
    
    
    //Send measure pressure command
    APP_SENSORS_writeByte(ALTITUDE2_DEVICE_ADDR_1, 0x56);
    
    //delay thread for 10ms so registers can update
    tx_thread_sleep(10);
    
    //Read the measurement registers and puts the data into rx.BuffBytes
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0x00, 3);
    
    tx_thread_sleep(10);
    
    //Extract bytes and bit-shift them to create 2-byte pressure reading
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[0];
    temp = byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[1];
    temp = temp << 8;
    temp = temp | byte_holder;
    byte_holder = APP_SENSORS_data.i2c.rxBuffBytes[2];
    temp = temp << 8;
    temp = temp | byte_holder;
    
    //Calculate relative values from readings and prom cal values
    float dT = temp - (c5 * 256.0);
    float actual_temp = 2000.0 + (dT * c6 / 8388608.0);
    float offset = (c2 * 131072.0 ) + ((c4 * dT) / 64.0);
    float sensitivity = (c1 * 65536.0) + ((c3 * dT) / 128.0);
    float actual_press = (((press * sensitivity) / 2097152.0) - offset) / 32768.0;
    
    //Update the passed-in array with temperature and pressure values
    data[0] = actual_press / 100.0;
    data[1] = actual_temp / 100.0;
}
