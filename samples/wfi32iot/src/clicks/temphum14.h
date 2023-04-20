#ifndef TEMPHUM14_H
#define TEMPHUM14_H

#include "definitions.h"

#ifdef __cplusplus
extern "C"{
#endif

#define TEMPHUM14_I2C_SLAVE_ADDR_GND                     0x40
#define TEMPHUM14_I2C_SLAVE_ADDR_VCC                     0x41
#define TEMPHUM14_CMD_RESET                              0x1E
#define TEMPHUM14_CMD_HEATER_ON                          0x04
#define TEMPHUM14_CMD_HEATER_OFF                         0x02
#define TEMPHUM14_CMD_READ_T_AND_RH                      0x00
#define TEMPHUM14_CMD_READ_RH                            0x10
#define TEMPHUM14_CMD_CONVERSION                         0x40
#define TEMPHUM14_CMD_READ_DIAGNOSTIC                    0x08
#define TEMPHUM14_CMD_READ_SERIAL_NUMBER                 0x0A
#define TEMPHUM14_TEMPERATURE_NUMBER_BYTES               3
#define TEMPHUM14_HUMIDITY_NUMBER_BYTES                  3    
#define TEMPHUM14_SERIAL_NUMBER_BYTES                    6
#define TEMPHUM14_CONVERSION_HUM_OSR_0_007               0x18
#define TEMPHUM14_CONVERSION_HUM_OSR_0_010               0x10
#define TEMPHUM14_CONVERSION_HUM_OSR_0_014               0x08
#define TEMPHUM14_CONVERSION_HUM_OSR_0_020               0x00
#define TEMPHUM14_CONVERSION_TEMP_0_012                  0x06
#define TEMPHUM14_CONVERSION_TEMP_0_016                  0x04
#define TEMPHUM14_CONVERSION_TEMP_0_025                  0x02
#define TEMPHUM14_CONVERSION_TEMP_0_040                  0x00
#define TEMPHUM14_BIT_MASK_HUM_OSR                       0x18
#define TEMPHUM14_BIT_MASK_TEMP_OSR                      0x06
#define TEMPHUM14_HEATER_DISABLE                         0x00
#define TEMPHUM14_HEATER_ENABLE                          0x01
#define TEMPHUM14_STATUS_OFF                             0x00
#define TEMPHUM14_STATUS_ON                              0x01
#define TEMPHUM14_CALC_16_BIT_DIVIDER                    65535.0
#define TEMPHUM14_CALC_TEMP_MULTI_FACT                   165.000
#define TEMPHUM14_CALC_TEMP_SUB_FACT                     40.0000
#define TEMPHUM14_CALC_HUM_MULTI_FACT                    100.000

typedef struct {
    float humidity, temperature;
}temphum14_data_struct;
    
uint32_t TEMPHUM14_init ( uint8_t addr );

void TEMPHUM14_setConversion ( uint8_t addr, uint8_t hum_osr, uint8_t temp_osr );

void TEMPHUM14_getTemperatureHumidity ( uint8_t addr, temphum14_data_struct *temphum14_data);

uint32_t TEMPHUM14_getSerialNumber ( uint8_t addr );

#ifdef __cplusplus
}
#endif
#endif // TEMPHUM14_H

// ------------------------------------------------------------------------ END
