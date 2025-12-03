#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

/* Memory management */
#define PAGE_SIZE 4096
#define KERNEL_STACK_SIZE (PAGE_SIZE * 4)
#define USER_STACK_SIZE (PAGE_SIZE * 2)

/* Task management */
#define MAX_TASKS 32
#define TASK_NAME_LEN 32

/* File system */
#define MAX_FILES 64
#define MAX_FILENAME 256
#define BLOCK_SIZE 512
#define FS_BLOCKS 2048

/* System calls */
#define SYS_EXIT 1
#define SYS_WRITE 2
#define SYS_READ 3
#define SYS_FORK 4
#define SYS_EXEC 5
#define SYS_WAIT 6
#define SYS_OPEN 7
#define SYS_CLOSE 8
#define SYS_READ_FS 9
#define SYS_WRITE_FS 10

/* Privilege levels */
#define MACHINE_MODE 3
#define SUPERVISOR_MODE 1
#define USER_MODE 0

void kernel_init(void);
void panic(const char *msg);

#endif

