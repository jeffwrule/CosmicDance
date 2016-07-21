/*
IBT-2 Motor Control Board driven by Arduino.

Speed and direction controlled by a potentiometer attached to analog input 0.
One side pin of the potentiometer (either one) to ground; the other side pin to +5V

Connection to the IBT-2 board:
IBT-2 pin 1 (RPWM) to Arduino pin 5(PWM)
IBT-2 pin 2 (LPWM) to Arduino pin 6(PWM)
IBT-2 pins 3 (R_EN), 4 (L_EN), 7 (VCC) to Arduino 5V pin
IBT-2 pin 8 (GND) to Arduino GND
IBT-2 pins 5 (R_IS) and 6 (L_IS) not connected
*/

int SENSOR_PIN = 0; // center pin of the potentiometer

int RPWM_Output = 5; // Arduino PWM output pin 5; connect to IBT-2 pin 1 (RPWM)
int LPWM_Output = 6; // Arduino PWM output pin 6; connect to IBT-2 pin 2 (LPWM)

void setup()
{
  pinMode(RPWM_Output, OUTPUT);
  pinMode(LPWM_Output, OUTPUT);
  Serial.begin (9600);
  Serial.println("Init Complete");
 }

void loop()
{
  int sensorValue = analogRead(SENSOR_PIN);

  // sensor value is in the range 0 to 1023
  // the lower half of it we use for reverse rotation; the upper half for forward rotation

 Serial.print("sensor value=");
 Serial.println (sensorValue);
  
  //if (sensorValue > 511)
 
    // reverse rotation
    int reversePWM = 127;
    analogWrite(RPWM_Output, 0);
    analogWrite(LPWM_Output, reversePWM);

 
  Serial.println("turn left for 5 seconds");
  delay(5000);
 
    analogWrite(RPWM_Output, 0);
    analogWrite(LPWM_Output, 0);

 
  //forward rotation
  int forwardPWM = (127);
  analogWrite(LPWM_Output, 0);
  analogWrite(RPWM_Output, forwardPWM);

  Serial.println("turn right for 5 seconds");
  delay(5000);
 
    analogWrite(RPWM_Output, 0);
    analogWrite(LPWM_Output, 0);

  
}



