#ifndef ULTRALOWPRESS_H
#define ULTRALOWPRESS_H

#ifdef __cplusplus
extern "C"{
#endif

#include "definitions.h"

#define ULTRALOWPRESS_I2CADDR           0x6C
#define ULTRALOWPRESS_REG_CMD           0x22
#define ULTRALOWPRESS_REG_TEMP          0x2E
#define ULTRALOWPRESS_REG_PRESS         0x30
#define ULTRALOWPRESS_REG_STATUS_SYNC   0x32
#define ULTRALOWPRESS_REG_STATUS        0x36
#define ULTRALOWPRESS_REG_SERIAL_NUM_L  0x50
#define ULTRALOWPRESS_REG_SERIAL_NUM_H  0x52
#define ULTRALOWPRESS_REG_CMD_SLEEP     0x6C32
#define ULTRALOWPRESS_REG_CMD_RESET     0xB169
#define ULTRALOWPRESS_STATUS_CLEAR      0xFFFF
#define ULTRALOWPRESS_B1                397.2
#define ULTRALOWPRESS_B0                -16881
#define ULTRALOWPRESS_P_MIN             -20.0
#define ULTRALOWPRESS_P_MAX             500.0
#define ULTRALOWPRESS_OUT_MIN           -26215.0
#define ULTRALOWPRESS_OUT_MAX           26214.0
#define ULTRALOWPRESS_STATUS_TEMP_MASK  0x0010
#define ULTRALOWPRESS_STATUS_PRESS_MASK 0x0008

typedef struct {
    float temperature, pressure;
} ulp_data_struct;
    
uint32_t ULTRALOWPRESS_init();

bool ULTRALOWPRESS_isReady();

void ULTRALOWPRESS_getData(ulp_data_struct *ulp_data);

int16_t ULTRALOWPRESS_2sCompToDecimal(uint16_t twos_compliment_val);

#ifdef __cplusplus
}
#endif

#endif // ULTRALOWPRESS_H

// ------------------------------------------------------------------------ END
