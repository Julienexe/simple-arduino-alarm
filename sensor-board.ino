#include <Wire.h>

#define SLAVE_ADDRESS 3
#define TRIG_PIN 3
#define ECHO_PIN 2

void setup() {
  Wire.begin(SLAVE_ADDRESS);
  Wire.onRequest(sendDistance);

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

void loop() {
  delay(100);
}

void sendDistance() {
  long duration = getUltrasonicDistance();
  int distance = duration * 0.034 / 2;
  Wire.write(distance >> 8);
  Wire.write(distance & 0xFF);
}

long getUltrasonicDistance() {
  PORTD &= ~(1 << TRIG_PIN); 
  delayMicroseconds(2);
  PORTD |= (1 << TRIG_PIN); 
  delayMicroseconds(10);
  PORTD &= ~(1 << TRIG_PIN); 

  return pulseIn(ECHO_PIN, HIGH);
}

