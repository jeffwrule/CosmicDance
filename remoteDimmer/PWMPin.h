#ifndef PWMPIN_H
#define PWMPIN_H

#include "PWM.h"

///////////////////////////////////////////////////////////
//
// Class to manage pwm on a simple pin for dimming 
// 
//
/////////////////////////////////////////////////////////// 
class PWMPin: public PWMDevice {
  public:
    void set_pwm_level(unsigned int pwm_level);
    unsigned int get_pwm_min();   // the minimum value on this pin
    unsigned int get_pwm_max();   // the maximum value on this pin
    unsigned int get_pwm_pin();   // return the current pin number
    char *get_pwm_type();         // return the type of this pin


    // pwm_pin      the PWM enable pin to use withi this routine
    PWMPin(unsigned int p_pwm_pin) :
      pwm_pin(p_pwm_pin)
      { 
          pinMode(pwm_pin, OUTPUT);
          analogWrite(pwm_pin, 0);


          Serial.print(F("CONSTRUCTOR::PWMPin: pwm_pin="));
          Serial.println(pwm_pin);
          Serial.println(F("end CONSTRUCTOR::PWMPin"));
       }
    
  private:
    const unsigned int pwm_min=0;       // min pwm level 0-255
    const unsigned int pwm_max=255;    // max pwm level 0-255
    unsigned int pwm_pin;               // what pin are we manipulating
    const char* pwm_type="PIN";
};

void PWMPin::set_pwm_level(unsigned int pwm_level) { analogWrite(pwm_pin, pwm_level); }

unsigned int PWMPin::get_pwm_min() {return pwm_min; }
    
unsigned int PWMPin::get_pwm_max() {return pwm_max; }

unsigned int PWMPin::get_pwm_pin() {return pwm_pin; }

char * PWMPin::get_pwm_type() { return pwm_type; }



////////////////////// end HBridgeMotor //////////////////////////


#endif
