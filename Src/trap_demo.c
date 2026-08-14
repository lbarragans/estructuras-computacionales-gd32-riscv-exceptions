#include <stdint.h>

#include "gd32vw55x.h"
#include "trap_demo.h"

/* API implementada por Firmware/RISCV/env_Eclipse/handlers.c. */
extern void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler);

/*
 * Debe conservar exactamente el orden empleado por entry.S y handlers.c.
 * entry.S restaura mepc desde este marco antes de ejecutar mret.
 */
typedef struct {
    uint32_t ra;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t mcause;
    uint32_t mepc;
    uint32_t msubm;
    uint32_t a6;
    uint32_t a7;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
} exception_frame_t;

volatile uint32_t g_exception_count = 0U;
volatile uint32_t g_last_mcause = 0U;
volatile uint32_t g_last_mepc = 0U;
volatile uint32_t g_last_mtval = 0U;
volatile uint32_t g_last_instruction = 0U;
volatile uint32_t g_instruction_length = 0U;
volatile uint32_t g_recovery_count = 0U;
volatile uint32_t g_unexpected_exception = 0U;
volatile uint32_t g_test_armed = 0U;
volatile uint32_t g_test_completed = 0U;

static uint32_t read_mtval(void)
{
    uint32_t value;
    __asm volatile ("csrr %0, mtval" : "=r" (value));
    return value;
}

static uint32_t instruction_length_at(uint32_t address)
{
    uint16_t first_halfword =
        *(const volatile uint16_t *)(uintptr_t)address;

    /* RISC-V: bits [1:0] != 11 identifican una instruccion de 16 bits. */
    return ((first_halfword & 0x3U) == 0x3U) ? 4U : 2U;
}

static void illegal_instruction_handler(unsigned long mcause, unsigned long sp)
{
    exception_frame_t *frame = (exception_frame_t *)(uintptr_t)sp;
    uint32_t exception_code = (uint32_t)mcause & 0x0FFFU;
    uint32_t instruction_length = instruction_length_at(frame->mepc);

    g_exception_count++;
    g_last_mcause = (uint32_t)mcause;
    g_last_mepc = frame->mepc;
    g_last_mtval = read_mtval();
    g_last_instruction =
        (uint32_t)(*(const volatile uint16_t *)(uintptr_t)frame->mepc);
    g_instruction_length = instruction_length;

    if ((exception_code == RISCV_EXC_ILLEGAL_INSTRUCTION) &&
        (g_test_armed != 0U) &&
        (instruction_length == 2U)) {
        /* Saltar c.unimp; entry.S restaurara este mepc y ejecutara mret. */
        frame->mepc += instruction_length;
        g_recovery_count++;
        g_test_armed = 0U;
    } else {
        g_unexpected_exception = 1U;
    }
}

void trap_demo_init(void)
{
    Exception_Register_EXC(
        RISCV_EXC_ILLEGAL_INSTRUCTION,
        (unsigned long)illegal_instruction_handler
    );
}
