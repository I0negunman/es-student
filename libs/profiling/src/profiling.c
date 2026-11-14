#include "profiling/profiling.h"
#include <stddef.h>
#include "systime/systime.h"


// итерация около 2 мкс, за секунду их около 500 000;
// ближайшая степень двойки 2^19 = 524 288, среднее помнит около секунды
#define AVG_SHIFT 19
// замер в 1/256 долях микросекунды, чтобы среднее не теряло дробную часть
#define FRACTION_SHIFT 8

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


#if PROFILING_ENABLED
void profiling_stop(uint32_t id) {
    uint32_t now = systime_us();
    uint32_t elapsed = now - stopwatches[id].start_time;
    stopwatches[id].measurements_count++;
    stopwatches[id].measurements_sum += elapsed;
    if (elapsed > stopwatches[id].measurements_max) {
        stopwatches[id].measurements_max = elapsed;
    }
}

#else
// профилирование выключено: вызовы ничего не делают
#define profiling_start(id) ((void)0)
#define profiling_stop(id) ((void)0)
#endif


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
