// kernel/timer.c
#include "timer.h"
#include "types.h"

void timer_init(void) {
    // No interrupt setup for now – just use time CSR.
}

void timer_tick(void) {
    // Not used when we rely on the time CSR directly.
}

uint64_t timer_get_ticks(void) {
    uint64_t t;
    // On RV64, time is a 64-bit CSR; a single read is enough.
    asm volatile("csrr %0, time" : "=r"(t));
    return t;
}
