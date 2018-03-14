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
    virtual void reverse() =0;
    virtual void go_left() =0;
    virtual void go_right() =0;
    virtual boolean is_stopped() =0;
    virtual boolean is_disabled() =0;
    virtual char current_direction() =0;
    virtual void enable() =0;
    virtual void status() =0;
    virtual String get_motor_name() =0;
    virtual void set_target_speed(unsigned int new_target_speed) =0;
    virtual unsigned int get_target_speed();
    virtual unsigned int get_max_speed() =0;
};

#endif
