#include <stdint.h>
#include "package.h"

void packet_to_send(struct packet * pac, uint8_t array[SIZE_ARRAY]) {
  array[0] = START_PACKET;
  array[1] = pac->length;
  array[2] = pac->type_of_message;
  array[3] = pac->payload[0];
  array[4] = END_PACKET;
  /*for (int i = 0; i < MAX_PAYLOAD_SIZE; i++) {
    array[i+3] = pac->payload[i];
  }
  array[11] = END_PACKET;*/
}

void received_packet(struct packet * pac, uint8_t array[SIZE_ARRAY]) {
  if ((array[0] == START_PACKET) && (array[4] == END_PACKET) && (array[1] == SIZE_ARRAY)) {
    pac->length = array[1];
    pac->type_of_message = array[2];
    pac->payload[0] = array[3];
  }
  /*if ((arreglo[0] == INICIO) && (arreglo[11] == FIN) && (arreglo[1] == TAMANIO_ARREGLO)) {
    paq->longitud = arreglo[1];
    paq->tipo_mensaje = arreglo[2];
    for (int i = 0; i < MAX_TAMANIO_PAYLOAD; i++) {
      paq->payload[i] = arreglo[i+3];
    }
  }*/
}