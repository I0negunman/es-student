#pragma once

#include <stdint.h>
#include "platform/platform.h"

void button_task_handle(void);
//void button(void);
bool button_task_is_pressed(void);
uint32_t button_task_get_press_count(void);

typedef void (*button_task_callback_t)(void);
void button_task_init(button_task_callback_t on_press, uint8_t number);

typedef struct {
    const char *name;
    button_task_callback_t on_press_callback;
    uint8_t button_state;
    uint8_t button_pin;
    uint64_t pressed_time;
    uint64_t unpressed_time;
    uint32_t counter;
} button_t;

extern button_t buttons[BUTTONS_COUNT];