#include <stdint.h>

#include "app_cfg.h"
#include "gd32vw55x_platform.h"
#include "wrapper_os.h"

#include "FreeRTOS.h"
#include "task.h"
#include "gd32vw55x.h"

#define LED_GPIO_PORT GPIOC
#define LED_GPIO_PIN  GPIO_PIN_13
#define LED_GPIO_CLOCK RCU_GPIOC
#define RISCV_EXC_ILLEGAL_INSTRUCTION 2U

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

/*
 * Sustituye la entrada debil exc_entry del MSDK. Esta ruta controlada no usa
 * la pila auxiliar ni el contexto flotante del port: conserva los registros
 * temporales que modifica, recupera una instruccion ilegal comprimida de dos
 * bytes y vuelve directamente a la tarea con mret.
 */
void exc_entry(void) __attribute__((naked, aligned(64), section(".text.trap")));
void exc_entry(void)
{
    __asm volatile (
        "addi sp, sp, -16\n"
        "sw t0, 0(sp)\n"
        "sw t1, 4(sp)\n"
        "sw t2, 8(sp)\n"
        "csrr t0, mcause\n"
        "la t1, g_last_mcause\n"
        "sw t0, 0(t1)\n"
        "andi t1, t0, 0xfff\n"
        "li t2, 2\n"
        "bne t1, t2, 2f\n"
        "la t1, g_test_armed\n"
        "lw t2, 0(t1)\n"
        "beqz t2, 2f\n"
        "csrr t0, mepc\n"
        "la t1, g_last_mepc\n"
        "sw t0, 0(t1)\n"
        "lhu t2, 0(t0)\n"
        "la t1, g_last_instruction\n"
        "sw t2, 0(t1)\n"
        "bnez t2, 2f\n"
        "addi t0, t0, 2\n"
        "csrw mepc, t0\n"
        "la t1, g_instruction_length\n"
        "li t2, 2\n"
        "sw t2, 0(t1)\n"
        "la t1, g_exception_count\n"
        "lw t0, 0(t1)\n"
        "addi t0, t0, 1\n"
        "sw t0, 0(t1)\n"
        "la t1, g_recovery_count\n"
        "lw t0, 0(t1)\n"
        "addi t0, t0, 1\n"
        "sw t0, 0(t1)\n"
        "la t1, g_test_armed\n"
        "sw zero, 0(t1)\n"
        "j 3f\n"
        "2:\n"
        "la t1, g_unexpected_exception\n"
        "li t2, 1\n"
        "sw t2, 0(t1)\n"
        "1: j 1b\n"
        "3:\n"
        "lw t0, 0(sp)\n"
        "lw t1, 4(sp)\n"
        "lw t2, 8(sp)\n"
        "addi sp, sp, 16\n"
        "mret\n"
    );
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
    gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
}

static void led_set(uint8_t on)
{
    if (on != 0U) {
        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
    } else {
        gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
    }
}

static void test_task(void *argument)
{
    (void)argument;

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
    sys_os_init();
    platform_init();
    led_init();

    BaseType_t indicator_ok = xTaskCreate(
        indicator_task, "Indicator", configMINIMAL_STACK_SIZE,
        NULL, tskIDLE_PRIORITY + 1U, &indicator_handle
    );
    BaseType_t test_ok = xTaskCreate(
        test_task, "TrapTest", configMINIMAL_STACK_SIZE,
        NULL, tskIDLE_PRIORITY + 2U, NULL
    );

    if ((indicator_ok != pdPASS) || (test_ok != pdPASS)) {
        for (;;) {
        }
    }

    sys_os_start();
    for (;;) {
    }
}
