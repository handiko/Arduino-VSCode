#include <Arduino.h>

#define DELAY 750
#define PIN_LED 5

void setup() {
  DDRB |= (1 << PIN_LED);
}

void loop() {
  PORTB |= (1 << PIN_LED);
  delay(DELAY);

  PORTB &= ~(1 << PIN_LED);
  delay(DELAY);
}