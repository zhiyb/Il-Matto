// Author: Yubo Zhi (normanzyb@gmail.com)

#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

void uart_init();
FILE *uart_io();

#ifdef __cplusplus
}
#endif

#endif
