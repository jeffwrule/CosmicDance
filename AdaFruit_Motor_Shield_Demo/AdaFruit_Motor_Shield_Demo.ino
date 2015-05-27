/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_PWMServoDriver.h"

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Or, create it with a different I2C address (say for stacking)
// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

// Select which 'port' M1, M2, M3 or M4. In this case, M1
Adafruit_DCMotor *myMotor = AFMS.getMotor(4);
// You can also make another motor on port M2
//Adafruit_DCMotor *myOtherMotor = AFMS.getMotor(2);

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  // create with the default frequency 1.6KHz
  //AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
  // Set the speed to start, from 0 (off) to 255 (max speed)
  myMotor->setSpeed(150);
  myMotor->run(FORWARD);
  // turn on motor
  myMotor->run(RELEASE);
}

void loop() {
  uint8_t i;
  
  Serial.print("FORWARD...\n");
  myMotor->run(FORWARD);
  Serial.print("forward speed 50\n");
  myMotor->setSpeed(50); 
  Serial.print("Sleeping 10 seconds\n");
  delay(10000);
 
  Serial.print("forward speed 255\n");
  myMotor->setSpeed(255); 
  Serial.print("Sleeping 10 seconds\n");
  delay(10000);
 
  Serial.print("BACKWARD...\n");
  myMotor->run(BACKWARD);
  Serial.print("backward speed 50\n");
  myMotor->setSpeed(50); 
  Serial.print("Sleeping 10 seconds\n");
  delay(10000);
 
  Serial.print("backward speed 255\n");
  myMotor->setSpeed(255); 
  Serial.print("Sleeping 10 seconds\n");
  delay(10000);

  Serial.print("Motor Release...\n");
  myMotor->run(RELEASE);
  Serial.print("sleeping 5 seconds...\n");
  delay(5000);
}
