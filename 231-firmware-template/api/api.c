#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "api.h"
#include "log.h"
#include "led.h"
#include "device.h"
#include "command.h"
#include "profiling.h"

volatile double pi_result;

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


double calc_pi(int n)
{
    double sum = 0.0;
    double sign = 1.0;

    for (int k = 0; k < n; k++) {
        // Прибавить очередной член ряда: ±1 / (2k + 1)
        sum += sign / (2.0 * k + 1.0);
        sign = -sign;
    }

    // Сумма ряда равна π/4
    return 4.0 * sum;
}

void cmd_calc_pi(const command_t *command)
{
    uint32_t calc_pi_terms = 1000000;
    uint64_t start_us = time_us_64();
    if (command->argc > 0)
    {
        if (command->argc != 1 || !parse_u32(command->argv[0], &calc_pi_terms))
        {
            printf("error: usage calc_pi [terms]\n");
            return;
        }
    }
    pi_result = calc_pi(calc_pi_terms);
    uint64_t spent_us = time_us_64() - start_us;

    printf("pi: %.8f\n", pi_result);
    printf("time: %llu ms\n", spent_us / 1000);
}

const api_command_t api_commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime },
    { "calc_pi", "calculate pi: calc_pi [terms]", cmd_calc_pi },
    { "main_time_exec", "superloop iteration: average and maximum", cmd_main_time_exec },
    { "main_time_reset", "reset maximum iteration", cmd_main_time_reset },
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

