#include "telemetry-task.h"
#include "systime/systime.h"
#include "platform/platform.h"

telemetry_task_callback_t on_record_callback;
telemetry_record_t telemetry_record;
static telemetry_state_t telemetry_state;


static uint64_t half_period_us;
static uint64_t last_telemetry_us;

void telemetry_task_init(telemetry_task_callback_t on_record) {
    on_record_callback = on_record;
    telemetry_state = TELEMETRY_STATE_IDLE;
}

void telemetry_task_handle(void) {
    switch (telemetry_state) {
        case TELEMETRY_STATE_RUN:
            if (systime_period_elapsed(&last_telemetry_us, half_period_us*2)) {
                telemetry_record.time_us = systime_us();
                telemetry_record.voltage_v = platform_voltage_read();
                telemetry_record.temp_c = platform_temp_read();
                on_record_callback(&telemetry_record);
                break;
            }
        case TELEMETRY_STATE_IDLE:
            break;
    }
}

bool telemetry_task_start(uint32_t period_ms) {
    if (period_ms < 10) {
        return false;
    }
    else {
        telemetry_state = TELEMETRY_STATE_RUN;
        half_period_us = (uint64_t)period_ms * 1000 / 2;
        return true;
    }
}

void telemetry_task_stop(void) {
    telemetry_state = TELEMETRY_STATE_IDLE;
}

telemetry_state_t telemetry_task_get_state(void) {
    return telemetry_state;
}

uint32_t telemetry_task_get_period_ms(void) {
    return (uint32_t)half_period_us * 2 / 1000;
}