#pragma once

#include <stdbool.h>
#include <stdint.h>

#define BUTTONS_COUNT 2

// запускает стандартный ввод-вывод и настраивает выводы
void platform_init(void);

void platform_led_set(bool on);


// нажата ли кнопка сейчас, без подавления дребезга
bool platform_button_read(uint8_t number);

extern uint8_t buttons_pins[];
extern uint8_t buttons_count;

void platform_led_toggle(void);