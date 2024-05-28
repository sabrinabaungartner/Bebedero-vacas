#define START_PACKET '<'
#define END_PACKET '>'
#define ESCAPE '\'

#define GET_WATER_LEVEL_AND_TEMPERATURE 1
#define REPLY_WATER_LEVEL_AND_TEMPERATURE 2
#define TURN_ON_PUMP 3
#define TURN_OFF_PUMP 4
#define REPLY_TURN_ON_PUMP 5
#define REPLY_TURN_OFF_PUMP 6
#define GET_WATER_LEVEL 7
#define REPLY_WATER_LEVEL 8

#define MAX_PAYLOAD_SIZE 7 // 4 bytes for water_level (int value), 2 bytes for water_temperature (integer and decimal part) and 1 byte for success or failure in pump actions
#define SIZE_ARRAY 11 // 1 byte for lenght + 1 byte for START_PACKET + 1 byte for END_PACKET + 1 byte of type message (GET/REPLY/FILL_WATERER) + 7 bytes for MAX_PAYLOAD_SIZE
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