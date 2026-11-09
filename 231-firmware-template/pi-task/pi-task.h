#pragma once

#include "pico/stdlib.h"

bool pi_task_start(int n);
void pi_task_handle(void);
bool pi_task_get_result(double *result, uint64_t *time);

typedef enum {
    DIDNT_START, 
    CALCULATING,
    READY,
} calculation_state_t;

extern calculation_state_t calculation_state;