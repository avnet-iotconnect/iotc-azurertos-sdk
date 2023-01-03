#ifndef HARDWARE_SETUP_H
#define HARDWARE_SETUP_H

#include "r_gpio_rx_config.h"
#include "r_gpio_rx_if.h"

typedef enum {
	LED1 = GPIO_PORT_B_PIN_0,
	LED2 = GPIO_PORT_B_PIN_2
} RX65N_LED_PIN;

typedef enum {
	ON = 0,
	OFF = 1
} RX65N_LED_STATE;

void platform_setup(void);
void set_led(RX65N_LED_PIN led, RX65N_LED_STATE state);
int read_user_switch();

#endif // HARDWARE_SETUP_H

