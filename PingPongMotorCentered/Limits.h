#ifndef LIMITS_H
#define LIMITS_H

#include "GenericLimitSwitch.h"

///////////////////////////////////////////////////////////
//
// Class to manage a collection of limit switches
//  there are three: left, right, center
//  Center switch may be a real limit switch  
//  just a pointer to left or right limit switch
//
///////////////////////////////////////////////////////////

class Limits {
   
   public:
      void update( char motor_direction );
      boolean is_max_left( void );
      boolean is_max_right( void );
      boolean is_centered( void );
      char center_is(void);         // what postion is the center switch l,r,c
      void status(boolean do_print);
      char    home_is;            // home position l,r,c (the position of the center_switch)
      int     sum_limits( void ); // number of limit swtiches active (test for errors)

      // p_limits_name        The name of this limits combo
      // p_left_limit         The left limit switch structure
      // p_right_limit        The right limit switch structure
      // p_center_limit       The center limit swtich struct (AKA the home switch) can be a pointer to left_limit or right_limit if the represent home
      Limits(const char* p_limits_name,
             GenericLimitSwitch* p_left_limit, 
             GenericLimitSwitch* p_right_limit,
             GenericLimitSwitch* p_center_limit) :
        limits_name(p_limits_name),
        left_limit(p_left_limit), 
        right_limit(p_right_limit), 
        center_limit(p_center_limit) 
        { home_is =  center_limit->get_limit_position(); }  
 
   private:
      const char* limits_name;
      GenericLimitSwitch* left_limit;     //left limit switch 
      GenericLimitSwitch* right_limit;    //left limit switch 
      GenericLimitSwitch* center_limit;   //left limit switch 
      boolean left_limit_active;
      boolean right_limit_active;
      boolean center_limit_active;
};

void Limits::update(char motor_direction) {

  left_limit->update();
  right_limit->update();
  center_limit->update();

  left_limit_active = left_limit->get_is_at_limit();
  right_limit_active = right_limit->get_is_at_limit();
  center_limit_active = center_limit->get_is_at_limit();

  // if we are using an acs for the motor left and right, power will be off when we reach either
  // we must used the current motor direction to work this out.
  if (right_limit->get_limit_type() == "acs712" && left_limit->get_limit_type() == "acs712") {
      if (motor_direction == 'l') {  // was going left
        right_limit_active = false;      // we are not right
      } else { // was going right, must be right
        left_limit_active = false;       // we are not left      
      }
  }

}

int  Limits::sum_limits() {
    int limits_active=0;
    if (left_limit_active) { limits_active++; }
    if (right_limit_active) { limits_active++; }
    // handle case where we re-use the left or right limit for home/center
    if (center_limit_active && center_limit->get_limit_position() == 'c') { limits_active++; }
    return limits_active;
}

boolean Limits::is_max_left() {
    return left_limit_active;
}

boolean Limits::is_max_right() {
    return right_limit_active;
}

boolean Limits::is_centered() {
  return center_limit_active;
}

char Limits::center_is() {
  return center_limit->get_limit_position();
}


void Limits::status(boolean do_print) {
  if (! do_print) {
    return;
  }
  // dump the limit switch info
  Serial.print(F("L_LimitSwitch: "));
  left_limit->status(do_print);
  Serial.print(F("R_LimitSwitch: "));
  right_limit->status(do_print);
  Serial.print(F("C_LimitSwitch: "));
  center_limit->status(do_print);

  // dump the Limits info (group info)
  Serial.print(F("Limits:: limits_name="));
  Serial.print(limits_name);
  Serial.print(F(", is_max_left="));
  Serial.print(bool_tostr(is_max_left()));
  Serial.print(F(" is_centered="));
  Serial.print(bool_tostr(is_centered()));
  Serial.print(F(", is_max_right="));
  Serial.println(bool_tostr(is_max_right()));
}


////////////////////// end Limits //////////////////////////

#endif
