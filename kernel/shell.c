#include "shell.h"
#include "uart.h"
#include "fs.h"
#include "task.h"
#include "string.h"
#include "types.h"
#include "printf.h"

#define MAX_LINE 256
#define MAX_ARGS 16

static void cmd_help(void) {
    uart_puts("Available commands:\r\n");
    uart_puts("  help     - Show this help\r\n");
    uart_puts("  ls       - List files\r\n");
    uart_puts("  cat <file> - Display file contents\r\n");
    uart_puts("  echo <text> - Echo text\r\n");
    uart_puts("  ps       - List processes\r\n");
    uart_puts("  fork     - Fork current process\r\n");
    uart_puts("  exit     - Exit shell\r\n");
}

static void cmd_ls(void) {
    char buf[1024];
    if (fs_list_files(buf, sizeof(buf)) >= 0) {
        uart_puts(buf);
    } else {
        uart_puts("Error listing files\r\n");
    }
}

static void cmd_cat(char* filename) {
    if (!filename) {
        uart_puts("Usage: cat <filename>\r\n");
        return;
    }
    
    char buf[512];
    int total = 0;
    int n;
    
    while ((n = fs_read_file(filename, buf, sizeof(buf) - 1, total)) > 0) {
        buf[n] = '\0';
        uart_puts(buf);
        total += n;
    }
    
    if (total == 0) {
        uart_puts("File not found or empty\r\n");
    } else {
        uart_puts("\r\n");
    }
}

static void cmd_echo(char* text) {
    if (text) {
        uart_puts(text);
        uart_puts("\r\n");
    }
}

static void cmd_ps(void) {
    uart_puts("PID  NAME     STATE\r\n");
    /* Would iterate through tasks - simplified for now */
    uart_puts("0    idle     RUNNING\r\n");
}

static void cmd_fork(void) {
    int pid = task_fork();
    if (pid > 0) {
        printf("Forked child process: %d\r\n", pid);
    } else {
        uart_puts("Fork failed\r\n");
    }
}

static int parse_line(char* line, char** argv) {
    int argc = 0;
    int in_word = 0;
    
    for (int i = 0; line[i] && argc < MAX_ARGS; i++) {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\r' || line[i] == '\n') {
            if (in_word) {
                line[i] = '\0';
                in_word = 0;
            }
        } else {
            if (!in_word) {
                argv[argc++] = &line[i];
                in_word = 1;
            }
        }
    }
    
    if (in_word) {
        line[strlen(line)] = '\0';
    }
    
    argv[argc] = NULL;
    return argc;
}

void shell_start(void) {
    char line[MAX_LINE];
    int pos = 0;
    
    uart_puts("RISC-V OS Shell v1.0\r\n");
    uart_puts("Type 'help' for commands\r\n\r\n");
    
    while (1) {
        uart_puts("$ ");
        
        /* Read line */
        pos = 0;
        while (pos < MAX_LINE - 1) {
            char c = uart_getchar();
            
            if (c == '\r' || c == '\n') {
                uart_puts("\r\n");
                break;
            } else if (c == '\b' || c == 127) {
                if (pos > 0) {
                    pos--;
                    uart_puts("\b \b");
                }
            } else if (c >= 32 && c < 127) {
                line[pos++] = c;
                uart_putchar(c);
            }
        }
        
        line[pos] = '\0';
        
        if (pos == 0) {
            continue;
        }
        
        /* Parse command */
        char* argv[MAX_ARGS];
        int argc = parse_line(line, argv);
        
        if (argc == 0) {
            continue;
        }
        
        /* Execute command */
        if (strcmp(argv[0], "help") == 0) {
            cmd_help();
        } else if (strcmp(argv[0], "ls") == 0) {
            cmd_ls();
        } else if (strcmp(argv[0], "cat") == 0) {
            cmd_cat(argv[1]);
        } else if (strcmp(argv[0], "echo") == 0) {
            cmd_echo(argv[1]);
        } else if (strcmp(argv[0], "ps") == 0) {
            cmd_ps();
        } else if (strcmp(argv[0], "fork") == 0) {
            cmd_fork();
        } else if (strcmp(argv[0], "exit") == 0) {
            uart_puts("Exiting shell...\r\n");
            break;
        } else {
            printf("Unknown command: %s\r\n", argv[0]);
            uart_puts("Type 'help' for available commands\r\n");
        }
    }
}

