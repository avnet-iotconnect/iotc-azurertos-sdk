#ifndef ALTITUDE2_H
#define ALTITUDE2_H

#include <math.h>
#include "definitions.h"

#define ALTITUDE2_DEVICE_ADDR_0                       0x76
#define ALTITUDE2_DEVICE_ADDR_1                       0x77 


#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    float pressure, temperature, cal_1, cal_2, cal_3, cal_4, cal_5, cal_6;
} alt2_data_struct;
    
void ALTITUDE2_reset();

void ALTITUDE2_cal_vals(alt2_data_struct *alt2_data);

void ALTITUDE2_readData(alt2_data_struct *alt2_data);


#ifdef __cplusplus
}
#endif
#endif  // _ALTITUDE2_H_
