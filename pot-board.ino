#include <avr/io.h>
#include <avr/interrupt.h>

#define SLAVE_ADDRESS 2
#define POT_PIN A0

volatile uint8_t highByte = 0;
volatile uint8_t lowByte = 0;

void setup() {
  // Set up ADC
  ADMUX = (1 << REFS0); // Use AVcc as the reference
  ADMUX |= (POT_PIN & 0x0F); // Select the analog pin
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Enable ADC, prescaler = 64

  // Set up I2C (TWI)
  TWAR = (SLAVE_ADDRESS << 1); // Set slave address
  TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWIE); // Enable TWI, ACK, and interrupt

  sei(); // Enable global interrupts
}

void loop() {
  // Main loop remains empty since everything is interrupt-driven
}

// ADC Read function
uint16_t readADC() {
  ADCSRA |= (1 << ADSC); // Start conversion
  while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
  return ADC;
}

// I2C ISR
ISR(TWI_vect) {
  switch (TWSR & 0xF8) { // Mask status bits
    case 0x60: // Own SLA+W received, ACK sent
      TWCR |= (1 << TWINT) | (1 << TWEA);
      break;

    case 0xA8: // Own SLA+R received, ACK sent
      {
        uint16_t potValue = readADC();
        uint16_t threshold = map(potValue, 0, 1023, 5, 100);

        highByte = (threshold >> 8);
        lowByte = (threshold & 0xFF);

        TWDR = highByte; // Load high byte
        TWCR |= (1 << TWINT); // Clear interrupt flag
      }
      break;

    case 0xB8: // Data byte transmitted, ACK received
      TWDR = lowByte; // Load low byte
      TWCR |= (1 << TWINT); // Clear interrupt flag
      break;

    case 0xC0: // Data byte transmitted, NACK received
    case 0x88: // Previously addressed with own SLA+W, NACK received
      TWCR |= (1 << TWINT) | (1 << TWEA); // Clear interrupt flag, prepare for next transmission
      break;

    case 0x00: // Bus error
      TWCR = (1 << TWSTO) | (1 << TWINT); // Release bus
      break;

    default:
      TWCR |= (1 << TWINT); // Clear interrupt flag for any other state
      break;
  }
}