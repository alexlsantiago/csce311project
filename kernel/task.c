#include "task.h"
#include "memory.h"
#include "string.h"
#include "scheduler.h"
#include "sync.h"
#include <kernel.h>

static task_t tasks[MAX_TASKS];
static int next_pid = 1;
static task_t* current_task = NULL;
static spinlock_t task_lock;
// At the top of task.c, after includes:
static void add_task_to_list(task_t* task);
static void remove_task_from_list(task_t* task);
task_t* get_task_list(void);


void task_init(void) {
    memset(tasks, 0, sizeof(tasks));
    spinlock_init(&task_lock);

    /* Create idle task (PID 0) */
    current_task = &tasks[0];
    current_task->pid = 0;
    current_task->ppid = 0;
    current_task->state = TASK_RUNNING;
    strcpy(current_task->name, "idle");
    current_task->stack = NULL;
    current_task->next = NULL;
    add_task_to_list(current_task);
}

task_t* task_create(const char* name, void (*entry)(void)) {
    spinlock_lock(&task_lock);

    /* Find free slot */
    task_t* task = NULL;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_ZOMBIE || tasks[i].state == 0) {
            task = &tasks[i];
            break;
        }
    }
    if (!task) {
        spinlock_unlock(&task_lock);
        return NULL;
    }

    memset(task, 0, sizeof(task_t));
    task->pid = next_pid++;
    task->ppid = current_task ? current_task->pid : 0;
    strncpy(task->name, name, TASK_NAME_LEN-1);
    task->state = TASK_READY;

    /* Allocate stack */
    task->stack = get_free_page();
    task->sp = task->stack ? ((uint64_t)task->stack + KERNEL_STACK_SIZE) : 0;

    /* Entry point */
    task->pc = (uint64_t)entry;

    /* Add to scheduler ready queue */
    scheduler_add_task(task);
    add_task_to_list(task);

    spinlock_unlock(&task_lock);
    return task;
}

void task_exit(int code) {
    spinlock_lock(&task_lock);

    if (current_task) {
        current_task->state = TASK_ZOMBIE;
        current_task->exit_code = code;

        if (current_task->stack) free_page(current_task->stack);
        remove_task_from_list(current_task);
    }

    spinlock_unlock(&task_lock);
    scheduler_yield(); // switch to next task
}

int task_fork(void) {
    task_t* child = task_create("forked", NULL);
    if (!child) return -1;

    if (current_task) {
        memcpy(child->regs, current_task->regs, sizeof(current_task->regs));
        child->pc = current_task->pc;
        child->sp = current_task->sp;
    }

    return child->pid;
}

task_t* get_current_task(void) {
    return current_task;
}

void set_current_task(task_t* task) {
    current_task = task;
}

/* Helpers for shell ps: linked list of all tasks */
static task_t* task_list_head = NULL;

void add_task_to_list(task_t* task) {
    task->next = task_list_head;
    if (task_list_head) task_list_head->prev = task;
    task_list_head = task;
    task->prev = NULL;
}

void remove_task_from_list(task_t* task) {
    if (task->prev) task->prev->next = task->next;
    else task_list_head = task->next;

    if (task->next) task->next->prev = task->prev;
}

task_t* get_task_list(void) {
    return task_list_head;
}
// In task.c, add these at the bottom (after get_task_list)

#include "elf.h"   // for elf_load

int task_exec(const char* path, char** argv) {
    uint64_t entry;
    if (elf_load(path, &entry) != 0) {
        return -1; // load failed
    }

    if (current_task) {
        current_task->pc = entry;
        // reset stack pointer
        current_task->sp = (uint64_t)current_task->stack + KERNEL_STACK_SIZE;
    }
    return 0;
}

int task_wait(int pid) {
    spinlock_lock(&task_lock);

    task_t* child = NULL;
    for (task_t* t = task_list_head; t; t = t->next) {
        if (t->pid == pid && t->ppid == (current_task ? current_task->pid : 0)) {
            child = t;
            break;
        }
    }
    if (!child) {
        spinlock_unlock(&task_lock);
        return -1;
    }

    while (child->state != TASK_ZOMBIE) {
        spinlock_unlock(&task_lock);
        scheduler_yield();
        spinlock_lock(&task_lock);
    }

    int exit_code = child->exit_code;
    child->state = 0; // free slot
    spinlock_unlock(&task_lock);
    return exit_code;
}
