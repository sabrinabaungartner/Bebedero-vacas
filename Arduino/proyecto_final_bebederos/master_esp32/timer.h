#include <stdint.h>

void timer_init(void (*function)(), volatile uint32_t microseconds);
void timer_start();
void timer_stop();