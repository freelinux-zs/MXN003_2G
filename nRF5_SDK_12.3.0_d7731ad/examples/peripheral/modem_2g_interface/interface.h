#ifndef __INTERFACE_H
#define  __INTERFACE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void uart_putbyte(const char *fmt, ...);
bool mt2503_custom_common_hdlr(char *full_cmd_string);

#ifdef __cplusplus
}
#endif

#endif



