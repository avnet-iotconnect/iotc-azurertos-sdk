#ifndef T9602_H
#define T9602_H

#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include "definitions.h"


#ifdef __cplusplus
extern "C"{
#endif

typedef struct {
    float humidity, temperature;
} t9602_data_struct;
    
void T9602_readData(t9602_data_struct *t9602_data);    

#ifdef __cplusplus
}
#endif
#endif 
