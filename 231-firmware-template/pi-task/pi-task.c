#include "pi-task.h"

#define PI_TERMS_PER_CALL 100


// Состояние расчёта: должно пережить возврат из обработчика
static uint32_t pi_k;
static uint32_t pi_n;
static double pi_sum;
static double pi_sign;
static double pi_result;
static uint32_t time_start;
static uint32_t time_finish;
calculation_state_t calculation_state = DIDNT_START;

bool pi_task_start(int n)
{
    if (calculation_state != CALCULATING) {
        pi_n = n;
        pi_k = 0;
        pi_sum = 0.0;
        pi_sign = 1.0;
        calculation_state = CALCULATING;
        time_start = time_us_64();
        return true;
    }
    else {
        return false;
    }
}

void pi_task_handle(void)
{
    if (calculation_state != CALCULATING) {
        return;

    }

    // Очередная порция членов ряда, но не дальше последнего
    int end = pi_k + PI_TERMS_PER_CALL;
    if (end > pi_n) {
        end = pi_n;
    }
    for (; pi_k < end; pi_k++) {
        pi_sum += pi_sign / (2.0 * pi_k + 1.0);
        pi_sign = -pi_sign;
    }

    if (pi_k == pi_n) {
        pi_result = 4.0 * pi_sum;
        calculation_state = READY;
        time_finish = time_us_64();
    }
}

// Готов ли результат: если да, записать его по указателю
bool pi_task_get_result(double *result, uint64_t *time)
{
    if (calculation_state != READY) {
        return false;
    }
    *result = pi_result;
    *time = time_finish - time_start;
    return true;
}