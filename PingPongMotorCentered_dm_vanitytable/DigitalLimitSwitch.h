#ifndef DIGITALLIMITSWITCH_H
#define DIGITALLIMITSWITCH_H

#include "GenericLimitSwitch.h"

//////////////////////////////////////////////////////////////
//
// Limit Switch Class Specific Classes..
//
/////////////////////////////////////////////////////////// 
/// Any digital limit switch which just has 0/1, input_pin will be set as INPUT_PULLUP
class DigitalLimitSwitch: public GenericLimitSwitch{

  public:
    void update();
    void status(boolean do_print);
    char* get_limit_type();
    boolean get_is_at_limit();
    char get_limit_position();
    char* get_switch_name();

    // p_siwtch_name            name for this limit switch
    // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    DigitalLimitSwitch (
          const char* p_switch_name, 
          int p_digital_pin, 
          char p_limit_position, 
          const char* p_normally_open_closed) :
      switch_name(p_switch_name),
      digital_pin(p_digital_pin),      
      limit_position(p_limit_position)
      { if (limit_position != 'l' && limit_position != 'r' && limit_position != 'c') {
          Serial.print(F("ERROR: Limit Switch "));
          Serial.print(switch_name);
          Serial.println(F(" limit_position="));
          Serial.println(limit_position);
        }
        if (strcmp(p_normally_open_closed,"NC") == 0) {
          limit_off = nc_off;
        } else {
          limit_off = no_off;
        }
        pinMode(digital_pin, INPUT_PULLUP);
        update();
        // print the init and set once values
        Serial.print(F("CONSTRUCTOR::DigitalLimitSwitch p_switch_name="));
        Serial.print(p_switch_name);
        Serial.print(F(", limit_type="));
        Serial.print(limit_type);
        Serial.print(F(", p_digital_pin="));
        Serial.print(p_digital_pin);
        Serial.print(F(", p_limit_position="));
        Serial.print(p_limit_position);
        Serial.print(F(", p_normally_open_closed="));
        Serial.print(p_normally_open_closed);
        Serial.print(F(", limit_off="));
        Serial.println(limit_off);
      }
      
  private:

    const char* limit_type = "digital";
    boolean is_at_limit=false;
    char limit_position;
    const char* switch_name;

    const int nc_off=0;   // NC(normally closed), so no push is closed circuit to ground so 0
    const int no_off=1;   // NO(normally open), so not pushed and pulled up to 5v by INPUT_PULLUP resistor

    int digital_pin;
    int limit_off;        // limit switch in 'open'/'not pushed' state
};

char* DigitalLimitSwitch::get_limit_type() {return limit_type; }
boolean DigitalLimitSwitch::get_is_at_limit() {return is_at_limit; }
char DigitalLimitSwitch::get_limit_position() {return limit_position; }
char* DigitalLimitSwitch::get_switch_name() {return switch_name; };

void DigitalLimitSwitch::update() {
 // delayMicroseconds(PIN_READ_DELAY_MS);
  is_at_limit = !digitalReads(digital_pin, 5) == limit_off;
}

void DigitalLimitSwitch::status(boolean do_print) {
  if (! do_print) {
    return;
  }
  Serial.print(F("DigitalLimitSwitch:: switch_name="));
  Serial.print(switch_name);
  Serial.print(F(", position="));
  Serial.print(limit_position);
  Serial.print(F(", is_at_limit: "));
  Serial.println(bool_tostr(is_at_limit));
}

#endif
