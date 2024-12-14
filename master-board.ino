#include <Wire.h>

#define SLAVE_POT 2
#define SLAVE_ULTRA 3
#define SLAVE_LCD 4
#define BUZZER_PIN 4
#define BUTTON_PIN 5 // Pin for the disarm button

volatile uint8_t *ddrB = &DDRB;
volatile uint8_t *portB = &PORTB;
volatile uint8_t *pinB = &PINB;

int thresholdDistance = 0;
int measuredDistance = 0;
bool alarmArmed = true; // Flag to track if the alarm is armed
unsigned long lastDebounceTime = 0; // To track debounce timing
const unsigned long debounceDelay = 50; // 50 ms debounce delay

void setup() {
    Wire.begin(); // Start as master
    PinConfig(ddrB, portB, BUZZER_PIN, 2); // Set BUZZER_PIN as OUTPUT
    PinConfig(ddrB, portB, BUTTON_PIN, 1); // Set BUTTON_PIN as INPUT_PULLUP
}

void loop() {
    // Check and debounce button
    if (debouncedReadButton()) {
        alarmArmed = !alarmArmed; // Toggle the alarm state
        if (!alarmArmed) {
            WriteRegisterDigital(portB, BUZZER_PIN, LOW); // Stop the buzzer
        }
    }

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
    if (alarmArmed && measuredDistance <= thresholdDistance) {
        WriteRegisterDigital(portB, BUZZER_PIN, HIGH);
    } else {
        WriteRegisterDigital(portB, BUZZER_PIN, LOW);
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

void PinConfig(volatile uint8_t *ddr, volatile uint8_t *port, uint8_t pin, uint8_t mode) {
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

void WriteRegisterDigital(volatile uint8_t *port, uint8_t pin, uint8_t value) {
    if (value == LOW) {
        *port &= ~(1 << pin); // Clear the corresponding bit in the PORT to set it LOW
    } else {
        *port |= (1 << pin);  // Set the corresponding bit in the PORT to set it HIGH
    }
}

uint8_t ReadPin(volatile uint8_t *pinReg, uint8_t pin) {
    return (*pinReg & (1 << pin)) ? HIGH : LOW; // Read the state of the pin
}

bool debouncedReadButton() {
    static uint8_t lastButtonState = HIGH; // Last stable button state
    uint8_t currentButtonState = ReadPin(pinB, BUTTON_PIN);

    if (currentButtonState != lastButtonState) {
        lastDebounceTime = millis(); // Reset debounce timer
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        // If the button state has stabilized
        if (currentButtonState == LOW && lastButtonState == HIGH) {
            lastButtonState = currentButtonState; // Update last state
            return true; // Button was pressed
        }
    }

    lastButtonState = currentButtonState; // Update last state
    return false; // No press detected
}
