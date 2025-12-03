#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"

void scheduler_init(void);
void scheduler_yield(void);
void scheduler_add_task(task_t* task);
task_t* scheduler_get_next_task(void);

#endif

