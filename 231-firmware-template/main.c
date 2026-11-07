#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "profiling.h"
#include "stdio-text-protocol.h"
#include "api.h"
#include "led-task.h"

int main()
{
    profiling_init();
    stdio_init_all();
    led_task_init();

    while (1)
    {
        profiling_iteration();
        led_task_handle();
        const command_t *command = stdio_text_protocol_handle();
        if (command != NULL)
        {
            api_handle(command);
        }
    }
}
