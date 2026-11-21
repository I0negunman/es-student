#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "led-task/led-task.h"
#include "button-task/button-task.h"
#include "pi-task/pi-task.h"
#include "profiling/profiling.h"  
#include "device/device.h"  
#include "firmware.h"
#include "systime/systime.h"
#include "platform/platform.h"



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


static void print_led_state(void) {
    switch (led_task_get_state()) {
    case LED_STATE_OFF:
        printf("led: off, period %lu ms\n", led_task_get_period_ms());
        break;
    case LED_STATE_ON:
        printf("led: on, period %lu ms\n", led_task_get_period_ms());
        break;
    case LED_STATE_BLINK:
        printf("led: blink, period %lu ms\n", led_task_get_period_ms());
        break;
    }
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

static void cmd_info(const command_t *) {
    device_info_t info;
    snprintf(info.name, sizeof(info.name), "%s", FIRMWARE_NAME);
    device_get_info(&info);
    printf("name: %s\n", info.name);
    printf("version: %s\n", FIRMWARE_VERSION);
    printf("project: %s\n", FIRMWARE_PROJECT);
    printf("repo: %s\n", FIRMWARE_REPO);
    printf("board: %s\n", DEVICE_BOARD);
    printf("serial: %s\n", info.board_id);
    printf("chip: manufacturer 0x%03x, part 0x%04x, revision %u\n", (unsigned)info.manufacturer, (unsigned)info.part, (unsigned)info.revision);
    printf("pico-sdk: %s\n", info.sdk_version);
}

static void cmd_uptime(const command_t *){
   printf("uptime: %llu ms\n", systime_us()/1000);
}

static void cmd_led_enable(const command_t *command) {
    led_task_set_state(LED_STATE_ON);
    print_led_state();
}

static void cmd_led_disable(const command_t *command) {
    led_task_set_state(LED_STATE_OFF);
    print_led_state();
}

static void cmd_profiling(const command_t *command) {
    if (PROFILING_ENABLED) {
        profiling_result_t result;
        for (uint32_t i = 0; i < STOPWATCH_COUNT; i++) {
            if (profiling_get(i, &result)) {
                printf("%-10s count %lu mean %.2f us max %lu us\n", result.name, result.measurements_count,
                    (float)result.measurements_sum / result.measurements_count, result.measurements_max);
            }
        }
    }
    else {
        printf("profiling is disabled in this build\n");
    }
}

static void cmd_profiling_reset(const command_t *command) {
    #if PROFILING_ENABLED
    profiling_reset();
    printf("profiling reset\n");
    #endif
}

static void cmd_pi_start(const command_t *command) {
    uint32_t calc_pi_terms = 1000000;
    if (command->argc > 0)
    {
        if (command->argc != 1 || !parse_u32(command->argv[0], &calc_pi_terms))
        {
            printf("error: usage pi_start [terms]\n");
            return;
        }
        if (calc_pi_terms == 0) {
            printf("error: terms must be greater than 0\n");
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

static void cmd_pi(const command_t *command) {
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

static void cmd_led_blink(const command_t *command) {
    led_task_set_state(LED_STATE_BLINK);
    print_led_state();
}

static void cmd_button(const command_t *command) {
    if (button_task_is_pressed()) {
        printf("button: pressed, presses %ld\n", button_task_get_press_count());
        return;
    }
    printf("button: released, presses %ld\n", button_task_get_press_count());
    return;
}
static void cmd_get_voltage(const command_t *command) {
    printf("voltage: %.03f V\n", platform_voltage_read());
}

const api_command_t api_commands[] = {
    { "info", "device passport", cmd_info },
    { "uptime", "time since reset", cmd_uptime },
    { "led_enable", "turn on LED", cmd_led_enable },
    { "led_disable", "turn off LED", cmd_led_disable },
    { "led_blink", "blink LED", cmd_led_blink },
    { "led_period", "set LED period: led_period <period_ms>", cmd_led_period },
    { "button", "see state of the button and amount of presses", cmd_button},
    { "pi", "see status of pi calculation or its result", cmd_pi},
    { "pi_start", "precision <numbers after the dot>", cmd_pi_start},
    { "profiling", "see profiling results", cmd_profiling},
    { "profiling_reset", "reset profiling results", cmd_profiling_reset},
    { "get_voltage", "get voltage on ACP channel", cmd_get_voltage},
};

const uint64_t api_command_count = sizeof(api_commands) / sizeof(api_commands[0]);