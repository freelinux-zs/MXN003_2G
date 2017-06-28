#ifndef __MODEM_INTERFACE
#define  __MODEM_INTERFACE
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_uart(void);

void uart_enable(bool status);


bool mt2503_custom_common_hdlr(char *full_cmd_string);

#ifdef __cplusplus
}
#endif

#endif

