/* Linker script for RISC-V kernel */
ENTRY(_start)

SECTIONS
{
    . = 0x80200000; /* QEMU virt machine loads kernel here */
    
    .text : {
        *(.text.boot)
        *(.text .text.*)
    }
    
    .rodata : {
        *(.rodata .rodata.*)
    }
    
    .data : {
        *(.data .data.*)
    }
    
    .bss : {
        _bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        _bss_end = .;
    }
    
    . = ALIGN(4096);
    _stack_bottom = .;
    . += 0x10000;  /* 64KB stack */
    _stack_top = .;
    
    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}

