#include "airquality7.h"

extern APP_SENSORS_DATA APP_SENSORS_data;

static uint8_t AIRQUALITY7_getCRC(uint8_t *data_in, uint8_t data_size);

//This is the function to read the air quality data from the AQ7 click sensor
void AIRQUALITY7_readData(air7_data_struct *air7_data)
{
    //Clear the i2c buffer
    for (int index = 0; index < 10; index++) {
        APP_SENSORS_data.i2c.rxBuffBytes[index] = 0;
    }
    
    //Preparing the GET STATUS command message
    uint8_t send_data[6] = {0, 0, 0, 0, 0, 0};
    uint8_t rec_data[7] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t crc_calc = 0;
    send_data[ 0 ] = AIRQUALITY7_CMD_GET_STATUS;
    send_data[ 5 ] = AIRQUALITY7_getCRC(send_data, 5);
    
    //Send the GET STATUS command
    APP_SENSORS_write(AIRQUALITY7_DEV_ADDR, send_data, 6);
    
    //Wait 100ms for sensor to gather data
    tx_thread_sleep(100);
    
    //Populate the i2c buffer with the data from the sensor
    APP_SENSORS_justRead(AIRQUALITY7_DEV_ADDR, 7);
    
    //Read the i2c buffer
    rec_data[0] = APP_SENSORS_data.i2c.rxBuffBytes[0];
    rec_data[1] = APP_SENSORS_data.i2c.rxBuffBytes[1];
    rec_data[2] = APP_SENSORS_data.i2c.rxBuffBytes[2];
    rec_data[3] = APP_SENSORS_data.i2c.rxBuffBytes[3];
    rec_data[4] = APP_SENSORS_data.i2c.rxBuffBytes[4];
    rec_data[5] = APP_SENSORS_data.i2c.rxBuffBytes[5];
    rec_data[6] = APP_SENSORS_data.i2c.rxBuffBytes[6];
    
    crc_calc = AIRQUALITY7_getCRC(rec_data, 6);
    
    //If CRC does not match
    if ( crc_calc != rec_data[6] ) {
        //Report all zeros
        air7_data->tvoc = 0;
        air7_data->co2 = 0;
    }
    
    //Extracting tVOC data
    float calculated_tvoc = 0;
    calculated_tvoc = rec_data[0] - 13;
    calculated_tvoc *= 1000;
    calculated_tvoc /= 229;
    air7_data->tvoc = calculated_tvoc;
    
    //Extracting CO2 data
    float calculated_co2 = 0;
    calculated_co2 = rec_data[1] - 13;
    calculated_co2 *= 1600;
    calculated_co2 /= 229;
    calculated_co2 += 400;
    air7_data->co2 = calculated_co2;
    
    //Extracting resistor value data
    uint32_t calculated_resistor_val = 0;
    calculated_resistor_val = (uint32_t)( rec_data[2] * 65536 );
    calculated_resistor_val += (uint16_t)( rec_data[3] * 256 );
    calculated_resistor_val += rec_data[4];
    calculated_resistor_val *= 10;
    air7_data->resistor_val = (float)calculated_resistor_val;
}

//This function is used to calibrate the sensor if its readings are off.
void airquality7_set_ppmco2(unsigned long ppmco2_value)
{
    //6-byte structure, first byte is the command value (0x08)
    uint8_t tmp_data[ 6 ] = {0x08, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    //Convert input value to bytes
    tmp_data[1] = (ppmco2_value >> 24) & 0xFF;
    tmp_data[2] = (ppmco2_value >> 16) & 0xFF;
    tmp_data[3] = (ppmco2_value >> 8) & 0xFF;
    tmp_data[4] = (ppmco2_value >> 0) & 0xFF;
    
    //6th byte is CRC
    tmp_data[ 5 ] = AIRQUALITY7_getCRC(tmp_data, 5);
    
    APP_SENSORS_write(AIRQUALITY7_DEV_ADDR, tmp_data, 6);
}

//This function retrieves the CRC value from incoming data.
static uint8_t AIRQUALITY7_getCRC(uint8_t *data_in, uint8_t data_size)
{
    uint8_t crc = 0x00;
    uint8_t cnt = 0x00;
    uint16_t sum = 0x0000;
 
    for (cnt = 0; cnt < data_size; cnt++) {
        sum += *data_in;
        data_in++;
    }
    
    crc = sum;
    crc += (sum / 0x0100);
    crc = 0xFF - crc;

    return crc;
}

