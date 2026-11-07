#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "led.h"
#include "profiling.h"
#include "stdio-text-protocol.h"
#include "api.h"


const uint BLINK_HALF_PERIOD_MS = 500;
uint64_t last_toggle_us = 0;

void blink(void)
{
    uint64_t now_us = time_us_64();

    if (now_us - last_toggle_us >= BLINK_HALF_PERIOD_MS * 1000)
    {
        last_toggle_us = now_us;
        led_toggle();
    }
}


int main()
{
    led_init();

    bool led = false;
    bool previous = false;
    volatile uint32_t *gpio_out_set = (uint32_t *)(SIO_BASE + SIO_GPIO_OUT_SET_OFFSET);
    volatile uint32_t *gpio_out_clr = (uint32_t *)(SIO_BASE + SIO_GPIO_OUT_CLR_OFFSET);

    profiling_init();
    stdio_init_all();

    while (1)
    {
        profiling_iteration();
        blink();
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}
