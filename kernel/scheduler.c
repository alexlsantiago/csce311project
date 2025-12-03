#include "scheduler.h"
#include "task.h"
#include "sync.h"
#include "types.h"

static task_t* ready_queue = NULL;
static spinlock_t scheduler_lock;

void scheduler_init(void) {
    spinlock_init(&scheduler_lock);
}

void scheduler_add_task(task_t* task) {
    spinlock_lock(&scheduler_lock);
    
    task->next = ready_queue;
    ready_queue = task;
    
    spinlock_unlock(&scheduler_lock);
}

task_t* scheduler_get_next_task(void) {
    spinlock_lock(&scheduler_lock);
    
    /* Round-robin scheduler */
    task_t* task = ready_queue;
    if (task) {
        ready_queue = task->next;
        task->next = NULL;
    }
    
    spinlock_unlock(&scheduler_lock);
    return task;
}

void scheduler_yield(void) {
    task_t* current = get_current_task();
    
    if (current && current->state == TASK_RUNNING) {
        current->state = TASK_READY;
        scheduler_add_task(current);
    }
    
    /* Get next task */
    task_t* next = scheduler_get_next_task();
    if (!next) {
        /* No ready tasks, keep current or use idle */
        next = current;
        if (!next) {
            /* Fallback - would need access to idle task */
            return;
        }
    }
    
    next->state = TASK_RUNNING;
    set_current_task(next);
    
    /* Context switch would happen here in a real implementation */
    /* For now, we'll just update the current task pointer */
}

