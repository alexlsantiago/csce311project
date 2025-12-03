#include "uart.h"
#include "types.h"

#define UART_RBR 0x00  /* Receive Buffer Register */
#define UART_THR 0x00  /* Transmit Holding Register */
#define UART_IER 0x01  /* Interrupt Enable Register */
#define UART_IIR 0x02  /* Interrupt ID Register */
#define UART_FCR 0x02  /* FIFO Control Register */
#define UART_LCR 0x03  /* Line Control Register */
#define UART_MCR 0x04  /* Modem Control Register */
#define UART_LSR 0x05  /* Line Status Register */
#define UART_MSR 0x06  /* Modem Status Register */
#define UART_SCR 0x07  /* Scratch Register */

#define UART_LSR_THRE 0x20  /* Transmit Holding Register Empty */
#define UART_LSR_DR   0x01  /* Data Ready */

static volatile uint8_t* uart_base = (volatile uint8_t*)UART_BASE;

static inline uint8_t uart_read_reg(int reg) {
    return uart_base[reg];
}

static inline void uart_write_reg(int reg, uint8_t val) {
    uart_base[reg] = val;
}

void uart_init(void) {
    /* UART is already initialized by QEMU, but we can set it up */
    /* For now, just ensure it's ready */
}

void uart_putchar(char c) {
    /* Wait for transmit buffer to be empty */
    while (!(uart_read_reg(UART_LSR) & UART_LSR_THRE));
    uart_write_reg(UART_THR, c);
    
    /* Add carriage return for newline */
    if (c == '\n') {
        while (!(uart_read_reg(UART_LSR) & UART_LSR_THRE));
        uart_write_reg(UART_THR, '\r');
    }
}

char uart_getchar(void) {
    /* Wait for data to be available */
    while (!(uart_read_reg(UART_LSR) & UART_LSR_DR));
    return uart_read_reg(UART_RBR);
}

void uart_puts(const char* s) {
    while (*s) {
        uart_putchar(*s++);
    }
}

