#include <stdint.h>

#include "gd32vw55x.h"
#include "systimer.h"
#include "trap_demo.h"

#define LED_GPIO_PORT          GPIOC
#define LED_GPIO_PIN           GPIO_PIN_13
#define LED_GPIO_CLOCK         RCU_GPIOC

#define SUCCESS_PULSES         3U
#define PULSE_HALF_PERIOD_MS   150U
#define SUCCESS_PAUSE_MS       1200U
#define ERROR_HALF_PERIOD_MS   70U

volatile uint32_t g_heartbeat_cycles = 0U;
volatile uint32_t g_led_toggle_count = 0U;
volatile uint32_t g_background_iterations = 0U;

static uint32_t last_led_event_ms = 0U;
static uint32_t completed_pulses = 0U;
static uint8_t led_is_on = 0U;
static uint8_t success_pause = 0U;

static void led_set(uint8_t turn_on)
{
    led_is_on = (turn_on != 0U) ? 1U : 0U;

    /* En la placa validada PC13 en alto enciende el LED. */
    if (led_is_on != 0U) {
        gpio_bit_set(LED_GPIO_PORT, LED_GPIO_PIN);
    } else {
        gpio_bit_reset(LED_GPIO_PORT, LED_GPIO_PIN);
    }
}

static void led_init(void)
{
    rcu_periph_clock_enable(LED_GPIO_CLOCK);
    gpio_mode_set(
        LED_GPIO_PORT,
        GPIO_MODE_OUTPUT,
        GPIO_PUPD_NONE,
        LED_GPIO_PIN
    );
    gpio_output_options_set(
        LED_GPIO_PORT,
        GPIO_OTYPE_PP,
        GPIO_OSPEED_10MHZ,
        LED_GPIO_PIN
    );
    led_set(0U);
}

static void led_toggle(void)
{
    led_set((uint8_t)(led_is_on == 0U));
    g_led_toggle_count++;
}

static void success_pattern_update(uint32_t now_ms)
{
    uint32_t interval =
        (success_pause != 0U) ? SUCCESS_PAUSE_MS : PULSE_HALF_PERIOD_MS;

    if ((uint32_t)(now_ms - last_led_event_ms) < interval) {
        return;
    }

    last_led_event_ms = now_ms;

    if (success_pause != 0U) {
        success_pause = 0U;
        completed_pulses = 0U;
        led_set(1U);
        g_led_toggle_count++;
        return;
    }

    led_toggle();
    if (led_is_on == 0U) {
        completed_pulses++;
        if (completed_pulses >= SUCCESS_PULSES) {
            success_pause = 1U;
            g_heartbeat_cycles++;
        }
    }
}

static void error_pattern_update(uint32_t now_ms)
{
    if ((uint32_t)(now_ms - last_led_event_ms) >= ERROR_HALF_PERIOD_MS) {
        last_led_event_ms = now_ms;
        led_toggle();
    }
}

int main(void)
{
    led_init();
    systimer_init_1ms();
    trap_demo_init();

    g_test_armed = 1U;
    trigger_illegal_instruction();

    /* Solo se alcanza si el manejador ajusto el mepc correctamente. */
    g_test_completed = 1U;
    last_led_event_ms = systimer_millis();

    while (1) {
        uint32_t now_ms = systimer_millis();

        if ((g_test_completed != 0U) &&
            (g_recovery_count == 1U) &&
            (g_unexpected_exception == 0U)) {
            success_pattern_update(now_ms);
        } else {
            error_pattern_update(now_ms);
        }

        g_background_iterations++;
    }
}
