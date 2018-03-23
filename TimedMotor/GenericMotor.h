#ifndef GENERICMOTOR_H
#define GENERICMOTOR_H

///////////////////////////////////////////////////////////
//
// Abstract Class to manage motor half_bridge and full bridge motors
//
/////////////////////////////////////////////////////////// 
class GenericMotor {

  public:
    virtual void stop() =0;
    virtual void start() =0;
    virtual void run() =0;
    virtual boolean get_is_stopped() =0;
    virtual void status(boolean do_print, unsigned long current_millis) =0;
    virtual char *get_motor_name() =0;
    virtual void set_start_speed(unsigned int new_start_speed) =0;
    virtual void set_target_speed(unsigned int new_target_speed) =0;
    virtual unsigned short get_target_speed();
    virtual unsigned short get_max_speed() =0;
    virtual int get_subclass() =0; 
};

#define SUB_CLASS_PWMMOTOR 100;
#define SUB_CLASS_ROTATINGMOTOR 200;

#endif
