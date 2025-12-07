# # RISC-V kernel entry point
# .section .text.boot
# .global _start
# _start:
#     # Set up stack pointer
#     la sp, _stack_top
    
#     # Clear BSS
#     la t0, _bss_start
#     la t1, _bss_end
# clear_bss:
#     beq t0, t1, done_clear
#     sd zero, 0(t0)
#     addi t0, t0, 8
#     j clear_bss
# done_clear:
    
#     # Jump to C kernel
#     jal kernel_main
    
#     # Should never return, but if it does, loop
# loop:
#     wfi
#     j loop

    .section .text.boot
    .global _start
_start:
    # Set up stack pointer
    la sp, _stack_top

    # Small UART debug helper: write a single char
    # a0 = char to write
    .macro uart_putc reg
        li t2, 0x10000000       # UART0 base for 'virt' machine
        sb \reg, 0(t2)
    .endm

    # indicate boot started
    li a0, 'B'
    uart_putc a0

    # Clear BSS section (clear 8 bytes at a time)
    la t0, _bss_start
    la t1, _bss_end
1:  bgeu t0, t1, 2f          # finish when t0 >= t1
    sd zero, 0(t0)
    addi t0, t0, 8
    j 1b
2:
    # indicate bss cleared
    li a0, 'C'
    uart_putc a0

    # Jump to C kernel (never returns)
    jal kernel_main

3:  # Kernel returned unexpectedly — spin with WFI
    li a0, 'R'               # R = returned unexpectedly
    uart_putc a0
    wfi
    j 3b
