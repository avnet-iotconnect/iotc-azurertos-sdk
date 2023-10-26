#ifndef DEMO_SCANF_H
#define DEMO_SCANF_H

#define CLI_NO_INPUT 3 // ascii EOT

void demo_scanf_init(void);
char my_sw_charget_function_timeout(uint16_t timeout);

char my_sw_get_string(char *buff, uint16_t len);
char my_sw_get_string_timeout(char *buff, uint16_t len, uint16_t timeout);

#endif
