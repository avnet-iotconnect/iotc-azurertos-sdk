/*
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * ----------------------------------------------------------------------------
 * --  _____       ______  _____                                              -
 * -- |_   _|     |  ____|/ ____|                                             -
 * --   | |  _ __ | |__  | (___    Institute of Embedded Systems              -
 * --   | | | '_ \|  __|  \___ \   Zurich University of                       -
 * --  _| |_| | | | |____ ____) |  Applied Sciences                           -
 * -- |_____|_| |_|______|_____/   8401 Winterthur, Switzerland               -
 * ----------------------------------------------------------------------------
 */

/**
 * @file stm32_hal.c
 * @brief Implementation of STM32 wrapper for Trusted Objects libTO, using
 * only the ST HAL drivers.
 * @modified by Trusted Objects
 */
#include "TODRV_HSE_i2c_wrapper.h"

#if (defined(STM32L072xx) && !defined(STM32L0))
#define STM32L0
#endif

#if (defined (STM32L0) || defined(USE_STM32L0XX_NUCLEO))
#include "stm32l0xx_hal.h"
#elif (defined (STM32L1) || defined(USE_STM32L1XX_NUCLEO))
#include "stm32l1xx_hal.h"
#elif (defined (STM32L4) || defined(USE_STM32L4XX_NUCLEO))
#include "stm32l4xx_hal.h"
#elif (defined (STM32F4) || defined(USE_STM32F4XX_NUCLEO))
#include "stm32f4xx_hal.h"
#endif

/* Macros
 * ------------------------------------------------------------------------- */

/* Timing samples for L0 with SYSCLK 32MHz set in SystemClock_Config() */
#if (defined (STM32L0) || defined(USE_STM32L0XX_NUCLEO))
#if (!defined(NUCLEO_I2C_EXPBD_TIMING_100KHZ))
#define NUCLEO_I2C_EXPBD_TIMING_100KHZ       0x10A13E56 /* Analog Filter ON, Rise Time 400ns, Fall Time 100ns */
#endif
#if (!defined(NUCLEO_I2C_EXPBD_TIMING_400KHZ))
#define NUCLEO_I2C_EXPBD_TIMING_400KHZ       0x00B1112E /* Analog Filter ON, Rise Time 250ns, Fall Time 100ns */
#endif
#endif /* USE_STM32L0XX_NUCLEO */

/* Timing samples for L4 with SYSCLK 80MHz set in SystemClock_Config() */
#if (defined (STM32L4) || defined(USE_STM32L4XX_NUCLEO))
#if (!defined(NUCLEO_I2C_EXPBD_TIMING_100KHZ))
#define NUCLEO_I2C_EXPBD_TIMING_400KHZ       0x10D1143A /* Analog Filter ON, Rise time 250ns, Fall Time 100ns */
#endif
#if (!defined(NUCLEO_I2C_EXPBD_TIMING_400KHZ))
#define NUCLEO_I2C_EXPBD_TIMING_1000KHZ      0x00D00E28 /* Analog Filter ON, Rise time 120ns, Fall time 25ns */
#endif
#endif /* USE_STM32L4XX_NUCLEO */

/* Definition for I2Cx clock resources */
#define I2Cx I2C1
#define I2Cx_CLK_ENABLE() __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define I2Cx_FORCE_RESET() __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET() __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx pins */
#define I2Cx_SCL_PIN GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT GPIOB
#define I2Cx_SCL_AF GPIO_AF4_I2C1
#define I2Cx_SDA_PIN GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT GPIOB
#define I2Cx_SDA_AF GPIO_AF4_I2C1

/* Definition for Trusted Objects secure element */
#define TO_SE_ADDRESS (0x50 << 1u)
#define TO_SE_TIMEOUT (10000u)


/* Private variables
 * ------------------------------------------------------------------------- */

static I2C_HandleTypeDef I2C_Handle;


/* Public function definitions
 * ------------------------------------------------------------------------- */

/*
 * See header file
 */
TO_lib_ret_t TO_data_init(void)
{
	I2C_Handle.Instance = I2Cx;

	I2C_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
#if ((defined (STM32F4)) || (defined (STM32L1)))
	I2C_Handle.Init.ClockSpeed = 400000;
	I2C_Handle.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
#elif (defined (STM32L0) || defined(STM32L4))
	I2C_Handle.Init.Timing = NUCLEO_I2C_EXPBD_TIMING_400KHZ;    /* 400KHz */
#endif
	I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	I2C_Handle.Init.OwnAddress1 = TO_SE_ADDRESS;
	I2C_Handle.Init.OwnAddress2 = 0xFE;

	if(HAL_I2C_Init(&I2C_Handle) != HAL_OK) {
		return TO_ERROR;
	}

	return TO_OK;
}

/*
 * See header file
 */
TO_lib_ret_t TO_data_fini(void)
{
	if(HAL_I2C_DeInit(&I2C_Handle) != HAL_OK) {
		return TO_ERROR;
	}

	return TO_OK;
}

/*
 * See header file
 */
TO_lib_ret_t TO_data_read(void *data, unsigned int length)
{
	HAL_StatusTypeDef status;

#if 0
    #include <stdlib.h>
	const uint8_t* input = (const uint8_t*)data;

	printf("I2C READ \"\r\n");
	for (int i=0; input && i <length; i++) {
		printf("%02x", input[i]);
	}
	printf("\"\r\n");
#endif

	status = HAL_I2C_Master_Receive(&I2C_Handle, (uint16_t)TO_SE_ADDRESS,
			(uint8_t *)data, length, TO_SE_TIMEOUT);
	if(status != HAL_OK) {
		return TO_DEVICE_READ_ERROR;
	}

	return TO_OK;
}

/*
 * See header file
 */
TO_lib_ret_t TO_data_write(const void *data, unsigned int length)
{
	HAL_StatusTypeDef status;

#if 0
    #include <stdlib.h>
	const char* input = (const char*)data;

	printf("I2C WRITE \"\r\n");
	for (int i=0; input && i <length; i++) {
		printf("%02x", input[i]);
	}
	printf("\"\r\n");
#endif

	status = HAL_I2C_Master_Transmit(&I2C_Handle, (uint16_t)TO_SE_ADDRESS,
			(uint8_t*)data, length, TO_SE_TIMEOUT);
	if(status != HAL_OK) {
		return TO_DEVICE_WRITE_ERROR;
	}

	return TO_OK;
}


/* External function definitions
 * ------------------------------------------------------------------------- */

/**
 * @brief I2C MSP Initialization
 * @param hi2c: I2C handle pointer
 *
 * This function configures the hardware resources used in this example:
 * - Peripheral's clock enable
 * - Peripheral's GPIO Configuration
 * - DMA configuration for transmission request by peripheral
 * - NVIC configuration for DMA interrupt request enable
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
	  GPIO_InitTypeDef  GPIO_Init;

	  /* Enable GPIO TX/RX clock */
	  I2Cx_SCL_GPIO_CLK_ENABLE();
	  I2Cx_SDA_GPIO_CLK_ENABLE();

	  /* Enable I2C1 clock */
	  I2Cx_CLK_ENABLE();

	  /* I2C TX GPIO pin configuration  */
	  GPIO_Init.Pin = I2Cx_SCL_PIN;
	  GPIO_Init.Mode = GPIO_MODE_AF_OD;
	  GPIO_Init.Pull = GPIO_PULLUP;
	  GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	  GPIO_Init.Alternate = I2Cx_SCL_AF;

	  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_Init);

	  /* I2C RX GPIO pin configuration  */
	  GPIO_Init.Pin = I2Cx_SDA_PIN;
	  GPIO_Init.Alternate = I2Cx_SDA_AF;

	  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_Init);
}

/**
 * @brief I2C MSP De-Initialization
 * @param hi2c: I2C handle pointer
 *
 * This function frees the hardware resources used in this example:
 * - Disable the Peripheral's clock
 * - Revert GPIO, DMA and NVIC configuration to their default state
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c)
{
	  /* Reset I2C */
	  I2Cx_FORCE_RESET();
	  I2Cx_RELEASE_RESET();

	  /* Deconfigure I2C Tx as alternate function  */
	  HAL_GPIO_DeInit(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);

	  /* Deconfigure I2C Rx as alternate function  */
	  HAL_GPIO_DeInit(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);
}

/**
 * @brief I2C error callbacks.
 * @note This example shows a simple way to report transfer error, and you can
 * add your own implementation.
 * @param hi2c: I2C handle pointer
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	/* Implement own ErrorCallback */
}

#if 0
/*
 * THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * ----------------------------------------------------------------------------
 * --  _____       ______  _____                                              -
 * -- |_   _|     |  ____|/ ____|                                             -
 * --   | |  _ __ | |__  | (___    Institute of Embedded Systems              -
 * --   | | | '_ \|  __|  \___ \   Zurich University of                       -
 * --  _| |_| | | | |____ ____) |  Applied Sciences                           -
 * -- |_____|_| |_|______|_____/   8401 Winterthur, Switzerland               -
 * ----------------------------------------------------------------------------
 */

/**
 * @file stm32_hal.c
 * @brief Implementation of STM32 wrapper for Trusted Objects libTO, using
 * only the ST HAL drivers.
 * @modified by Trusted Objects
 */

#include "stm32l4s5i_iot01.h"
#include "TODRV_HSE_i2c_wrapper.h"

/* Private variables
 * ------------------------------------------------------------------------- */
/* Definition for Trusted Objects secure element */
#define TO_SE_ADDRESS (0x50 << 1u)
#define TO_SE_TIMEOUT (10000u)

extern void I2Cx_SE_Init(I2C_HandleTypeDef *i2c_handler);
extern void I2Cx_SE_DeInit(I2C_HandleTypeDef *i2c_handler);

extern void TO_Power_GPIO_Init();
extern void TO_Power_GPIO_Deinit();
extern void TO_Power_GPIO_On();
extern void TO_Power_GPIO_Off();

static I2C_HandleTypeDef I2C_Handle;

/* Public function definitions
 * ------------------------------------------------------------------------- */

/*
 * See header file
 */
TO_lib_ret_t TO_data_init(void)
{
	I2Cx_SE_Init(&I2C_Handle);
	TO_Power_GPIO_Init();
	TO_Power_GPIO_Off();
	HAL_Delay(10);
	return TO_OK;
}

/*
 * See header file
 */
TO_lib_ret_t TO_data_fini(void)
{
	I2Cx_SE_DeInit(&I2C_Handle);
	TO_Power_GPIO_Deinit();
	TO_Power_GPIO_On();
	return TO_OK;
}

/*
 * See header file
 */
TO_lib_ret_t TO_data_read(void *data, unsigned int length)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Receive(&I2C_Handle, (uint16_t)TO_SE_ADDRESS,
			(uint8_t *)data, length, TO_SE_TIMEOUT);
	if(status != HAL_OK) {
		return TO_DEVICE_READ_ERROR;
	}

	return TO_OK;
}

/*
 * See header file
 */
TO_lib_ret_t TO_data_write(const void *data, unsigned int length)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(&I2C_Handle, (uint16_t)TO_SE_ADDRESS,
			(uint8_t*)data, length, TO_SE_TIMEOUT);
	if(status != HAL_OK) {
		return TO_DEVICE_WRITE_ERROR;
	}

	return TO_OK;
}
#endif
