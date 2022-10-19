#include "common_hardware_code.h"

#include "atmel_start.h"
#include "hal_gpio.h"

/*TRNG Registers Address  */
#define TRNG_REGS       ((trng_registers_t*)0x42002800)    

const char output_string[]="sample runs successfully!";
/*function prototype*/
uint32_t TRNG_ReadData(void);
void TRNG_Initialize(void);
uint32_t hardware_rand(void);

VOID board_setup(void)
{
  
#ifdef TX_ENABLE_EXECUTION_CHANGE_NOTIFY

    /* Enable execution profile.  */
    CoreDebug -> DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT -> CTRL |= DWT_CTRL_CYCCNTENA_Msk;

#endif
    
    atmel_start_init();
    
    TRNG_Initialize();
    
    gpio_set_pin_pull_mode(PIN_PB31, GPIO_PULL_OFF);
    gpio_set_pin_function(PIN_PB31, PINMUX_PB31A_EIC_EXTINT15);
    
    /* Init the seed.  */
    srand(hardware_rand());
    
}

uint32_t hardware_rand(void)
{
uint32_t random_number;

    random_number = TRNG_ReadData();
    
    return random_number;

}
void TRNG_Initialize( void )
{
void *hw = (void *)MCLK;
    hri_mclk_set_APBCMASK_TRNG_bit(hw);
}

uint32_t TRNG_ReadData( void )
{
	TRNG_REGS->TRNG_CTRLA |= TRNG_CTRLA_ENABLE_Msk;
	while(((TRNG_REGS->TRNG_INTFLAG) & (TRNG_INTFLAG_DATARDY_Msk)) != TRNG_INTFLAG_DATARDY_Msk);			
        TRNG_REGS->TRNG_CTRLA &= ~(TRNG_CTRLA_ENABLE_Msk);
	return (TRNG_REGS->TRNG_DATA);
}
