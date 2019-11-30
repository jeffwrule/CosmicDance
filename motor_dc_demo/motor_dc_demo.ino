//  Demo function:The application method to drive the DC motor.
//  Author:Loovee (luweicong@seeed.cc)
//  2016-3-11

#include "MotorDriver.h"

MotorDriver motor;

int my_speeed=90;
long int speed_delay_long=5000;
long int speed_delay_short=1000;

void setup()
{
    // initialize
    motor.begin();
    Serial.begin(250000);
}

void loop()
{
Serial.print("Going right "); Serial.println(my_speeed);
    motor.speed(0, my_speeed);            // set motor0 to speed 100
    delay(speed_delay_long);
Serial.println("Brake");
    motor.brake(0);                 // brake
    delay(speed_delay_short);
Serial.print("Going left "); Serial.println(-my_speeed);
    motor.speed(0, -my_speeed);           // set motor0 to speed -100
    delay(speed_delay_long);
Serial.println("Stopping");
    motor.stop(0);                  // stop
    delay(speed_delay_short);
}
// END FILE
