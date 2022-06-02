/*
 * MaaxboardRT i2c wrapper
 */
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_lpi2c.h"

#include "TODRV_HSE_i2c_wrapper.h"
#include "TO.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define TO_I2C_MASTER_BASE (LPI2C2_BASE)
#define TO_I2C_MASTER ((LPI2C_Type *)TO_I2C_MASTER_BASE)

#define TO136ADDR 					 0x50U
#define LPI2C_BAUDRATE               400000U


/*******************************************************************************
 * Variables
 ******************************************************************************/
lpi2c_master_config_t masterConfig;
lpi2c_master_transfer_t masterXfer = {0};


TO_lib_ret_t TO_data_init(void)
{
     /*
     * masterConfig.debugEnable = false;
     * masterConfig.ignoreAck = false;
     * masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
     * masterConfig.baudRate_Hz = 100000U;
     * masterConfig.busIdleTimeout_ns = 0;
     * masterConfig.pinLowTimeout_ns = 0;
     * masterConfig.sdaGlitchFilterWidth_ns = 0;
     * masterConfig.sclGlitchFilterWidth_ns = 0;
     */
    LPI2C_MasterGetDefaultConfig(&masterConfig);
    /* Change the default baudrate configuration */
    masterConfig.baudRate_Hz = LPI2C_BAUDRATE;

    LPI2C_MasterInit(TO_I2C_MASTER, &masterConfig, CLOCK_GetFreq(kCLOCK_OscRc48MDiv2));
    return TO_OK;
}


TO_lib_ret_t TO_data_fini(void)
{
    LPI2C_MasterDeinit(TO_I2C_MASTER);
    return TO_OK;
}

TO_lib_ret_t TO_data_read(void *data, unsigned int length)
{
    masterXfer.slaveAddress   = TO136ADDR;
    masterXfer.direction      = kLPI2C_Read;
    masterXfer.data           = data;
    masterXfer.dataSize       = length;

    status_t result = LPI2C_MasterTransferBlocking(TO_I2C_MASTER, &masterXfer);
	if (result == kStatus_Success) {
		return TO_OK;
	}

	return TO_DEVICE_READ_ERROR;
}

TO_lib_ret_t TO_data_write(const void *data, unsigned int length)
{
    masterXfer.slaveAddress   = TO136ADDR;
    masterXfer.direction      = kLPI2C_Write;
    masterXfer.data           = (uint8_t*)data;
    masterXfer.dataSize       = length;
//    masterXfer.flags          = kLPI2C_TransferDefaultFlag;
//    masterXfer.subaddress     = (uint32_t)deviceAddress;
//    masterXfer.subaddressSize = 1;

    status_t result = LPI2C_MasterTransferBlocking(TO_I2C_MASTER, &masterXfer);
	if (result == kStatus_Success) {
		return TO_OK;
	}

	return TO_DEVICE_WRITE_ERROR;
}
