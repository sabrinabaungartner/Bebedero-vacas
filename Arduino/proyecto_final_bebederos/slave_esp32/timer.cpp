#include <Arduino.h>
#include "timer.h"
#include "fnqueue.h" 

hw_timer_t * timer = NULL; // To configure the timer
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void (*timer_function)();

void IRAM_ATTR onTimer() { // ISR for the timer interrupt. This ISR will be executed when the timer interrupt occurs.
  fnqueue_add(timer_function); // Add timer function to the queue
}

void timer_init(void (*function)(), volatile uint32_t microseconds) {
  timer = timerBegin(1, 80, true); // Parameters: timer1, prescaler, counter to count up
  timerAttachInterrupt(timer, &onTimer, true); // Attach timer to an ISR. ISR will be executed when the interrupt is generated. ISR function ---> onTimer
  timerAlarmWrite(timer, microseconds, true); // Used to specify the counter value in which the timer interrupt should be generated (is generated every 1 second if microsegundos == 1000000).
  timer_function = function;
}
 
void timer_start() {
  timerAlarmEnable(timer); // Enable de timer interrupt.
}

void timer_stop() {
  timerDetachInterrupt(timer);
  timerAlarmDisable(timer);
}
