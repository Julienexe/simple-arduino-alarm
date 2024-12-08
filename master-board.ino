#include <Wire.h>

#define SLAVE_POT 2
#define SLAVE_ULTRA 3
#define SLAVE_LCD 4
#define BUZZER_PIN 4
volatile uint8_t *ddrB = &DDRB;
volatile uint8_t *portB = &PORTB;

int thresholdDistance = 0;
int measuredDistance = 0;

void setup() {
    Wire.begin(); // Start as master
    setPinMode(ddrB, portB, BUZZER_PIN, 1); // Set BUZZER_PIN as OUTPUT
}

void loop() {
    // Request threshold from Potentiometer board
    Wire.requestFrom(SLAVE_POT, 2);
    if (Wire.available() == 2) {
        thresholdDistance = Wire.read() << 8 | Wire.read();
    }

    // Request distance from Ultrasonic board
    Wire.requestFrom(SLAVE_ULTRA, 2);
    if (Wire.available() == 2) {
        measuredDistance = Wire.read() << 8 | Wire.read();
    }

    // Compare and control buzzer
    if (measuredDistance <= thresholdDistance) {
        digitalWriteDirect(portB, BUZZER_PIN, HIGH);
    } else {
        digitalWriteDirect(portB, BUZZER_PIN, LOW);
    }

    // Send data to LCD board
    Wire.beginTransmission(SLAVE_LCD);
    Wire.write(thresholdDistance >> 8);
    Wire.write(thresholdDistance & 0xFF);
    Wire.write(measuredDistance >> 8);
    Wire.write(measuredDistance & 0xFF);
    Wire.endTransmission();

    delay(200);
}

void setPinMode(volatile uint8_t *ddr, volatile uint8_t *port, uint8_t pin, uint8_t mode) {
        if (mode == 0) { // INPUT mode
                *ddr &= ~(1 << pin);    // Clear the corresponding bit in the DDR to set as input
                *port &= ~(1 << pin);   // Clear the corresponding bit in the PORT to disable pull-up
        } else if (mode == 1) { // INPUT_PULLUP mode
                *ddr &= ~(1 << pin);    // Clear the corresponding bit in the DDR to set as input
                *port |= (1 << pin);    // Set the corresponding bit in the PORT to enable pull-up
        } else { // OUTPUT mode
                *ddr |= (1 << pin);     // Set the corresponding bit in the DDR to set as output
        }
}

void digitalWriteDirect(volatile uint8_t *port, uint8_t pin, uint8_t value) {
    if (value == LOW) {
        *port &= ~(1 << pin); // Clear the corresponding bit in the PORT to set it LOW
    } else {
        *port |= (1 << pin);  // Set the corresponding bit in the PORT to set it HIGH
    }
}
