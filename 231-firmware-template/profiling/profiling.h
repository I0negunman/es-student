#pragma once

#include "pico/types.h"
#define STOPWATCH_COUNT 5
// void profiling_init(void);
// void profiling_iteration(void);
// float profiling_avg_us(void);
// uint32_t profiling_max_us(void);
// void profiling_reset_max(void);


typedef struct {
    const char *name;
    uint32_t measurements_count;
    uint32_t measurements_sum;
    uint32_t measurements_max;
    uint32_t start_time;
} profiling_result_t;

void profiling_stopwatch_init(uint32_t id, const char *name);
void profiling_start(uint32_t id);
void profiling_stop(uint32_t id);
bool profiling_get(uint32_t id, profiling_result_t *result);
void profiling_reset(void);