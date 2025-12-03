#include "uart.h"
#include "types.h"

void console_init(void) {
    uart_init();
}

void console_puts(const char* s) {
    uart_puts(s);
}

char console_getchar(void) {
    return uart_getchar();
}

