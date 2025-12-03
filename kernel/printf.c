#include "types.h"
#include "uart.h"

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)

/* print unsigned 64-bit integer */
static void print_uint64(uint64_t n, int base) {
    char buf[32];
    int i = 0;

    if (n == 0) {
        uart_putchar('0');
        return;
    }

    while (n > 0) {
        int digit = n % base;
        buf[i++] = (digit < 10)
            ? ('0' + digit)
            : ('a' + (digit - 10));
        n /= base;
    }

    while (i--)
        uart_putchar(buf[i]);
}

void printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;

            /* ======== %l and %ll support ======== */
            if (*fmt == 'l') {
                fmt++;
                if (*fmt == 'l') {   // %ll
                    fmt++;
                    if (*fmt == 'u') {
                        uint64_t v = va_arg(args, unsigned long long);
                        print_uint64(v, 10);
                    } else if (*fmt == 'x') {
                        uart_puts("0x");
                        uint64_t v = va_arg(args, unsigned long long);
                        print_uint64(v, 16);
                    }
                } else {             // %l
                    if (*fmt == 'u') {
                        unsigned long v = va_arg(args, unsigned long);
                        print_uint64(v, 10);
                    }
                    else if (*fmt == 'x') {
                        uart_puts("0x");
                        unsigned long v = va_arg(args, unsigned long);
                        print_uint64(v, 16);
                    }
                }
            }
            else {

                /* ======== normal formatting ======== */
                switch (*fmt) {
                    case 'd':
                    case 'i': {
                        int val = va_arg(args, int);
                        if (val < 0) {
                            uart_putchar('-');
                            val = -val;
                        }
                        print_uint64((uint64_t)val, 10);
                        break;
                    }

                    case 'u':
                        print_uint64((uint64_t)va_arg(args, unsigned int), 10);
                        break;

                    case 'x':
                        uart_puts("0x");
                        print_uint64((uint64_t)va_arg(args, unsigned int), 16);
                        break;

                    case 'p':
                        uart_puts("0x");
                        print_uint64((uint64_t)va_arg(args, uintptr_t), 16);
                        break;

                    case 's': {
                        const char* s = va_arg(args, const char*);
                        uart_puts(s);
                        break;
                    }

                    case 'c':
                        uart_putchar(va_arg(args, int));
                        break;

                    case '%':
                        uart_putchar('%');
                        break;

                    default:
                        uart_putchar('%');
                        uart_putchar(*fmt);
                        break;
                }
            }
        }
        else {
            uart_putchar(*fmt);
        }

        fmt++;
    }

    va_end(args);
}
