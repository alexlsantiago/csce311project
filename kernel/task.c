#include "task.h"
#include "memory.h"
#include "sync.h"
#include "kernel.h"
#include "paging.h"
#include "string.h"
#include "types.h"
#include "scheduler.h"

static task_t tasks[MAX_TASKS];
static int next_pid = 1;
static task_t* current_task = NULL;
static task_t* task_list = NULL;
static spinlock_t task_lock;

void task_init(void) {
    memset(tasks, 0, sizeof(tasks));
    spinlock_init(&task_lock);
    
    /* Create idle task */
    current_task = &tasks[0];
    current_task->pid = 0;
    current_task->ppid = 0;
    strcpy(current_task->name, "idle");
    current_task->state = TASK_RUNNING;
    current_task->next = NULL;
    current_task->prev = NULL;
    task_list = current_task;
}

task_t* task_create(const char* name, void (*entry)(void)) {
    spinlock_lock(&task_lock);
    
    /* Find free task slot */
    task_t* task = NULL;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == 0 || tasks[i].state == TASK_ZOMBIE) {
            task = &tasks[i];
            break;
        }
    }
    
    if (!task) {
        spinlock_unlock(&task_lock);
        return NULL;
    }
    
    /* Initialize task */
    memset(task, 0, sizeof(task_t));
    task->pid = next_pid++;
    task->ppid = current_task ? current_task->pid : 0;
    strncpy(task->name, name, TASK_NAME_LEN - 1);
    task->state = TASK_READY;
    
    /* Allocate stack */
    task->stack = get_free_page();
    if (!task->stack) {
        spinlock_unlock(&task_lock);
        return NULL;
    }
    
    /* Set up stack pointer */
    task->sp = (uint64_t)task->stack + KERNEL_STACK_SIZE;
    
    /* Set up entry point */
    task->pc = (uint64_t)entry;
    
    /* Set up page table */
    task->page_table = setup_page_table();
    
    /* Add to task list */
    task->next = task_list;
    if (task_list) {
        task_list->prev = task;
    }
    task_list = task;
    task->prev = NULL;
    
    spinlock_unlock(&task_lock);
    return task;
}

void task_exit(int code) {
    spinlock_lock(&task_lock);
    
    if (current_task) {
        current_task->state = TASK_ZOMBIE;
        current_task->exit_code = code;
        
        /* Free resources */
        if (current_task->stack) {
            free_page(current_task->stack);
        }
        if (current_task->page_table) {
            free_page(current_task->page_table);
        }
        
        /* Remove from list */
        if (current_task->prev) {
            current_task->prev->next = current_task->next;
        } else {
            task_list = current_task->next;
        }
        if (current_task->next) {
            current_task->next->prev = current_task->prev;
        }
    }
    
    spinlock_unlock(&task_lock);
    
    /* Yield to scheduler */
    scheduler_yield();
}

void task_yield(void) {
    scheduler_yield();
}

task_t* get_current_task(void) {
    return current_task;
}

void set_current_task(task_t* task) {
    current_task = task;
}

int task_fork(void) {
    /* Simplified fork - just create a new task */
    task_t* child = task_create("forked", NULL);
    if (!child) {
        return -1;
    }
    
    /* Copy parent's context */
    if (current_task) {
        memcpy(child->regs, current_task->regs, sizeof(current_task->regs));
        child->pc = current_task->pc;
        child->sp = current_task->sp;
    }
    
    return child->pid;
}

int task_exec(const char* path, char** argv) {
    /* Load and execute program */
    uint64_t entry;
    if (elf_load(path, &entry) != 0) {
        return -1;
    }
    
    if (current_task) {
        current_task->pc = entry;
        /* Reset stack */
        current_task->sp = (uint64_t)current_task->stack + KERNEL_STACK_SIZE;
    }
    
    return 0;
}

int task_wait(int pid) {
    /* Wait for child process to exit */
    spinlock_lock(&task_lock);
    
    task_t* child = NULL;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].pid == pid && tasks[i].ppid == (current_task ? current_task->pid : 0)) {
            child = &tasks[i];
            break;
        }
    }
    
    if (!child) {
        spinlock_unlock(&task_lock);
        return -1;
    }
    
    /* Wait for zombie state */
    while (child->state != TASK_ZOMBIE) {
        spinlock_unlock(&task_lock);
        task_yield();
        spinlock_lock(&task_lock);
    }
    
    int exit_code = child->exit_code;
    child->state = 0;  /* Free the slot */
    
    spinlock_unlock(&task_lock);
    return exit_code;
}

