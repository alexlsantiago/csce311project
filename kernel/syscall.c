#include "kernel.h"
#include "task.h"
#include "fs.h"
#include "uart.h"
#include "types.h"

/* System call handler */
uint64_t syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
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
            return -1;
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
            return -1;
        }
        
        case SYS_FORK:
            return task_fork();
            
        case SYS_EXEC: {
            const char* path = (const char*)arg1;
            char** argv = (char**)arg2;
            return task_exec(path, argv);
        }
        
        case SYS_WAIT: {
            int pid = (int)arg1;
            return task_wait(pid);
        }
        
        case SYS_OPEN:
        case SYS_CLOSE:
            /* File operations - simplified */
            return 0;
            
        case SYS_READ_FS: {
            const char* path = (const char*)arg1;
            void* buf = (void*)arg2;
            uint32_t size = (uint32_t)arg3;
            uint32_t offset = (uint32_t)arg4;
            return fs_read_file(path, buf, size, offset);
        }
        
        case SYS_WRITE_FS: {
            const char* path = (const char*)arg1;
            const void* buf = (const void*)arg2;
            uint32_t size = (uint32_t)arg3;
            uint32_t offset = (uint32_t)arg4;
            return fs_write_file(path, buf, size, offset);
        }
        
        default:
            return -1;
    }
}

