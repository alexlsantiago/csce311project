#include "kernel.h"
#include "task.h"
#include "fs.h"
#include "uart.h"
#include "types.h"

/* System call handler */
uint64_t syscall_handler(uint64_t syscall_num,
                         uint64_t arg1,
                         uint64_t arg2,
                         uint64_t arg3) {
    switch (syscall_num) {
        case SYS_EXIT:
            task_exit((int)arg1);
            return 0;

        case SYS_WRITE: {
            int fd = (int)arg1;
            const char* buf = (const char*)arg2;
            size_t count = (size_t)arg3;

            if (fd == 1 || fd == 2) {  /* stdout/stderr */
                for (size_t i = 0; i < count; i++) {
                    uart_putchar(buf[i]);
                }
                return count;
            }
            return (uint64_t)-1;
        }

        case SYS_READ: {
            int fd = (int)arg1;
            char* buf = (char*)arg2;
            size_t count = (size_t)arg3;

            if (fd == 0) {  /* stdin */
                for (size_t i = 0; i < count; i++) {
                    buf[i] = uart_getchar();
                }
                return count;
            }
            return (uint64_t)-1;
        }

        case SYS_FORK:
            return (uint64_t)task_fork();

        case SYS_EXEC: {
            const char* path = (const char*)arg1;
            char** argv = (char**)arg2;
            return (uint64_t)task_exec(path, argv);
        }

        case SYS_WAIT: {
            int pid = (int)arg1;
            return (uint64_t)task_wait(pid);
        }

        case SYS_OPEN:
        case SYS_CLOSE:
            /* File descriptor ops not implemented yet */
            return 0;

        case SYS_READ_FS: {
            const char* path = (const char*)arg1;
            void* buf = (void*)arg2;
            uint32_t size = (uint32_t)arg3;
            /* For now, always read from offset 0 */
            return (uint64_t)fs_read_file(path, buf, size, 0);
        }

        case SYS_WRITE_FS: {
            const char* path = (const char*)arg1;
            const void* buf = (const void*)arg2;
            uint32_t size = (uint32_t)arg3;
            /* For now, always write from offset 0 */
            return (uint64_t)fs_write_file(path, buf, size, 0);
        }

        default:
            return (uint64_t)-1;
    }
}
