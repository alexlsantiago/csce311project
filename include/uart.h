#ifndef UART_H
#define UART_H

#include "types.h"

#define UART_BASE 0x10000000

void uart_init(void);
void uart_putchar(char c);
char uart_getchar(void);
void uart_puts(const char* s);

#endif

