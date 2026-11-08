void button_task_handle(void);
void button_task_init();
void button(void);

typedef enum {
    PRESSED,
    IF_PRESSED,
    RELEASED,
    IF_RELEASED,
} button_state_t;