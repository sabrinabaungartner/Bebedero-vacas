#include <stdint.h>
#include "Arduino.h"


static volatile int anidamiento;

void critical_begin(void) {
  cli();
}

void critical_end(void) {
  sei();
}
