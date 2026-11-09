#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "api.h"
#include "log.h"
#include "device.h"
#include "command.h"
#include "profiling.h"
#include "led-task.h"
#include "button-task.h"
#include "pi-task.h"


static bool parse_u32(const char *text, uint32_t *value)
{
    // strtoul приняла бы и знак: из "-1" получилось бы 4294967295
    if (!isdigit((unsigned char)text[0]))
    {
        return false;
    }

    char *end;
    errno = 0;
    unsigned long number = strtoul(text, &end, 10);

    // после числа остались символы или число не поместилось
    if (*end != '\0' || errno == ERANGE || number > UINT32_MAX)
    {
        return false;
    }

    *value = (uint32_t)number;
    return true;
}

void cmd_info(const command_t *) {
    device_info();
}

void cmd_uptime(const command_t *){
   printf("uptime: %llu ms\n", time_us_64()/1000);
}

void cmd_main_time_exec(const command_t *) {
   printf("iteration avg %.2f us, max %u us\n", profiling_avg_us(), (unsigned)profiling_max_us());
}

void cmd_main_time_reset(const command_t *) {
   profiling_reset_max();
   printf("max reset\n");
}

void cmd_led_enable(const command_t *command) {
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

void cmd_led_disable(const command_t *command) {
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

void cmd_pi_start(const command_t *command) {
    uint32_t calc_pi_terms = 1000000;
    if (command->argc > 0)
    {
        if (command->argc != 1 || !parse_u32(command->argv[0], &calc_pi_terms))
        {
            printf("error: usage pi_start [terms]\n");
            return;
        }
    }
    if (!pi_task_start(calc_pi_terms)) {
        printf("error: pi is busy\n");
    }
    else {
        printf("pi: started, %ld terms\n", calc_pi_terms);
    }
}

void cmd_pi(const command_t *command) {
    double pi_result;
    uint64_t pi_time;
    switch (calculation_state) {
    case DIDNT_START:
        printf("pi: not started\n");
        break;
    case CALCULATING:
        printf("pi: running\n");
        break;
    case READY:
        pi_task_get_result(&pi_result, &pi_time);
        // Первое переключение — на ближайшей итерации
        printf("pi: %f in %lld ms\n", pi_result, pi_time / 1000);
        break;
    }   
}

void cmd_led_blink(const command_t *command) {
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

void cmd_button(const command_t *command) {
    if (button_task_is_pressed()) {
        printf("button: pressed, pressed %ld\n", button_task_get_press_count());
        return;
    }
    printf("button: released, pressed %ld\n", button_task_get_press_count());
    return;
}



static void cmd_led_period(const command_t *command)
{
    uint32_t period_ms;

    if (command->argc != 1 || !parse_u32(command->argv[0], &period_ms))
    {
        printf("error: usage led_period <period_ms>\n");
        return;
    }

    // годится ли такой период, решает задача светодиода
    if (!led_task_set_period_ms(period_ms))
    {
        printf("error: period_ms must be greater than 0\n");
        return;
    }

    // ответ: состояние и период светодиода
    print_led_state();
}

const api_command_t api_commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime },
    { "main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec },
    { "main_time_reset", "reset maximum iteration", cmd_main_time_reset },
    { "led_enable", "turn on LED", cmd_led_enable },
    { "led_disable", "turn off LED", cmd_led_disable },
    { "led_blink", "blink LED", cmd_led_blink },
    { "led_period", "set LED period: led_period <period_ms>", cmd_led_period },
    { "button", "see state of the button and amount of presses", cmd_button},
    { "pi", "see status of pi calculation or its result", cmd_pi},
    { "pi_start", "precision <numbers after the dot>", cmd_pi_start},
};

const uint api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);

void api_handle(const command_t *command)
{
    if (command->truncated) {
        printf("error: command longer than 63 characters or 4 arguments\n");
        return;
    }

    if (strcmp(command->name, "help") == 0) {
        printf("help             list of commands\n");
        for (uint32_t i = 0; i < api_command_count; i++) 
        {
            printf("%-16s %s\n", api_commands[i].name, api_commands[i].help);
        }
        return;
    }

    for (uint32_t i = 0; i < api_command_count; i++)
    {
        if (strcmp(command->name, api_commands[i].name) == 0)
        {
            api_commands[i].callback(command);
            return;
        }
    }
    printf("error: unknown command '%s', try help\n", command->name);
}

