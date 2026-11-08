#include <stdio.h>
#include "button-task.h"
#include "pico/stdlib.h"
#include "led-task.h"


static button_state_t button_state;
static const uint BUTTON_PIN = 15;
static uint64_t pressed_time;
static uint64_t unpressed_time;
static uint32_t counter = 0;

void button_task_init() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    button_state = RELEASED;
}

void button_task_handle(void)
{
    bool not_pressed = gpio_get(BUTTON_PIN);
     
    switch (button_state)
    {
    case RELEASED:
        if (!not_pressed) {
            pressed_time = time_us_64();
            button_state = IF_PRESSED;
            return;
        }
        return;
    case IF_PRESSED:
        if (not_pressed) {
            button_state = RELEASED;
            return;
        }
        if (!not_pressed) {
            if ((time_us_64() - pressed_time) > 20000) {
                button_state = PRESSED;
                led_task_next_state();
                counter += 1;
            }
            return;
        }
        return;
    case PRESSED:
        if (not_pressed) {
            unpressed_time = time_us_64();
            button_state = IF_RELEASED;
            return;
        }
        return;
    case IF_RELEASED:
        if (!not_pressed) {
            button_state = PRESSED;
            return;
        }
        if (not_pressed) {
            if ((time_us_64() - unpressed_time) > 20000) {
                button_state = RELEASED;
            }
            return;
        } 
        return;
    // переходы по таблице; при переходе в «нажата»
    // увеличиваем счётчик и вызываем led_task_next_state()
    }
}


void button(void) {
    switch (button_state)
    {
        case RELEASED:
        case IF_PRESSED:
            printf("released, presses %ld\n", counter);
            return;
        case PRESSED:
        case IF_RELEASED:
            printf("pressed, presses %ld\n", counter);
            return;
    }

}

