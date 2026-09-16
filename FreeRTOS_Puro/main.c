#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "gd32vw55x.h"

#define LED_GPIO_PORT GPIOC
#define LED_GPIO_PIN  GPIO_PIN_13
#define LED_GPIO_CLOCK RCU_GPIOC
#define RISCV_EXC_ILLEGAL_INSTRUCTION 2U

extern void Exception_Register_EXC(uint32_t EXCn, unsigned long exc_handler);

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

static TaskHandle_t indicator_handle;

static uint32_t read_mtval(void)
{
    uint32_t value;
    __asm volatile ("csrr %0, mtval" : "=r" (value));
    return value;
}

static void illegal_instruction_handler(unsigned long mcause, unsigned long sp)
{
    exception_frame_t *frame = (exception_frame_t *)(uintptr_t)sp;
    const uint16_t instruction =
        *(const volatile uint16_t *)(uintptr_t)frame->mepc;
    const uint32_t length = ((instruction & 0x3U) == 0x3U) ? 4U : 2U;
    const uint32_t code = (uint32_t)mcause & 0x0FFFU;

    g_exception_count++;
    g_last_mcause = (uint32_t)mcause;
    g_last_mepc = frame->mepc;
    g_last_mtval = read_mtval();
    g_last_instruction = instruction;
    g_instruction_length = length;

    if ((code == RISCV_EXC_ILLEGAL_INSTRUCTION) &&
        (g_test_armed != 0U) && (instruction == 0U) && (length == 2U)) {
        frame->mepc += 2U;
        g_recovery_count++;
        g_test_armed = 0U;
    } else {
        g_unexpected_exception = 1U;
    }
}

static void trigger_illegal_instruction(void)
{
    __asm volatile (".hword 0x0000");
}

static void led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLOCK);
    gpio_mode_set(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                  LED_GPIO_PIN);
    gpio_output_options_set(LED_GPIO_PORT, GPIO_OTYPE_PP,
                            GPIO_OSPEED_10MHZ, LED_GPIO_PIN);
    gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
}

static void led_set(uint8_t on)
{
    if (on != 0U) {
        gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
    } else {
        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
    }
}

static void test_task(void *argument)
{
    (void)argument;

    Exception_Register_EXC(
        RISCV_EXC_ILLEGAL_INSTRUCTION,
        (unsigned long)illegal_instruction_handler
    );

    g_test_armed = 1U;
    trigger_illegal_instruction();
    g_test_completed = 1U;

    xTaskNotifyGive(indicator_handle);
    vTaskDelete(NULL);
}

static void indicator_task(void *argument)
{
    uint8_t success;
    (void)argument;

    (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    success = (uint8_t)((g_test_completed != 0U) &&
                        (g_exception_count == 1U) &&
                        (g_recovery_count == 1U) &&
                        (g_unexpected_exception == 0U));

    for (;;) {
        if (success != 0U) {
            uint32_t pulse;
            for (pulse = 0U; pulse < 3U; pulse++) {
                led_set(1U);
                vTaskDelay(pdMS_TO_TICKS(150U));
                led_set(0U);
                vTaskDelay(pdMS_TO_TICKS(150U));
            }
            vTaskDelay(pdMS_TO_TICKS(1200U));
        } else {
            led_set(1U);
            vTaskDelay(pdMS_TO_TICKS(70U));
            led_set(0U);
            vTaskDelay(pdMS_TO_TICKS(70U));
        }
    }
}

int main(void)
{
    led_init();

    (void)xTaskCreate(indicator_task, "Indicator", configMINIMAL_STACK_SIZE,
                      NULL, tskIDLE_PRIORITY + 1U, &indicator_handle);
    (void)xTaskCreate(test_task, "TrapTest", configMINIMAL_STACK_SIZE,
                      NULL, tskIDLE_PRIORITY + 2U, NULL);

    vTaskStartScheduler();
    for (;;) {
    }
}
