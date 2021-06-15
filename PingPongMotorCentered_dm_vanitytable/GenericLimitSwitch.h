#ifndef GENERICLIMITSWITCH_H
#define GENERICLIMITSWITCH_H

///////////////////////////////////////////////////////////
//
// Abstract Class to a single limit switch
//
/////////////////////////////////////////////////////////// 
class GenericLimitSwitch {

  public:
    virtual void update() =0;
    virtual void status(boolean do_print) =0;
    virtual char* get_limit_type() =0;
    virtual boolean get_is_at_limit() =0;
    virtual char get_limit_position() =0;
    virtual char* get_switch_name() =0;

};

#endif
