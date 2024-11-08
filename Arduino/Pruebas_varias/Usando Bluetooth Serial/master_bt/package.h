#define START_PACKET '<'
#define END_PACKET '>'
#define ESCAPE '\\'

#define GET_HELLO_STRING 1
#define REPLY_HELLO_STRING 2
#define MAX_PAYLOAD_SIZE 1 // 1 byte for string "Hello"
#define SIZE_ARRAY 5 // 1 byte for string "hello" + 1 byte for START_PACKET + 1 byte for END_PACKET + 1 byte of type message (GET/REPLY) + 1 byte for MAX_PAYLOAD_SIZE
#define NULO 0

struct packet {
  uint8_t length;
  uint8_t type_of_message;
  uint8_t payload[MAX_PAYLOAD_SIZE];
};

/** The packet must be converted to an array, where each position of the packet is a byte. This allows the array to be sent byte by byte,
indicating the beginning and end of the sent packet*/
void packet_to_send(struct packet * pac, uint8_t array[SIZE_ARRAY]);

/** The received array must be converted to the received packet*/
void received_packet(struct packet * pac, uint8_t array[SIZE_ARRAY]);