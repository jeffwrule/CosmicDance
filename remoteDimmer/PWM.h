#ifndef PWM_H
#define PWM_H

///////////////////////////////////////////////////////////
//
// Abstract Class to manage motor half_bridge and full bridge motors
//
/////////////////////////////////////////////////////////// 
class PWMDevice {

  public:
    virtual void set_pwm_level(unsigned int pwm_level) =0;
    virtual unsigned int get_pwm_pin() = 0;
    virtual unsigned int get_pwm_min() =0;
    virtual unsigned int get_pwm_max() =0;
    virtual char * get_pwm_type()=0;
};

#endif
