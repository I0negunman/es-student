#include <stdint.h>

void button_task_handle(void);
void button_task_init();
void button(void);
bool button_task_is_pressed(void);
uint32_t button_task_get_press_count(void);

typedef enum {
    PRESSED,
    IF_PRESSED,
    RELEASED,
    IF_RELEASED,
} button_state_t;