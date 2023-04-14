#ifndef PHT_H
#define PHT_H

#ifdef __cplusplus
extern "C"{
#endif

#include "definitions.h"

#define PHT_RH_MEASURE_RH_HOLD                                     0xE5
#define PHT_RH_MEASURE_RH_NO_HOLD                                  0xF5
#define PHT_SENSOR_TYPE_RH                                         0x00
#define PHT_SENSOR_TYPE_PT                                         0x01
#define PHT_I2C_SLAVE_ADDR_RH                                      0x40
#define PHT_I2C_SLAVE_ADDR_P_AND_T                                 0x76


void PHT_reset();
void PHT_cal_vals(float values[]);
void PHT_readData(float data[], float cal[]);

#ifdef __cplusplus
}
#endif
#endif // PHT_H

// ------------------------------------------------------------------------ END
