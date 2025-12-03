#include "kernel.h"
#include "uart.h"
#include "memory.h"
#include "task.h"
#include "scheduler.h"
#include "fs.h"
#include "shell.h"

extern char _bss_start[];
extern char _bss_end[];
extern char _stack_top[];

void kernel_init(void) {
    /* Initialize UART for output */
    uart_init();
    uart_puts("\r\n=== RISC-V OS Boot ===\r\n");
    
    /* Initialize memory management */
    uart_puts("Initializing memory...\r\n");
    memory_init();
    
    /* Initialize file system */
    uart_puts("Initializing file system...\r\n");
    fs_init();
    
    /* Initialize task system */
    uart_puts("Initializing task system...\r\n");
    scheduler_init();
    task_init();
    
    /* Start shell */
    uart_puts("Starting shell...\r\n\r\n");
    shell_start();
    
    /* Should never reach here */
    panic("Kernel init returned");
}

void panic(const char *msg) {
    uart_puts("\r\nKERNEL PANIC: ");
    uart_puts(msg);
    uart_puts("\r\n");
    while(1) {
        asm volatile("wfi");
    }
}

