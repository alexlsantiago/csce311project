#include <stdint.h>
#include <stdio.h>

#define SBI_EXT_SRST 0x53525354  // System Reset extension
#define SBI_SRST_RESET 0x0
#define SBI_SRST_SHUTDOWN 0x0

static inline void sbi_shutdown() {
    register uint64_t a7 asm("a7") = SBI_EXT_SRST;
    register uint64_t a0 asm("a0") = SBI_SRST_SHUTDOWN;
    register uint64_t a1 asm("a1") = SBI_SRST_RESET;
    asm volatile("ecall"
                 :
                 : "r"(a0), "r"(a1), "r"(a7)
                 : "memory");
}

void os_exit() {
    printf("OS shutting down...\r\n");
    sbi_shutdown();

    // fallback infinite loop
    while(1) {
        asm volatile("wfi");
    }
}
