#include "button-task.h"
#include "systime/systime.h"

#include <stdio.h>

typedef enum {
    PRESSED,
    IF_PRESSED,
    RELEASED,
    IF_RELEASED,
} button_state_t;

button_t buttons[BUTTONS_COUNT] = {
    {"Button 1", NULL, 2, 0, 0, 0, 0},
    {"Button 2", NULL, 2, 0, 0, 0, 0}
};

void button_task_handle(void)
{
    for (uint8_t number = 0; number < BUTTONS_COUNT; number++) {
        button_t *button = &buttons[number];

        button_task_callback_t on_press_callback = button->on_press_callback;
        uint8_t button_pin = button->button_pin;
        bool not_pressed = platform_button_read(button_pin);

        switch (button->button_state)
        {
        case RELEASED:
            if (!not_pressed) {
                button->pressed_time = systime_us();
                button->button_state = IF_PRESSED;
                continue;
            }
            continue;

        case IF_PRESSED:
            if (not_pressed) {
                button->button_state = RELEASED;
                continue;
            }
            if (!not_pressed) {
                if ((systime_us() - button->pressed_time) > 20000) {
                    button->button_state = PRESSED;
                    on_press_callback();
                    button->counter += 1;
                }
                continue;
            }
            continue;

        case PRESSED:
            if (not_pressed) {
                button->unpressed_time = systime_us();
                button->button_state = IF_RELEASED;
                continue;
            }
            continue;

        case IF_RELEASED:
            if (!not_pressed) {
                button->button_state = PRESSED;
                continue;
            }
            if (not_pressed) {
                if ((systime_us() - button->unpressed_time) > 20000) {
                    button->button_state = RELEASED;
                }
                continue;
            }
            continue;
        }
    }
}

bool button_task_is_pressed(void) {
        switch (buttons[0].button_state)
        {
            case RELEASED:
            case IF_PRESSED:
                return 1;
            case PRESSED:
            case IF_RELEASED:
                return 0;
        }
        return 0;
}

uint32_t button_task_get_press_count(void) {
    return buttons[0].counter;
}

void button_task_init(button_task_callback_t on_press, uint8_t number) {
    buttons[number].on_press_callback = on_press;
    buttons[number].button_pin = buttons_pins[number];
}