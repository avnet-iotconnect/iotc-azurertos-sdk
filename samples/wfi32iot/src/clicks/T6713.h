#ifndef T6713_H
#define T6713_H

#include <math.h>
#include "definitions.h"

#define T6713_DEV_ADDR                             0x15
    
#define T6713_CMD_GET_PPM                          0x04
    

#ifdef __cplusplus
extern "C"{
#endif


void T6713_readData(int data[]);  
void T6713_calibrate();

#ifdef __cplusplus
}
#endif
#endif 