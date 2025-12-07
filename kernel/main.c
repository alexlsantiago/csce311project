// kernel/main.c
#include "uart.h"
#include "memory.h"
#include "task.h"
#include "scheduler.h"
#include "fs.h"
#include "shell.h"
#include "timer.h"

extern char _bss_start[];
extern char _bss_end[];

// void kernel_main(void) {
//     // Zero BSS section
//     for (char *p = _bss_start; p < _bss_end; p++) {
//         *p = 0;
//     }

//     uart_puts("\r\n=== RISC-V OS Boot ===\r\n");

//     uart_puts("Initializing memory...\r\n");
//     memory_init();

//     uart_puts("Initializing timer...\r\n");
//     timer_init();

//     uart_puts("Initializing file system...\r\n");
//     fs_init();

//     uart_puts("Initializing task system...\r\n");
//     scheduler_init();
//     task_init();

//     uart_puts("Starting shell...\r\n\r\n");

//     // Jump into the shell; this never returns in normal use.
//     shell_start();

//     // If shell ever returns, just idle.
//     while (1) {
//         __asm__ volatile ("wfi");
//     }
// }
void kernel_main(void) {
    uart_puts("\r\n=== RISC-V OS Boot ===\r\n");

    uart_puts("Initializing memory...\r\n");
    memory_init();

    uart_puts("Initializing timer...\r\n");
    timer_init();

    uart_puts("Initializing file system...\r\n");
    fs_init();

    uart_puts("Initializing task system...\r\n");
    scheduler_init();
    task_init();

   

    uart_puts("Starting shell...\r\n\r\n");
     uart_puts("Starting shell task...\r\n");
    task_t* shell_task = task_create("shell", shell_start);
    if (!shell_task) {
        uart_puts("Failed to create shell task!\r\n");
        while(1) { __asm__ volatile ("wfi"); }
    }
    set_current_task(shell_task);

    shell_start();

    while (1) {
        __asm__ volatile ("wfi");
    }
}
