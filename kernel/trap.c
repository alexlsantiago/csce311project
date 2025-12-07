#include "trap.h"
#include "timer.h"
#include "scheduler.h"
#include "uart.h"
#include "printf.h"

#define SCAUSE_SUPERVISOR_TIMER 0x8000000000000005ULL

/* Set mtvec to our trap vector */
static inline void write_csr_mtvec(uint64_t x) {
    __asm__ volatile("csrw mtvec, %0" :: "r"(x));
}

/* Read scause CSR */
static inline uint64_t read_csr_scause() {
    uint64_t x;
    __asm__ volatile("csrr %0, scause" : "=r"(x));
    return x;
}

/* Read sepc CSR */
static inline uint64_t read_csr_sepc() {
    uint64_t x;
    __asm__ volatile("csrr %0, sepc" : "=r"(x));
    return x;
}

/* Trap handler called by assembly stub */
void trap_handler(uint64_t cause, uint64_t epc) {

    if (cause == SCAUSE_SUPERVISOR_TIMER) {
        timer_tick();          // increment ticks
        scheduler_yield();     // allow multitasking
        return;
    }

    printf("Unhandled trap: cause=%lx epc=%lx\r\n", cause, epc);
}

/* Trap initialization */
void trap_init() {
    extern void trap_vector();

    // Set trap entry point
    write_csr_mtvec((uint64_t)trap_vector);

    printf("[trap] initialized\r\n");
}
