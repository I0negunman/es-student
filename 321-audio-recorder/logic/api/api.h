#include "stdio-text-protocol/stdio-text-protocol.h"
#include "telemetry-task/telemetry-task.h"
#pragma once

// обработчик команды: получает имя и аргументы
typedef void (*api_callback_t)(const command_t *command);

// команда прибора: имя, справка для help и обработчик
typedef struct
{
    const char *name;
    const char *help;
    api_callback_t callback;
} api_command_t;

void api_handle(const command_t *command);
void api_print_telemetry(const telemetry_record_t *record);