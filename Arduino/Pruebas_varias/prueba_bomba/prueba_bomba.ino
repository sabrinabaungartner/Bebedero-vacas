// Pin de control del relé
//const int pinRelay = 5;
const int pinRelay = 21;

// Tiempo de funcionamiento de la bomba en milisegundos (ejemplo: 10 segundos)
const unsigned long tiempoFuncionamientoBomba = 10000; // 10 segundos en milisegundos

void encenderBomba() {
  // Activar el relé (dependiendo del tipo de relé)
  digitalWrite(pinRelay, LOW);
}

void apagarBomba() {
  // Desactivar el relé (dependiendo del tipo de relé)
  digitalWrite(pinRelay, HIGH);
}

void funcion_prender_apagar_bomba() {
  
  // Activar la bomba
  encenderBomba();
  delay(tiempoFuncionamientoBomba); // Esperar el tiempo de funcionamiento de la bomba

  // Apagar la bomba
  apagarBomba();

  // El programa termina aquí, ya que no hay bucle loop()
}

void setup() {
  // Configurar el pin del relé como salida
  pinMode(pinRelay, OUTPUT);
  funcion_prender_apagar_bomba();
  //delay(1000);
}

void loop() {
  //funcion_prender_apagar_bomba();
  //delay(15000);
}
