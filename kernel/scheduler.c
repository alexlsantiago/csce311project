#include "scheduler.h"
#include "task.h"
#include "sync.h"
#include "types.h"
#include "timer.h"

static task_t* ready_queue = NULL;
static spinlock_t scheduler_lock;

void scheduler_init(void) {
    spinlock_init(&scheduler_lock);
}

/* Add a task to the ready queue (LIFO simple queue) */
void scheduler_add_task(task_t* task) {
    if (!task) return;

    spinlock_lock(&scheduler_lock);

    task->next = ready_queue;
    ready_queue = task;

    spinlock_unlock(&scheduler_lock);
}

/* Pop the next task from the ready queue */
task_t* scheduler_get_next_task(void) {
    spinlock_lock(&scheduler_lock);

    task_t* task = ready_queue;
    if (task) {
        ready_queue = task->next;
        task->next = NULL;
    }

    spinlock_unlock(&scheduler_lock);
    return task;
}

/* NEW: expose the ready queue so shell.c can implement ps */
task_t* scheduler_get_task_list(void) {
    return ready_queue;
}

void scheduler_yield(void) {
    task_t* current = get_current_task();

    /* Put current task back on ready queue if it's still runnable */
    if (current && current->state == TASK_RUNNING) {
        current->state = TASK_READY;
        scheduler_add_task(current);
    }

    /* Get next task */
    task_t* next = scheduler_get_next_task();
    if (!next) {
        /* No ready tasks, keep current if it exists */
        next = current;
        if (!next) {
            /* Nothing to run at all – just return */
            return;
        }
    }

    next->state = TASK_RUNNING;
    set_current_task(next);

    /* A real context switch (saving/restoring registers) would go here.
       For now, we just change the current task pointer. */

       
}
