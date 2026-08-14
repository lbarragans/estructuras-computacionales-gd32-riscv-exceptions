    .section .text.trigger_illegal_instruction,"ax",@progbits
    .align 2
    .global trigger_illegal_instruction
    .type trigger_illegal_instruction, @function

trigger_illegal_instruction:
    /* 0x0000 es c.unimp: instruccion comprimida reservada e ilegal. */
    .hword 0x0000
    ret

    .size trigger_illegal_instruction, .-trigger_illegal_instruction
