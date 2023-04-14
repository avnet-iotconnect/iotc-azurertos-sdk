#ifndef ALTITUDE2_H
#define ALTITUDE2_H

#include <math.h>
#include "definitions.h"

#define ALTITUDE2_DEVICE_ADDR_0                       0x76
#define ALTITUDE2_DEVICE_ADDR_1                       0x77 


#ifdef __cplusplus
extern "C"{
#endif

void ALTITUDE2_reset();

void ALTITUDE2_cal_vals(float values[]);

void ALTITUDE2_readData(float data[], float cal[]);


#ifdef __cplusplus
}
#endif
#endif  // _ALTITUDE2_H_
