#ifndef VAVPRESS_H
#define VAVPRESS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "definitions.h"

#define EXTENDED_READOUT_NUMBYTES 4
#define VAVPRESS_SET_CMD_RESET_FIRMWARE                 0x11
#define VAVPRESS_SET_CMD_START_PRESSURE_CONVERSION      0x20
#define VAVPRESS_SET_CMD_RETRIEVE_ELECTRONIC_SIGNATURE  0x23
#define VAVPRESS_I2CADDR_0  0x5C // Default I2C address setting for VAV Click
#define VAVPRESS_I2CADDR_1  0x5D
#define VAVPRESS_I2CADDR_2  0x5E
#define VAVPRESS_I2CADDR_3  0x5F
#define EL_SIGNATURE_NUMBYTES 54

typedef struct {
    float temperature, pressure;
} vav_data_struct;
    
uint16_t VAVPRESS_init();

void VAVPRESS_getSensorReadings(vav_data_struct *vav_data);

#ifdef __cplusplus
}
#endif

#endif // VAVPRESS_H

// ------------------------------------------------------------------------ END
