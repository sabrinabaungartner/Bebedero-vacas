#include <stdint.h>
#include "package.h"

void packet_to_send(struct packet * pac, uint8_t array[SIZE_ARRAY]) {
  array[0] = START_PACKET;
  array[1] = pac->length;
  array[2] = pac->type_of_message;
  array[3] = pac->payload[0];
  array[4] = END_PACKET;
}

void received_packet(struct packet * pac, uint8_t array[SIZE_ARRAY]) {
  if ((array[0] == START_PACKET) && (array[4] == END_PACKET) && (array[1] == SIZE_ARRAY)) {
    pac->length = array[1];
    pac->type_of_message = array[2];
    pac->payload[0] = array[3];
  }
}