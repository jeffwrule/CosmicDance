#include <TimerOne.h>

// This example creates a PWM signal with 25 kHz carrier.
//
// Arduino's analogWrite() gives you PWM output, but no control over the
// carrier frequency.  The default frequency is low, typically 490 or
// 3920 Hz.  Sometimes you may need a faster carrier frequency.
//
// The specification for 4-wire PWM fans recommends a 25 kHz frequency
// and allows 21 to 28 kHz.  The default from analogWrite() might work
// with some fans, but to follow the specification we need 25 kHz.
//
// http://www.formfactors.org/developer/specs/REV1_2_Public.pdf
//
// Connect the PWM pin to the fan's control wire (usually blue).  The
// board's ground must be connected to the fan's ground, and the fan
// needs +12 volt power from the computer or a separate power supply.

const int ledPin = 9;    // LED connected to digital pin 9

void setup(void)
{
  Timer1.initialize(2000);  // 2000 us is 500Hz
  Serial.begin(250000);
}

void loop() {

  // analogWrite(ledPin, 0);
  // return;
  
  // values to 1023 fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 30; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    // analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    Timer1.pwm(ledPin, fadeValue);
    Serial.println(fadeValue);
    delay(300);
  }

  delay(1000);

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 30 ; fadeValue >= 0; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    // analogWrite(ledPin, fadeValue);
    Timer1.pwm(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    Serial.println(fadeValue);
    delay(300);
  }
  delay(1000);
}
