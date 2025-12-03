#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void timer_init();
void timer_tick();
uint64_t timer_get_ticks();

#endif
