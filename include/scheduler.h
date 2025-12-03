#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"
#include "task.h"

/*
 * Initialize scheduler
 */
void scheduler_init(void);

/*
 * Add a task to the ready queue
 */
void scheduler_add_task(task_t* task);

/*
 * Pop the next runnable task
 */
task_t* scheduler_get_next_task(void);

/*
 * Return the head of ready queue
 * (used by shell to display tasks)
 */
task_t* scheduler_get_task_list(void);

/*
 * Yield CPU to next task
 */
void scheduler_yield(void);

#endif
