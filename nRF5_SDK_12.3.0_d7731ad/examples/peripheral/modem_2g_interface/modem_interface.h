#ifndef __MODEM_INTERFACE
#define  __MODEM_INTERFACE
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_uart(void);

void uart_enable(bool status);




#ifdef __cplusplus
}
#endif

#endif

