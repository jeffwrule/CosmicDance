/*
  Fading

  This example shows how to fade an LED using the analogWrite() function.

  The circuit:
  - LED attached from digital pin 9 to ground.

  created 1 Nov 2008
  by David A. Mellis
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Fading
*/

int ledPin = 9;    // LED connected to digital pin 9

void setup() {
  // nothing happens in setup
  Serial.begin(250000);
}

void loop() {

  // analogWrite(ledPin, 0);
  // return;
  
  // fade in from min to max in increments of 5 points:
  for (int fadeValue = 0 ; fadeValue <= 30; fadeValue += 1) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    Serial.println(fadeValue);
    delay(300);
  }

  delay(1000);

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 30 ; fadeValue >= 0; fadeValue -= 1) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    Serial.println(fadeValue);
    delay(300);
  }
  delay(1000);
}
