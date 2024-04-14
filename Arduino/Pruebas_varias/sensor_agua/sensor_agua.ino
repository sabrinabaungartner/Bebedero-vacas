#define POWER 19
#define SIGNAL 15 //18
//int water_pin = 33;  // Define el número del pin D15 (GPIO15)
//int water_pin = 34;

int value=0;

int level=0;

void setup() {
  Serial.begin(115200);
  pinMode(POWER,OUTPUT);
  digitalWrite(POWER,LOW);
}

void loop() {
  level=waterSensor();
  Serial.print("Water Level:");
  Serial.println(level);
  delay(1000);

}

int waterSensor()
{
  digitalWrite(POWER,HIGH);
  delay(10);
  value=analogRead(SIGNAL);
  delay(10);
  digitalWrite(POWER,LOW);
  return value;
}