# RISC-V kernel entry point
.section .text.boot
.global _start
_start:
    # Set up stack pointer
    la sp, _stack_top
    
    # Clear BSS
    la t0, _bss_start
    la t1, _bss_end
clear_bss:
    beq t0, t1, done_clear
    sd zero, 0(t0)
    addi t0, t0, 8
    j clear_bss
done_clear:
    
    # Jump to C kernel
    call kernel_main
    
    # Should never return, but if it does, loop
loop:
    wfi
    j loop

