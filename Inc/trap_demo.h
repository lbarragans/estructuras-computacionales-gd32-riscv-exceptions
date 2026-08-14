#ifndef TRAP_DEMO_H
#define TRAP_DEMO_H

#include <stdint.h>

#define RISCV_EXC_ILLEGAL_INSTRUCTION 2U

extern volatile uint32_t g_exception_count;
extern volatile uint32_t g_last_mcause;
extern volatile uint32_t g_last_mepc;
extern volatile uint32_t g_last_mtval;
extern volatile uint32_t g_last_instruction;
extern volatile uint32_t g_instruction_length;
extern volatile uint32_t g_recovery_count;
extern volatile uint32_t g_unexpected_exception;
extern volatile uint32_t g_test_armed;
extern volatile uint32_t g_test_completed;

void trap_demo_init(void);
void trigger_illegal_instruction(void);

#endif /* TRAP_DEMO_H */
