#ifndef TASK_H
#define TASK_H

#include "types.h"
#include "sync.h"

/* Max length of a task name (including null terminator) */
#ifndef TASK_NAME_LEN
#define TASK_NAME_LEN 32
#endif



typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_ZOMBIE
} task_state_t;

typedef struct task {
    uint64_t regs[32];      /* RISC-V registers */
    uint64_t pc;            /* Program counter */
    uint64_t sp;            /* Stack pointer */
    uint64_t satp;          /* Page table register */
    task_state_t state;
    int pid;
    int ppid;
    char name[TASK_NAME_LEN];
    void* stack;
    void* page_table;
    struct task* next;
    struct task* prev;
    struct task* next_ready;
    mutex_t* wait_mutex;
    int exit_code;
} task_t;

/* Task API */
task_t* task_create(const char* name, void (*entry)(void));
void task_exit(int code);
void task_yield(void);
task_t* get_current_task(void);
int task_fork(void);
int task_exec(const char* path, char** argv);
int task_wait(int pid);

/* Initialization and internal helpers */
void task_init(void);
void set_current_task(task_t* task);
task_t* get_task_list(void);  // Expose the internal list to scheduler.c


#endif
