/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>

#define DELAY 200
#define PIN_LED 5

void setup()
{
  DDRB = (1 << PIN_LED);
}

void loop()
{
  // turn the LED on (HIGH is the voltage level)
  PORTB |= (1 << PIN_LED);
  // wait for a second
  delay(DELAY);
  // turn the LED off by making the voltage LOW
  PORTB &= ~(1 << PIN_LED);
   // wait for a second
  delay(DELAY);
}
