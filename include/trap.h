#ifndef TRAP_H
#define TRAP_H

#include "types.h"

void trap_init();
void trap_handler(uint64_t cause, uint64_t epc);

#endif
