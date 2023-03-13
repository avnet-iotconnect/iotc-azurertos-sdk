#include <tx_api.h>

#include "r_cg_macrodriver.h"
#include <platform.h>
#include <Config_SCI5.h>

#include "demo_printf.h"


TX_MUTEX demo_printf_mutex;
TX_SEMAPHORE demo_printf_semaphore;

void demo_printf_init(void)
{
    tx_mutex_create(&demo_printf_mutex, "Demo printf mutex", TX_INHERIT);

    tx_semaphore_create(&demo_printf_semaphore, "Demo printf semaphore", 0u);
}

void demo_printf_transmit_end(void)
{
    tx_semaphore_put(&demo_printf_semaphore);
}

void my_sw_charput_function(char c)
{
    tx_mutex_get(&demo_printf_mutex, TX_WAIT_FOREVER);

    R_Config_SCI5_Serial_Send(&c, 1u);

    tx_semaphore_get(&demo_printf_semaphore, TX_WAIT_FOREVER);

    tx_mutex_put(&demo_printf_mutex);

	return;
}

