    .section .text
    .global trap_vector

trap_vector:
    addi sp, sp, -16      # make space on stack
    sd ra, 8(sp)
    sd a0, 0(sp)

    # Get scause and sepc
    csrr a0, scause
    csrr a1, sepc

    # Call C trap handler
    call trap_handler

    ld ra, 8(sp)
    ld a0, 0(sp)
    addi sp, sp, 16

    sret
