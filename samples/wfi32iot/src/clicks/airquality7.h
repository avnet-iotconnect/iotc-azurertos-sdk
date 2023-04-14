#ifndef AIRQUALITY7_H
#define AIRQUALITY7_H

#include <math.h>
#include "definitions.h"

#define AIRQUALITY7_DEV_ADDR                             0x70
    
#define AIRQUALITY7_CMD_GET_STATUS                       0x0C
    

#ifdef __cplusplus
extern "C"{
#endif


void AIRQUALITY7_readData(float data[]);  

void airquality7_set_ppmco2();

#ifdef __cplusplus
}
#endif
#endif 
