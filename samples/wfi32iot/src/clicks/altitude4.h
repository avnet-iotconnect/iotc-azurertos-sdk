#ifndef ALTITUDE4_H
#define ALTITUDE4_H

#include <math.h>
#include "definitions.h"


#define ALTITUDE4_OK                                  0x00
#define ALTITUDE4_INIT_ERROR                          0xFF


#define ALTITUDE4_SLAVE_ADDRESS                       0x27
#define ALTITUDE4_SENS_DATA                           0xAC
   

#ifdef __cplusplus
extern "C"{
#endif


void ALTITUDE4_readData(float data[]);


#ifdef __cplusplus
}
#endif
#endif  // _ALTITUDE4_H_