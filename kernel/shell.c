// #include "shell.h"
// #include "uart.h"
// #include "string.h"
// #include "printf.h"
// #include "task.h"
// #include "fs.h"
// #include "scheduler.h"
// #include "timer.h"
// #include "memory.h"
// #include <stdio.h>


// #define INPUT_BUF 128
// static char input_buf[INPUT_BUF];

// /* very simple strtok */
// static char* strtok_simple(char *str, char delim) {
//     static char* saved;
//     if (str)
//         saved = str;

//     if (!saved || *saved == '\0')
//         return NULL;

//     char* start = saved;

//     while (*saved && *saved != delim)
//         saved++;

//     if (*saved == delim) {
//         *saved = '\0';
//         saved++;
//     }

//     return start;
// }

// void shell_help() {
//     printf("Available commands:\r\n");
//     printf("  help          - Show this help\r\n");
//     printf("  ls            - List files\r\n");
//     printf("  cat <file>    - Display file contents\r\n");
//     printf("  echo <text>   - Echo text\r\n");
//     printf("  ps            - List processes\r\n");
//     printf("  fork          - Fork current process\r\n");
//     printf("  uptime        - Show OS uptime\r\n");
//     printf("  meminfo       - Show memory usage\r\n");
//     printf("  clear         - Clear screen\r\n");
//     printf("  exit          - Exit shell\r\n");
// }

// void shell_ls() {
//     char buf[512];
//     int n = fs_list_files(buf, sizeof(buf));
//     if (n <= 0) {
//         printf("(no files)\r\n");
//         return;
//     }
//     printf("%s\r\n", buf);
// }

// void shell_cat(char* filename) {
//     if (!filename) {
//         printf("Usage: cat <filename>\r\n");
//         return;
//     }

//     char buf[512];
//     int n = fs_read_file(filename, buf, sizeof(buf) - 1, 0);

//     if (n < 0) {
//         printf("Error: file not found\r\n");
//         return;
//     }

//     buf[n] = '\0';
//     printf("%s\r\n", buf);
// }

// void shell_ps() {
//     task_t* t = scheduler_get_task_list();

//     printf("PID   STATE   NAME\r\n");
//     printf("--------------------------\r\n");

//     while (t) {
//         printf("%d     %d      %s\r\n", t->pid, t->state, t->name);
//         t = t->next;
//     }
// }

// void shell_start() {
//     printf("RISC-V OS Shell v1.0\r\n");
//     printf("Type 'help' for commands\r\n");

//     while (1) {
//         printf("> ");

//         int pos = 0;

//         while (1) {
//             char c = uart_getchar();

//             if (c == '\r' || c == '\n') {
//                 uart_putchar('\r');
//                 uart_putchar('\n');
//                 input_buf[pos] = 0;
//                 break;
//             }

//             if (pos < INPUT_BUF - 1) {
//                 uart_putchar(c);
//                 input_buf[pos++] = c;
//             }
//         }

//         if (pos == 0)
//             continue;
//         char* redirect_file = NULL;

//         for (int i = 0; i < pos; i++) {
//             if (input_buf[i] == '>') {
//                 input_buf[i] = 0;  // end command text before ">"

//                 // skip spaces
//                 int j = i + 1;
//                 while (input_buf[j] == ' ')
//                     j++;

//                 if (input_buf[j] != 0)
//                     redirect_file = &input_buf[j];

//                 break;
//             }
//         }


//         char* cmd = strtok_simple(input_buf, ' ');
//         char* args = strtok_simple(NULL, ' ');

//         if (!cmd) continue;

//         if (strcmp(cmd, "help") == 0)
//             shell_help();

//         else if (strcmp(cmd, "echo") == 0)
//             printf("%s\r\n", args ? args : "");

//         else if (strcmp(cmd, "ls") == 0)
//             shell_ls();

//         else if (strcmp(cmd, "cat") == 0)
//             shell_cat(args);

//         else if (strcmp(cmd, "ps") == 0)
//             shell_ps();

//         else if (strcmp(cmd, "fork") == 0) {
//             int pid = task_fork();
//             if (pid == 0)
//                 printf("Child process running (PID=%d)\r\n", get_current_task()->pid);
//             else
//                 printf("Forked child process: %d\r\n", pid);
//         }

//         else if (strcmp(cmd, "uptime") == 0) {
//             uint64_t ticks = timer_get_ticks();
//             printf("Uptime: %lu ticks\r\n", ticks);
//         }

//         else if (strcmp(cmd, "meminfo") == 0) {
//             uint64_t mem = mem_get_allocated();
//             printf("Memory allocated: %lu bytes\r\n", mem);
//         }

//         else if (strcmp(cmd, "clear") == 0)
//             printf("\033[2J\033[H");

//         else if (strcmp(cmd, "exit") == 0) {
//             printf("Exiting shell...\r\n");
//             return;

//         }

//         else
//             printf("Unknown command: %s\r\n", cmd);
//     }
// }
#include "shell.h"
#include "uart.h"
#include "string.h"
#include "printf.h"
#include "task.h"
#include "fs.h"
#include "scheduler.h"
#include "timer.h"
#include "memory.h"
#include <stdio.h>
#include "system.h"

#define INPUT_BUF 128
static char input_buf[INPUT_BUF];

/* very simple strtok */
static char* strtok_simple(char *str, char delim) {
    static char* saved;
    if (str)
        saved = str;

    if (!saved || *saved == '\0')
        return NULL;

    char* start = saved;

    while (*saved && *saved != delim)
        saved++;

    if (*saved == delim) {
        *saved = '\0';
        saved++;
    }

    return start;
}

void shell_help() {
    printf("Available commands:\r\n");
    printf("  help          - Show this help\r\n");
    printf("  ls            - List files\r\n");
    printf("  cat <file>    - Display file contents\r\n");
    printf("  echo <text>   - Echo text or redirect: echo hi > x.txt\r\n");
    printf("  ps            - List processes\r\n");
    printf("  fork          - Fork current process\r\n");
    printf("  uptime        - Show OS uptime\r\n");
    printf("  meminfo       - Show memory usage\r\n");
    printf("  clear         - Clear screen\r\n");
    printf("  exit          - Exit shell\r\n");
}

void shell_ls() {
    char buf[512];
    int n = fs_list_files(buf, sizeof(buf));
    if (n <= 0) {
        printf("(no files)\r\n");
        return;
    }
    printf("%s\r\n", buf);
}

void shell_cat(char* filename) {
    if (!filename) {
        printf("Usage: cat <filename>\r\n");
        return;
    }

    char buf[512];
    int n = fs_read_file(filename, buf, sizeof(buf) - 1, 0);

    if (n < 0) {
        printf("Error: file not found\r\n");
        return;
    }

    buf[n] = '\0';
    printf("%s\r\n", buf);
}

void shell_ps() {
    task_t* t = scheduler_get_task_list();

    printf("PID   STATE   NAME\r\n");
    printf("--------------------------\r\n");

    while (t) {
        printf("%d     %d      %s\r\n", t->pid, t->state, t->name);
        t = t->next;
    }
}

void shell_start() {
    printf("RISC-V OS Shell v1.0\r\n");
    printf("Type 'help' for commands\r\n");

    while (1) {
        printf("> ");

        int pos = 0;

        // Read user input
        while (1) {
            char c = uart_getchar();

            if (c == '\r' || c == '\n') {
                uart_putchar('\r');
                uart_putchar('\n');
                input_buf[pos] = 0;
                break;
            }

            if (pos < INPUT_BUF - 1) {
                uart_putchar(c);
                input_buf[pos++] = c;
            }
        }

        if (pos == 0)
            continue;

        // ---------- REDIRECTION PARSING ----------
        char* redirect_file = NULL;

        for (int i = 0; i < pos; i++) {
            if (input_buf[i] == '>') {
                input_buf[i] = 0;  // end command text before ">"

                // skip spaces
                int j = i + 1;
                while (input_buf[j] == ' ')
                    j++;

                if (input_buf[j] != 0)
                    redirect_file = &input_buf[j];

                break;
            }
        }
        // -----------------------------------------

        char* cmd = strtok_simple(input_buf, ' ');
        char* args = strtok_simple(NULL, ' ');

        if (!cmd)
            continue;

        // ----------------- COMMANDS -----------------

        if (strcmp(cmd, "help") == 0)
            shell_help();

        else if (strcmp(cmd, "echo") == 0) {

            if (redirect_file) {
                // Determine length of text to write
                uint32_t data_len = args ? strlen(args) : 0;

                // Create file with the correct size
                fs_create_file(redirect_file, data_len);

                // Write text into the file at offset 0
                if (data_len > 0)
                    fs_write_file(redirect_file, args, data_len, 0);

            } else {
                printf("%s\r\n", args ? args : "");
            }
        }

        else if (strcmp(cmd, "ls") == 0)
            shell_ls();

        else if (strcmp(cmd, "cat") == 0)
            shell_cat(args);

        else if (strcmp(cmd, "ps") == 0)
            shell_ps();

        else if (strcmp(cmd, "fork") == 0) {
            int pid = task_fork();
            if (pid == 0)
                printf("Child process running (PID=%d)\r\n",
                       get_current_task()->pid);
            else
                printf("Forked child process: %d\r\n", pid);
        }

        else if (strcmp(cmd, "uptime") == 0) {
            uint64_t ticks = timer_get_ticks();
            printf("Uptime: %lu ticks\r\n", ticks);
        }

        else if (strcmp(cmd, "meminfo") == 0) {
            uint64_t mem = mem_get_allocated();
            printf("Memory allocated: %lu bytes\r\n", mem);
        }

        else if (strcmp(cmd, "clear") == 0)
            printf("\033[2J\033[H");

        else if (strcmp(cmd, "exit") == 0) {
            printf("Exiting shell...\r\n");
            return;            
        }

        else
            printf("Unknown command: %s\r\n", cmd);
    }
}
