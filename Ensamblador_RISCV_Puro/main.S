.section .text
    .align 2

    /* Ejercicio 08: excepcion ilegal y recuperacion en RISC-V puro. */
    .equ RCU_AHB1EN,           0x40023830
    .equ GPIOC_CTL,            0x40020800
    .equ GPIOC_OMODE,          0x40020804
    .equ GPIOC_OSPD,           0x40020808
    .equ GPIOC_PUD,            0x4002080C
    .equ GPIOC_BOP,            0x40020818
    .equ GPIOC_BC,             0x40020828
    .equ GPIOC_CLOCK_BIT,      0x00000004
    .equ PC13_BIT,             0x00002000
    .equ PC13_FIELD_MASK,      0x0C000000
    .equ PC13_OUTPUT_MODE,     0x04000000
    .equ PC13_SPEED_10MHZ,     0x04000000
    .equ SYSTIMER_CMP_LO,      0xD1000008
    .equ SYSTIMER_CMP_HI,      0xD100000C
    .equ SYSTIMER_MTIMECTL,    0xD1000FF8
    .equ MTIMECTL_CMPCLREN,    0x00000002
    .equ ECLIC_INTIP7,         0xD200101C
    .equ ECLIC_INTIE7,         0xD200101D
    .equ ECLIC_INTATTR7,       0xD200101E
    .equ ECLIC_INTCTRL7,       0xD200101F
    .equ ECLIC_POS_EDGE,       0x02
    .equ SYSTIMER_COMPARE_1MS, 40000
    .equ EXC_ILLEGAL,          2
    .equ FRAME_MEPC_OFFSET,    48

    /* API de Firmware/RISCV/env_Eclipse/handlers.c. */
    .extern Exception_Register_EXC

    .global main
    .type main, @function
main:
    /* GPIOC PC13: salida push-pull, LED inicialmente apagado. */
    li      t0, RCU_AHB1EN
    lw      t1, 0(t0)
    ori     t1, t1, GPIOC_CLOCK_BIT
    sw      t1, 0(t0)
    fence   iorw, iorw
    li      t0, GPIOC_CTL
    lw      t1, 0(t0)
    li      t2, PC13_FIELD_MASK
    not     t2, t2
    and     t1, t1, t2
    li      t2, PC13_OUTPUT_MODE
    or      t1, t1, t2
    sw      t1, 0(t0)
    li      t0, GPIOC_OMODE
    lw      t1, 0(t0)
    li      t2, PC13_BIT
    not     t2, t2
    and     t1, t1, t2
    sw      t1, 0(t0)
    li      t0, GPIOC_OSPD
    lw      t1, 0(t0)
    li      t2, PC13_FIELD_MASK
    not     t2, t2
    and     t1, t1, t2
    li      t2, PC13_SPEED_10MHZ
    or      t1, t1, t2
    sw      t1, 0(t0)
    li      t0, GPIOC_PUD
    lw      t1, 0(t0)
    li      t2, PC13_FIELD_MASK
    not     t2, t2
    and     t1, t1, t2
    sw      t1, 0(t0)
    li      t0, GPIOC_BOP
    li      t1, PC13_BIT
    sw      t1, 0(t0)

    /* SysTimer de 1 ms y ECLIC IRQ 7 para el patron observable. */
    li      t0, SYSTIMER_MTIMECTL
    li      t1, MTIMECTL_CMPCLREN
    sw      t1, 0(t0)
    li      t0, SYSTIMER_CMP_LO
    li      t1, -1
    sw      t1, 0(t0)
    li      t0, SYSTIMER_CMP_HI
    sw      zero, 0(t0)
    li      t0, SYSTIMER_CMP_LO
    li      t1, SYSTIMER_COMPARE_1MS
    sw      t1, 0(t0)
    li      t0, ECLIC_INTIP7
    sb      zero, 0(t0)
    li      t0, ECLIC_INTATTR7
    li      t1, ECLIC_POS_EDGE
    sb      t1, 0(t0)
    li      t0, ECLIC_INTCTRL7
    sb      zero, 0(t0)
    li      t0, ECLIC_INTIE7
    li      t1, 1
    sb      t1, 0(t0)
    li      t0, 8
    csrs    mstatus, t0

    /* Registrar handler de causa 2 en la infraestructura oficial. */
    li      a0, EXC_ILLEGAL
    la      a1, illegal_instruction_handler
    call    Exception_Register_EXC

    la      t0, g_test_armed
    li      t1, 1
    sw      t1, 0(t0)

    /* 0x0000 = c.unimp. El handler debe avanzar mepc dos bytes. */
.Lcontrolled_fault:
    .hword  0x0000
.Lafter_fault:
    la      t0, g_test_completed
    li      t1, 1
    sw      t1, 0(t0)
    la      t0, g_systimer_ticks
    lw      t1, 0(t0)
    la      t0, g_last_led_event_ms
    sw      t1, 0(t0)

.Lmain_loop:
    /* Validar todas las condiciones antes de elegir el patron. */
    la      t0, g_test_completed
    lw      t1, 0(t0)
    li      t2, 1
    bne     t1, t2, .Lerror_pattern
    la      t0, g_exception_count
    lw      t1, 0(t0)
    bne     t1, t2, .Lerror_pattern
    la      t0, g_recovery_count
    lw      t1, 0(t0)
    bne     t1, t2, .Lerror_pattern
    la      t0, g_unexpected_exception
    lw      t1, 0(t0)
    bnez    t1, .Lerror_pattern

    /* Patron correcto: seis semiperiodos de 150 ms y pausa de 1200 ms. */
    la      t0, g_systimer_ticks
    lw      t1, 0(t0)
    la      t2, g_last_led_event_ms
    lw      t3, 0(t2)
    sub     t4, t1, t3
    la      t5, g_success_pause
    lw      t6, 0(t5)
    beqz    t6, .Lnormal_interval
    li      a0, 1200
    bltu    t4, a0, .Lbackground
    sw      t1, 0(t2)
    sw      zero, 0(t5)
    la      t0, g_pattern_step
    sw      zero, 0(t0)
    li      t0, GPIOC_BC
    li      t1, PC13_BIT
    sw      t1, 0(t0)
    la      t0, g_led_is_on
    li      t1, 1
    sw      t1, 0(t0)
    j       .Lbackground
.Lnormal_interval:
    li      a0, 150
    bltu    t4, a0, .Lbackground
    sw      t1, 0(t2)
    la      t0, g_led_is_on
    lw      t1, 0(t0)
    xori    t1, t1, 1
    sw      t1, 0(t0)
    beqz    t1, .Lsuccess_off
    li      t0, GPIOC_BC
    li      t2, PC13_BIT
    sw      t2, 0(t0)
    j       .Lstep_advance
.Lsuccess_off:
    li      t0, GPIOC_BOP
    li      t2, PC13_BIT
    sw      t2, 0(t0)
.Lstep_advance:
    la      t0, g_pattern_step
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)
    li      t2, 6
    bltu    t1, t2, .Lbackground
    la      t0, g_success_pause
    li      t1, 1
    sw      t1, 0(t0)
    la      t0, g_heartbeat_cycles
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)
    j       .Lbackground

.Lerror_pattern:
    la      t0, g_systimer_ticks
    lw      t1, 0(t0)
    la      t2, g_last_led_event_ms
    lw      t3, 0(t2)
    sub     t4, t1, t3
    li      t5, 70
    bltu    t4, t5, .Lbackground
    sw      t1, 0(t2)
    la      t0, g_led_is_on
    lw      t1, 0(t0)
    xori    t1, t1, 1
    sw      t1, 0(t0)
    beqz    t1, .Lerror_off
    li      t0, GPIOC_BC
    li      t1, PC13_BIT
    sw      t1, 0(t0)
    j       .Lbackground
.Lerror_off:
    li      t0, GPIOC_BOP
    li      t1, PC13_BIT
    sw      t1, 0(t0)

.Lbackground:
    la      t0, g_background_iterations
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)
    j       .Lmain_loop
    .size main, .-main

    /*
     * Firma del SDK: a0=mcause, a1=puntero al frame guardado por entry.S.
     * El campo mepc ocupa el offset 48 para GD32VW55x SDK V1.6.0.
     */
    .align 2
    .type illegal_instruction_handler, @function
illegal_instruction_handler:
    mv      t6, a1
    la      t0, g_exception_count
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)
    la      t0, g_last_mcause
    sw      a0, 0(t0)
    lw      t1, FRAME_MEPC_OFFSET(t6)
    la      t0, g_last_mepc
    sw      t1, 0(t0)
    csrr    t2, mtval
    la      t0, g_last_mtval
    sw      t2, 0(t0)
    lhu     t2, 0(t1)
    la      t0, g_last_instruction
    sw      t2, 0(t0)
    andi    t3, t2, 3
    li      t4, 3
    beq     t3, t4, .Llength_four
    li      t3, 2
    j       .Llength_store
.Llength_four:
    li      t3, 4
.Llength_store:
    la      t0, g_instruction_length
    sw      t3, 0(t0)

    li      t4, 0x0fff
    and     t4, a0, t4
    li      t5, EXC_ILLEGAL
    bne     t4, t5, .Lunexpected
    la      t0, g_test_armed
    lw      t4, 0(t0)
    li      t5, 1
    bne     t4, t5, .Lunexpected
    bnez    t2, .Lunexpected
    li      t4, 2
    bne     t3, t4, .Lunexpected
    addi    t1, t1, 2
    sw      t1, FRAME_MEPC_OFFSET(t6)
    la      t0, g_recovery_count
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)
    la      t0, g_test_armed
    sw      zero, 0(t0)
    ret
.Lunexpected:
    la      t0, g_unexpected_exception
    li      t1, 1
    sw      t1, 0(t0)
    ret
    .size illegal_instruction_handler, .-illegal_instruction_handler

    .align 2
    .global eclic_mtip_handler
    .type eclic_mtip_handler, @function
eclic_mtip_handler:
    li      t0, ECLIC_INTIP7
    sb      zero, 0(t0)
    la      t0, g_systimer_ticks
    lw      t1, 0(t0)
    addi    t1, t1, 1
    sw      t1, 0(t0)
    ret
    .size eclic_mtip_handler, .-eclic_mtip_handler

.section .bss
    .align 2
    .global g_exception_count
g_exception_count:          .word 0
    .global g_last_mcause
g_last_mcause:              .word 0
    .global g_last_mepc
g_last_mepc:                .word 0
    .global g_last_mtval
g_last_mtval:               .word 0
    .global g_last_instruction
g_last_instruction:         .word 0
    .global g_instruction_length
g_instruction_length:      .word 0
    .global g_recovery_count
g_recovery_count:           .word 0
    .global g_unexpected_exception
g_unexpected_exception:     .word 0
    .global g_test_armed
g_test_armed:               .word 0
    .global g_test_completed
g_test_completed:           .word 0
    .global g_systimer_ticks
g_systimer_ticks:           .word 0
    .global g_last_led_event_ms
g_last_led_event_ms:        .word 0
    .global g_pattern_step
g_pattern_step:             .word 0
    .global g_success_pause
g_success_pause:            .word 0
    .global g_led_is_on
g_led_is_on:                .word 0
    .global g_heartbeat_cycles
g_heartbeat_cycles:         .word 0
    .global g_background_iterations
g_background_iterations:   .word 0
