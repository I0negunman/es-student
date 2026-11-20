#include "platform/platform.h"
#include "pico/stdlib.h"


uint8_t buttons_pins[BUTTONS_COUNT] = {15, 14};


void platform_init(void) {
    stdio_init_all();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    for (uint8_t number = 0; number < BUTTONS_COUNT; number++) {
        gpio_init(buttons_pins[number]);
        gpio_set_dir(buttons_pins[number], GPIO_IN);
        gpio_pull_up(buttons_pins[number]);
    }
}

void platform_led_set(bool on) {
    gpio_put(PICO_DEFAULT_LED_PIN, on);
}

void platform_led_toggle(void) {
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
}

bool platform_button_read(uint8_t number) {
    return gpio_get(buttons_pins[number]) == 0;
}