#include <tx_api.h>

#include "r_cg_macrodriver.h"
#include <platform.h>
#include <Config_SCI5.h>

#include "demo_scanf.h"
#include "demo_printf.h"
#include "app_config.h"

TX_MUTEX demo_scanf_mutex;
TX_SEMAPHORE demo_scanf_semaphore;

extern volatile uint8_t * gp_sci5_rx_address;

#define MAX_IOTC_CONFIG_BUFF_LEN 256

void demo_scanf_init(void)
{
    tx_mutex_create(&demo_scanf_mutex, "Demo scanf mutex", TX_INHERIT);

    tx_semaphore_create(&demo_scanf_semaphore, "Demo scanf semaphore", 0u);
}

void demo_scanf_transmit_end(void)
{
    tx_semaphore_put(&demo_scanf_semaphore);
    //printf("transmit end\r\n");
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

char my_sw_charget_function_variable_length(char **buff, uint8_t len, uint16_t timeout)
{
    char c = 0;

    if (len == 0){
        printf("impossible buffer len %d\r\n", len);
        return 0;
    }

    *buff = (char*)calloc(len+1, sizeof(char));

    if (!(*buff)){
        printf("failed to calloc\r\n");
        return 0;
    } 

    tx_mutex_get(&demo_scanf_mutex, TX_WAIT_FOREVER);

    if(R_Config_SCI5_Serial_Receive(*buff, len) != MD_OK) {
        c = 0;
    }

    tx_semaphore_get(&demo_scanf_semaphore, timeout);

    tx_mutex_put(&demo_scanf_mutex);

    // echo character back...

    for (int i = 0; i < len; i++){
    	if (*buff[i] == '\0'){
    		break;
    	}
    	my_sw_charput_function(*buff[i]);
    }


    return c;
}


char my_sw_get_string(char *buff, uint16_t len)
{
    char c = 0;

    if (len == 0){
        printf("impossible buffer len %d\r\n", len);
        return 0;
    }

    for (int i = 0; i < len; i++){
        buff[i] = 0;
    }

    char test_buff[MAX_IOTC_CONFIG_BUFF_LEN] = {0};

    tx_mutex_get(&demo_scanf_mutex, TX_WAIT_FOREVER);
    
    if(R_Config_SCI5_Serial_Receive(&test_buff, len) != MD_OK) {
        c = 0;
    }

    tx_semaphore_get(&demo_scanf_semaphore, TX_WAIT_FOREVER);

    tx_mutex_put(&demo_scanf_mutex);
    
    memcpy(buff, test_buff, strlen(test_buff));
    
    
    for (int i = 0; i < len; i++){
    	if (buff[i] == '\0' || buff[i] == 13){
            buff[i] = '\0';
    		break;
    	}
        // echo character back...
    	my_sw_charput_function(buff[i]);
    }


    return c;
}

char my_sw_get_string_timeout(char *buff, uint16_t len, uint16_t timeout)
{
    char c = 0;

    if (len == 0){
        printf("impossible buffer len %d\r\n", len);
        return 0;
    }

    for (int i = 0; i < len; i++){
        buff[i] = 0;
    }

    char test_buff[MAX_IOTC_CONFIG_BUFF_LEN] = {0};

    tx_mutex_get(&demo_scanf_mutex, TX_WAIT_FOREVER);
    
    if(R_Config_SCI5_Serial_Receive(&test_buff, len) != MD_OK) {
        c = 0;
    }


    tx_semaphore_get(&demo_scanf_semaphore, timeout);

    tx_mutex_put(&demo_scanf_mutex);

    // echo character back...
    memcpy(buff, test_buff, strlen(test_buff));    

    for (int i = 0; i < len; i++){
    	if (buff[i] == '\0' || buff[i] == 13){
            buff[i] = '\0';
    		break;
    	}
    	my_sw_charput_function(buff[i]);
    }


    return c;
}
