/*
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */

#include <FastLED.h>
 
int trigPin = 11;    // Trigger
int echoPin = 12;    // Echo
long duration, cm, inches;

boolean is_active = true;

 
void setup() {
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  is_active = true;
}


void test_on_off() {

  static unsigned long last_ms=0;
  unsigned long cur_ms = millis();
  if (cur_ms < last_ms) last_ms = 0;

  if (cur_ms - last_ms < 2000) return;
  
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
 
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
  
  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  if (inches <= 20) {
    is_active = !is_active;
    last_ms = cur_ms;
    Serial.print("is_active triggered: ");
    Serial.println(is_active);
  }
}

void loop() {

  static boolean is_off=true;

  EVERY_N_MILLISECONDS(500) {
    test_on_off();
  }

  if (is_off != is_active) {
    if (is_active) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    is_off=is_active;
  }

}
