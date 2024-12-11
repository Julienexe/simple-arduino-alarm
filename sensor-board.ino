#include <Wire.h>

#define SLAVE_ADDRESS 3
#define TRIG_PIN 3
#define ECHO_PIN 2

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendDistance);

  // Set TRIG_PIN as OUTPUT and ECHO_PIN as INPUT using direct register access
  DDRD |= (1 << TRIG_PIN); // Set TRIG_PIN as OUTPUT
  DDRD &= ~(1 << ECHO_PIN); // Set ECHO_PIN as INPUT
}

void loop() {
  delay(100);
}

void sendDistance() {
  long duration = getUltrasonicDistance();
  int distance = duration * 0.034 / 2;
  Wire.write(distance >> 8);
  Wire.write(distance & 0xFF);
}


