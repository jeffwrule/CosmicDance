#ifndef PWMSFSHIELD_H
#define PWMSFSHIELD_H

#include "PWM.h"
#include "SparkFun_Tlc5940.h"

///////////////////////////////////////////////////////////
//
// Class to pins on Spark Fun TLC5940 Shield 
// SparkFun Part: DEV-10615
//
/////////////////////////////////////////////////////////// 
class PWMSFShield: public PWMDevice {
  public:
    void set_pwm_level(unsigned int pwm_level);
    unsigned int get_pwm_min();   // the minimum value on this pin
    unsigned int get_pwm_max();   // the maximum value on this pin
    unsigned int get_pwm_pin();   // return the current pin number
    char *get_pwm_type();         // return the type of this pin


    // pwm_pin      the PWM enable pin to use withi this routine
    PWMSFShield(unsigned int p_pwm_pin) :
      pwm_pin(p_pwm_pin)
      { 
          Tlc.init(4095);
          Tlc.set(pwm_pin, 4095);
          Tlc.update();

          Serial.print(F("CONSTRUCTOR::PWMSFShield: pwm_pin="));
          Serial.println(pwm_pin);
          Serial.println(F("end CONSTRUCTOR::PWMPin"));
       }
    
  private:
    const unsigned int pwm_min=0;       // min pwm level 4095-0; not this driver is off at 4095 and 0 is full on
    const unsigned int pwm_max=4095;    // max pwm level 4095-0  not this driver is off at 4095 and 0 is full on
    unsigned int pwm_pin;               // what pin are we manipulating
    const char* pwm_type="SFShield";
    unsigned int last_pwm=pwm_max+1;
};

void PWMSFShield::set_pwm_level(unsigned int pwm_level) { 
   if (pwm_level == last_pwm) return;
   Tlc.set(pwm_pin, 4095 - pwm_level);  Tlc.update(); 
   last_pwm=pwm_level;
 
   
//  for (int i=0; i<16; i++) {
//     Tlc.set(i, 4095 - pwm_level); 
//  }
//  Tlc.update();
  }

unsigned int PWMSFShield::get_pwm_min() {return pwm_min; }
    
unsigned int PWMSFShield::get_pwm_max() {return pwm_max; }

unsigned int PWMSFShield::get_pwm_pin() {return pwm_pin; }

char * PWMSFShield::get_pwm_type() { return pwm_type; }



////////////////////// end HBridgeMotor //////////////////////////


#endif
