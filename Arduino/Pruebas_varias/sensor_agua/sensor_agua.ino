#define POWER 19
#define SIGNAL 33

int value = 0;
int level = 0;

void setup() {
  Serial.begin(115200);
  pinMode(POWER, OUTPUT);
  pinMode(SIGNAL, INPUT);
  digitalWrite(POWER, HIGH); // Encender el sensor
}

void loop() {
  uint8_t* dataBytes = waterSensor(); // Obtener los bytes de datos desde el sensor
  int reconstructedValue = bytesToInt(dataBytes); // Reconstruir el valor int desde los bytes
  Serial.print("Reconstructed Value: ");
  Serial.println(reconstructedValue);
  delay(1000);
}

uint8_t* waterSensor() {
  value = analogRead(SIGNAL); // Leer valor analógico del pin SIGNAL (GPIO15)
  Serial.print("value antes de convertir: ");
  Serial.println(value);

  // Convertir el valor int a un arreglo de bytes (uint8_t[])
  static uint8_t bytes_to_send[sizeof(int)];
  for (size_t i = 0; i < sizeof(int); i++) {
    bytes_to_send[i] = (value >> (i * 8)) & 0xFF; // Obtener el byte i-ésimo del valor int
  }

  return bytes_to_send; // Devolver el arreglo de bytes (uint8_t*)
}

int bytesToInt(uint8_t* bytes) {
  int reconstructedValue = 0;
  for (size_t i = 0; i < sizeof(int); i++) {
    reconstructedValue |= (bytes[i] << (i * 8)); // Reconstruir el valor int a partir de los bytes
  }
  return reconstructedValue;
}