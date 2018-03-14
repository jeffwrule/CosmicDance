/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(A6, OUTPUT);
  pinMode(A7, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A3, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A4, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A5, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A6, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A7, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(0, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(1, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(3, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(9, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(10, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(11, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(5000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(A0, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A1, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A2, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A3, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A4, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A5, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A6, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(A7, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(0, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(1, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(2, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(3, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(4, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(5, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(7, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(8, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(9, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(10, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(11, LOW);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(12, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(3000);                       // wait for a second
}
