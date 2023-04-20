#ifndef AIRQUALITY7_H
#define AIRQUALITY7_H

#include <math.h>
#include "definitions.h"

#define AIRQUALITY7_DEV_ADDR                             0x70
    
#define AIRQUALITY7_CMD_GET_STATUS                       0x0C
    

#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    float tvoc, co2, resistor_val;
}air7_data_struct;
    
void AIRQUALITY7_readData(air7_data_struct *air7_data);  

void airquality7_set_ppmco2(unsigned long ppmco2_value);

#ifdef __cplusplus
}
#endif
#endif 
