#ifndef PLAYBOOK_H
#define PLAYBOOK_H

#include "config.h"
#include "Light.h"
#include "InterruptDev.h"
#include "Playbook.h"
#include "Motor.h"


const uint8_t PB_NUM_LIGHTS=6;
const uint8_t PB_NUM_ACTIVE_STEPS=8;
const uint32_t NEW_START_POSITION_CHECK_DELAY=2000;

typedef struct Step {
    boolean   wait_for_position=false;  // are we waiting for the position marker
    uint32_t  delay_position_check_ms;  // how long to wait until we check for a position interrupt, we can get a few
    int16_t   low_level;                // lighting low_level  (for oscellating)
    int16_t   high_level;               // lighting high_level (for oscellatiing)
    uint32_t  step_delay_ms;            // how long to delay each step 
};

class Playbook {

  private:

    Light         **lights;       // list of lights with pins and level info
    uint8_t       num_lights;     // how many lights do we have  
    InterruptDev  *position_dev;  // position interrupt, interrupts at 12,3,6,9 o'clock positions
    InterruptDev  *on_off_dev;    // push button on/off device
    Motor         *motor;         // motor that rotates or not
    
    uint32_t      delay_until_ms=0;     // don't accept new position interrupts until this time
    uint32_t      cur_ms;               // current milliseconds
    uint32_t      last_status_ms=0;     // check for time to print new status
    uint32_t      status_delay_ns=1500; // print every 1.5 seconds

    boolean       new_position=false;   // did we hit the position limit switch 
    boolean       new_on_off=false;     // did someone push the start/stop button
    uint16_t      last_position_int;    // number of position interrupts seen
    uint16_t      last_on_off_int;      // number of on off interrupts seen

    boolean       is_active=true;       // are we active or inactive (switched with push button, starts out as active)
    boolean       is_new_step=false;    // time to switch to the next step
    
    void next_step();                  // transition to next step 
    void stop();                       // stop the device
    void start();                      // start the device
    void run_active();                 // start the device
    void run_inactive();               // start the device

  public:
  
    void run();     // activate the plan
    void display(); // dump the details of our object
    void status();  // current running status

    Playbook(Light **lights,  
            uint8_t num_lights, 
            InterruptDev *position_dev, 
            InterruptDev *on_off_dev, 
            Motor *motor) :
              lights(lights), 
              num_lights(num_lights), 
              position_dev(position_dev), 
              on_off_dev(on_off_dev), 
              motor(motor) 
       {
            last_position_int = position_dev->get_num_interrupts();
            last_on_off_int = on_off_dev->get_num_interrupts();
            is_active=true;
            motor->stop();
            is_new_step=false;
            cur_ms = millis();
       }
};


// actively look for external inputs (stop start and position milestones).  
// Then apply the appropriate next step
void Playbook::run() {

  cur_ms = millis();
  if (cur_ms > delay_until_ms) delay_until_ms=0;  // check for ms wraparound

  // check for position switch ;  there are times when we want to ignore new interrupts on this pin
  if (cur_ms > delay_until_ms) {
    if (position_dev->get_num_interrupts() > last_on_off_int) {
      new_position = true;
    }
  }

  // check for on_off_switch push
  if (on_off_dev->get_num_interrupts() > last_on_off_int) {
    new_on_off = true;
  }

  status();

  // handle new on/off switch push
  if (new_on_off) {
    if (is_active) {
      stop();
    } else {
      start();
    }
    delay_until_ms = millis() + NEW_START_POSITION_CHECK_DELAY;
    last_position_int = position_dev->get_num_interrupts();
    last_on_off_int = on_off_dev->get_num_interrupts();
    new_on_off = false;
  }

  // new_position is handled in run_active();
  
  if (is_active) {
    run_active();
  } else {
    run_inactive();
  }
  
}

// run through the active steps
void Playbook::run_active() {
  
}

// stop the device, put it in idle mode
void Playbook::run_inactive() {
}


// start the whole series again
void Playbook::start() {

  delay_until_ms=cur_ms + 500;
  // lets start moving
  motor->start();
  
  // this is just a place holder for implement the steps 
  Light::next_step_ms=40;           // when to execute the next step
  Light::dimm_low=DIMM_LEVEL_LOW;   // level we are starting from (for constraining light and oscellation)
  Light::dimm_high=DIMM_LEVEL_HIGH; // lower bound of our dimm range (will likely be same as either dimm_low or dimm_high)

  // this will be removed, just do something with the lights for now
  for (uint8_t i=0; i<num_lights; i++) {
    lights[i]->dimm_target=DIMM_LEVEL_HIGH;
  }
}

// stop the device, put it in idle mode
void Playbook::stop() {
  
  motor->stop();
  
  Light::next_step_ms=40;           // when to execute the next step
  Light::dimm_low=DIMM_LEVEL_LOW;   // level we are starting from (for constraining light and oscellation)
  Light::dimm_high=DIMM_LEVEL_HIGH; // lower bound of our dimm range (will likely be same as either dimm_low or dimm_high)
  
  // this will be removed, just do something with the lights for now
  for (uint8_t i=0; i<num_lights; i++) {
    lights[i]->dimm_target = DIMM_LEVEL_LOW;
  }
}

void Playbook::display() {

  Serial.println(F("Playbook: "));
  for (uint8_t i=0; i<num_lights; i++) {
    lights[i]->display();
    Serial.println(F(""));
  }
  Serial.print(F("    num_lights: "));
  Serial.println(num_lights);
  position_dev->display();
  on_off_dev->display();
  motor->display();
  Serial.print(F("    delay_until_ms: "));
  Serial.println(delay_until_ms);
  Serial.print(F("    num_lights: "));
  Serial.println(num_lights);
  Serial.print(F("    cur_ms: "));
  Serial.println(cur_ms);
  Serial.print(F("    new_position: "));
  Serial.println(new_position);
  Serial.print(F("    new_on_off: "));
  Serial.println(new_on_off);
  Serial.print(F("    last_position_int: "));
  Serial.println(last_position_int);
  Serial.print(F("    last_on_off_int: "));
  Serial.println(last_on_off_int);
  Serial.print(F("    is_active: "));
  Serial.println(is_active);
  Serial.print(F("    is_new_step: "));
  Serial.println(is_new_step);  
}

void Playbook::status() {

  if (cur_ms < last_status_ms) last_status_ms = cur_ms;
  if (cur_ms - last_status_ms < status_delay_ns) return;
  last_status_ms = cur_ms;
  
  Serial.print(F("Playbook::status(): "));
  Serial.print(F("new_position="));
  Serial.print(new_position);
  Serial.print(F(", new_on_off="));
  Serial.print(new_on_off);
  Serial.print(F(", is_active="));
  Serial.print(is_active);
  Serial.print(F(", is_new_step="));
  Serial.print(is_new_step);
  Serial.print(F(", on_of_dev_interrupts="));
  Serial.print(on_off_dev->get_num_interrupts());
  Serial.println(F(""));
  on_off_dev->display();
}

#endif
