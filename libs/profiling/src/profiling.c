#include "profiling/profiling.h"

#if PROFILING_ENABLED

#include <stddef.h>
#include "systime/systime.h"

static profiling_result_t stopwatches[STOPWATCH_COUNT];

void profiling_stopwatch_init(uint32_t id, const char *name) {
    stopwatches[id].name = name;
    stopwatches[id].measurements_count = 0;
    stopwatches[id].measurements_sum = 0;
    stopwatches[id].measurements_max = 0;
    stopwatches[id].start_time = 0;
}

void profiling_start(uint32_t id) {
    stopwatches[id].start_time = systime_us();
}

void profiling_stop(uint32_t id) {
    uint32_t now = systime_us();
    uint32_t elapsed = now - stopwatches[id].start_time;

    stopwatches[id].measurements_count++;
    stopwatches[id].measurements_sum += elapsed;

    if (elapsed > stopwatches[id].measurements_max) {
        stopwatches[id].measurements_max = elapsed;
    }
}

bool profiling_get(uint32_t id, profiling_result_t *result) {
    if (stopwatches[id].name == NULL) {
        return false;
    }

    *result = stopwatches[id];
    return true;
}

void profiling_reset(void) {
    for (uint32_t i = 0; i < STOPWATCH_COUNT; i++) {
        stopwatches[i].measurements_count = 0;
        stopwatches[i].measurements_sum = 0;
        stopwatches[i].measurements_max = 0;
    }
}

#endif
