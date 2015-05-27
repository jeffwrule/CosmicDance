//  Demo function:The application method to drive the DC motor.
//  Author:Frankie.Chu
//  Date:20 November, 2012

#include "MotorDriver.h"

void setup()
{
	/*Configure the motor A to control the wheel at the left side.*/
	/*Configure the motor B to control the wheel at the right side.*/
	motordriver.init();
}
 
void loop()
{
  	motordriver.setSpeed(255,MOTORA);
	motordriver.goForward();
        delay(100);
        motordriver.stop();
  	motordriver.setSpeed(70,MOTORA);
	motordriver.goForward();
	delay(10000);
        motordriver.stop();
//	motordriver.goBackward();
//	delay(2000);

        motordriver.stop();
        delay(2000);
        
        motordriver.setSpeed(255, MOTORA);
	motordriver.goForward();
	delay(10000);
        motordriver.stop();
//	motordriver.goBackward();
//	delay(2000);

        motordriver.stop();
        delay(2000);
}
