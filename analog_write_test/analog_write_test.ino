/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

#define ANALOG_PIN A0

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  pinMode(ANALOG_PIN, OUTPUT);
  Serial.println(F("End setup"));

}

// the loop function runs over and over again forever
void loop() {
  analogWrite(ANALOG_PIN, 50);   // turn the LED on (HIGH is the voltage level)
  Serial.println(F("Output 50"));
  delay(2000);              // wait for a second
  analogWrite(ANALOG_PIN, 100);    // turn the LED off by making the voltage LOW
  Serial.println(F("Output 100"));
  delay(2000);              // wait for a second
  analogWrite(ANALOG_PIN, 150);    // turn the LED off by making the voltage LOW
  Serial.println(F("Output 150"));
  delay(2000);              // wait for a second
  analogWrite(ANALOG_PIN, 200);    // turn the LED off by making the voltage LOW
  Serial.println(F("Output 200"));
  delay(2000);              // wait for a second
  analogWrite(ANALOG_PIN, 250);    // turn the LED off by making the voltage LOW
  Serial.println(F("Output 250"));
  delay(2000);              // wait for a second
}

