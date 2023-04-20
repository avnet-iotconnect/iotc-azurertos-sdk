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
void ALTITUDE2_cal_vals(alt2_data_struct *alt2_data)
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++) {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Read the pressure sensitivity prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA2, 2);
    tx_thread_sleep(10);
    uint16_t c1 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    c1 = c1 << 8;
    c1 = c1 | APP_SENSORS_data.i2c.rxBuffBytes[1];
    alt2_data->cal_1 = c1;
    
    //Read the pressure offset prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA4, 2);
    tx_thread_sleep(10);
    uint16_t c2 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    c2 = c2 << 8;
    c2 = c2 | APP_SENSORS_data.i2c.rxBuffBytes[1];
    alt2_data->cal_2 = c2;
    
    //Read the temperature coefficient of pressure sensitivity prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA6, 2);
    tx_thread_sleep(10);
    uint16_t c3 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    c3 = c3 << 8;
    c3 = c3 | APP_SENSORS_data.i2c.rxBuffBytes[1];
    alt2_data->cal_3 = c3;
    
    //Read the temperature coefficient of pressure offset prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xA8, 2);
    tx_thread_sleep(10);
    uint16_t c4 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    c4 = c4 << 8;
    c4 = c4 | APP_SENSORS_data.i2c.rxBuffBytes[1];
    alt2_data->cal_4 = c4;
    
    //Read the reference temperature prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xAA, 2);
    tx_thread_sleep(10);
    uint16_t c5 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    c5 = c5 << 8;
    c5 = c5 | APP_SENSORS_data.i2c.rxBuffBytes[1];
    alt2_data->cal_5 = c5;
    
    //Read the temperature coefficient prom value
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0xAC, 2);
    tx_thread_sleep(10);
    uint16_t c6 = APP_SENSORS_data.i2c.rxBuffBytes[0];
    c6 = c6 << 8;
    c6 = c6 | APP_SENSORS_data.i2c.rxBuffBytes[1];
    alt2_data->cal_6 = c6;
}

//This function reads the temperature and pressure
//data from the sensor and uses the calibration values
//to scale the data and update the data array parameter
void ALTITUDE2_readData(alt2_data_struct *alt2_data)
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++) {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Send measure pressure command
    APP_SENSORS_writeByte(ALTITUDE2_DEVICE_ADDR_1, 0x46);
    
    //delay thread for 10ms so registers can update
    tx_thread_sleep(10);
    
    //Read the measurement registers and puts the data into rx.BuffBytes
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0x00, 3);
    
    //delay thread for 10ms so registers can update
    tx_thread_sleep(10);
    
    //Extract bytes and bit-shift them to create 2-byte pressure reading
    uint32_t raw_pressure = APP_SENSORS_data.i2c.rxBuffBytes[0];
    raw_pressure = raw_pressure << 8;
    raw_pressure = raw_pressure | APP_SENSORS_data.i2c.rxBuffBytes[1];
    raw_pressure = raw_pressure << 8;
    raw_pressure = raw_pressure | APP_SENSORS_data.i2c.rxBuffBytes[2];
    
    //Send measure pressure command
    APP_SENSORS_writeByte(ALTITUDE2_DEVICE_ADDR_1, 0x56);
    
    //delay thread for 10ms so registers can update
    tx_thread_sleep(10);
    
    //Read the measurement registers and puts the data into rx.BuffBytes
    APP_SENSORS_writeReadBytes(ALTITUDE2_DEVICE_ADDR_1, 0x00, 3);
    
    tx_thread_sleep(10);
    
    //Extract bytes and bit-shift them to create 2-byte pressure reading
    uint32_t raw_temperature = APP_SENSORS_data.i2c.rxBuffBytes[0];
    raw_temperature = raw_temperature << 8;
    raw_temperature = raw_temperature | APP_SENSORS_data.i2c.rxBuffBytes[1];
    raw_temperature = raw_temperature << 8;
    raw_temperature = raw_temperature | APP_SENSORS_data.i2c.rxBuffBytes[2];
    
    //Calculate relative values from readings and prom cal values
    float dT = raw_temperature - (alt2_data->cal_5 * 256.0);
    float corrected_temperature = 2000.0 + (dT * alt2_data->cal_6 / 8388608.0);
    float offset = (alt2_data->cal_2 * 131072.0) + ((alt2_data->cal_4 * dT) / 64.0);
    float sensitivity = (alt2_data->cal_1 * 65536.0) + ((alt2_data->cal_3 * dT) / 128.0);
    float corrected_pressure = (((raw_pressure * sensitivity) / 2097152.0) - offset) / 32768.0;
    
    //Update the passed-in struct with temperature and pressure values
    alt2_data->pressure = corrected_pressure / 100.0;
    alt2_data->temperature = corrected_temperature / 100.0;
}
