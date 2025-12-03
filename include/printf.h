#ifndef PRINTF_H
#define PRINTF_H

/* Simple kernel printf interface.
 * Implementation is in kernel/printf.c
 */

int kprintf(const char *fmt, ...);

#endif
