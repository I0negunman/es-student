#include "platform/platform.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"

// вход АЦП: вывод GP26, канал 0
#define ADC_PIN 26
#define ADC_CHANNEL 0
#define ADC_TEMP_CHANNEL 4
#

// опорное напряжение АЦП на плате Pico и число уровней 12-разрядного кода
#define ADC_VREF_V 3.3f
#define ADC_LEVELS 4096

typedef enum {
    ADC_FREE,
    ADC_CAPTURE,
} adc_user_t;

static const uint BUTTON_PIN = 15;
static volatile adc_user_t adc_user;

static repeating_timer_t blink_timer;
static bool blink_running;
static volatile bool blink_on;
static volatile uint32_t blink_toggles;

static repeating_timer_t capture_timer;
static uint16_t capture_samples[PLATFORM_CAPTURE_MAX];
static uint32_t capture_times[PLATFORM_CAPTURE_MAX];
static uint32_t capture_count;
static uint32_t capture_period_us;
static volatile uint32_t capture_index;
static volatile uint32_t capture_handler_us;




void platform_init(void) {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_set_temp_sensor_enabled(true);
}

void platform_led_set(bool on) {
    gpio_put(PICO_DEFAULT_LED_PIN, on);
}

void platform_led_toggle(void) {
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

bool platform_button_read(void) {
    return gpio_get(BUTTON_PIN) == 0;
}

float platform_voltage_read(void)
{
    adc_select_input(ADC_CHANNEL);
    return adc_read() * ADC_VREF_V / ADC_LEVELS;
}


float platform_temp_read(void) {
    adc_select_input(ADC_TEMP_CHANNEL);
    return (-(adc_read() * ADC_VREF_V / ADC_LEVELS - 0.706) / 0.001721 + 27);  
}

static bool on_blink(repeating_timer_t *timer)
{
    blink_on = !blink_on;
    gpio_put(PICO_DEFAULT_LED_PIN, blink_on);
    blink_toggles++;

    // true — вызвать снова через тот же период
    return true;
}

bool platform_blink_start(uint32_t period_ms)
{
    if (period_ms < 10 || period_ms > 60000)
    {
        return false;
    }

    platform_blink_stop();
    blink_toggles = 0;

    // минус перед периодом — отсчитывать от начала прошлого вызова
    blink_running = add_repeating_timer_ms(-(int32_t)period_ms, on_blink, NULL, &blink_timer);
    return blink_running;
}

uint32_t platform_blink_stop(void)
{
    if (blink_running)
    {
        cancel_repeating_timer(&blink_timer);
        blink_running = false;
    }

    blink_on = false;
    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    return blink_toggles;
}



// обработчик будильника захвата: время и отсчёт, затем время своей работы
static bool on_capture(repeating_timer_t *timer)
{
    uint32_t start_us = time_us_32();
    capture_times[capture_index] = start_us;
    capture_samples[capture_index] = adc_read();
    capture_index++;
    capture_handler_us += time_us_32() - start_us;

    if (capture_index < capture_count)
    {
        return true;
    }

    // захват окончен: АЦП свободен, будильник снимается
    adc_user = ADC_FREE;
    return false;
}

bool platform_capture_start(uint32_t count, uint32_t period_us)
{
    if (adc_user != ADC_FREE || count == 0 || count > PLATFORM_CAPTURE_MAX || period_us == 0)
    {
        return false;
    }

    adc_select_input(ADC_CHANNEL);
    capture_count = count;
    capture_period_us = period_us;
    capture_index = 0;
    capture_handler_us = 0;
    adc_user = ADC_CAPTURE;

    if (!add_repeating_timer_us(-(int64_t)period_us, on_capture, NULL, &capture_timer))
    {
        adc_user = ADC_FREE;
        return false;
    }
    return true;
}

bool platform_adc_busy(void) {
    return (adc_user != ADC_FREE);
}

bool platform_capture_done(void) {
    if (adc_user == ADC_FREE && capture_index == capture_count) {
        return true;
    }
    return false;
}

void platform_capture_get(platform_capture_t *capture) {
    uint32_t min_period = capture_times[1] - capture_times[0];
    uint32_t max_period = capture_times[1] - capture_times[0];
    capture->samples = capture_samples;
    capture->count = capture_count;
    capture->period_us = capture_period_us;
    
    for (int i = 0; i < capture_count - 1; i++) {
        uint32_t difference = capture_times[i + 1] - capture_times[i];
        if (difference < min_period) {
            min_period = difference;
        }
        if (difference > max_period) {
            max_period = difference;
        }
    }

    capture->min_interval_us = min_period;
    capture->max_interval_us = max_period;
    capture->handler_percent = 100.0f * capture_handler_us / (capture_times[capture_count - 1] - capture_times[0]);
}