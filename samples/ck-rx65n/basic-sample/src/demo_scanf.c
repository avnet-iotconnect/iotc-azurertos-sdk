#include <tx_api.h>

#include "r_cg_macrodriver.h"
#include <platform.h>
#include <Config_SCI5.h>

#include "demo_scanf.h"
#include "demo_printf.h"
#include "app_config.h"

TX_MUTEX demo_scanf_mutex;
TX_SEMAPHORE demo_scanf_semaphore;

void demo_scanf_init(void)
{
    tx_mutex_create(&demo_scanf_mutex, "Demo scanf mutex", TX_INHERIT);

    tx_semaphore_create(&demo_scanf_semaphore, "Demo scanf semaphore", 0u);
}

void demo_scanf_transmit_end(void)
{
    tx_semaphore_put(&demo_scanf_semaphore);
}

char my_sw_charget_function(void)
{
    char c = 0;

    tx_mutex_get(&demo_scanf_mutex, TX_WAIT_FOREVER);


    if(R_Config_SCI5_Serial_Receive(&c, 1u) != MD_OK) {
        c = 0;
    }

    tx_semaphore_get(&demo_scanf_semaphore, TX_WAIT_FOREVER);

    tx_mutex_put(&demo_scanf_mutex);

    // echo character back...
    my_sw_charput_function(c);

    return c;
}

char my_sw_charget_function_timeout(uint16_t timeout)
{
    char c = CLI_NO_INPUT;

    tx_mutex_get(&demo_scanf_mutex, TX_WAIT_FOREVER);

    if(R_Config_SCI5_Serial_Receive(&c, 1u) != MD_OK) {
        printf("error reading\r\n");
        c = CLI_NO_INPUT;
    }

    tx_semaphore_get(&demo_scanf_semaphore, timeout);

    tx_mutex_put(&demo_scanf_mutex);

    // echo character back...

    my_sw_charput_function(c);

    return c;
}
