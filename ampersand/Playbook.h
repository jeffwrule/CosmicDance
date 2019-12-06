#ifndef PLAYBOOK_H
#define PLAYBOOK_H

#include "config.h"
#include "Light.h"
#include "InterruptDev.h"
#include "Playbook.h"
#include "Motor.h"
#include "Playbook_steps.h"


//const uint8_t PB_NUM_LIGHTS=6;
//const uint8_t PB_NUM_ACTIVE_STEPS=8;
//const uint32_t NEW_START_POSITION_CHECK_DELAY=2000;


#define IS_OSCELLATING true
#define NOT_OSCELLATING false


      
class Playbook {

  private:

    Light           **lights;               // list of lights with pins and level info
    uint8_t         num_lights;             // how many lights do we have  
    InterruptDev    **positions;            // position interrupts, interrupts at 12,3,6,9 o'clock positions
    uint8_t         num_positions;          // how many items in the positions list
    InterruptDev    *on_off_dev;            // push button on/off device
    Motor           *motor;                 // motor that rotates or not
    ZeroCrossDimmer *zdimmer;               // pointer to the zdimmer
    uint8_t         skips_left;             // for a given wait for position step skip this many interrupts before stopping
    
    uint32_t      delay_pos_int_until_ms=0;   // don't accept new position interrupts until this absolute time
    boolean       delay_pos_waiting=false;    // are we in a wait state? (need this to be able to clear the interrupts after we are done
    uint32_t      timed_step_delay_until_ms;  // absolute time after which we should switch to the next step, on used when wait_for = duration
    uint32_t      dimm_cycle_end_ms;          // when does the current dimm cycle end, we want to be able to pulse the lights together

    uint32_t      last_status_ms=0;     // check for time to print new status
    uint32_t      status_delay_ns=1500; // print every 1.5 seconds

    boolean       new_position=false;   // did we hit the position limit switch 
    boolean       new_on_off=false;     // did someone push the start/stop button
    uint16_t      last_position_int;    // number of position interrupts seen
    uint16_t      last_on_off_int;      // number of on off interrupts seen


    boolean       is_active=true;       // are we active or inactive (switched with push button, starts out as active)

    Step          *cur_step_list;       // pointer to the active set of steps
    Step          *cur_step;            // pointer to the current active step
    LightPrep     **cur_step_lights;    // a list (array) of LightPrep pointers

    void start();                      // start the device
    void stop();                       // stop the device
    void next_step();                  // setup the next step in the list as the active step
    void activate_current_step();      // transition to next step 
    void run_active();                 // start the device
    void run_inactive();               // start the device
    void adjust_dimmer();              // change the light levels
    void start_current_step();         // apply the config from cur_setp_list[cur_step_id] to our state machine to start it off
    uint16_t get_sum_interrupts(InterruptDev **dev_list, uint8_t num_devs);     // return a some of all the position interrupts
    uint8_t get_pin_states(InterruptDev **dev_list, uint8_t num_devs);     // return a some of all the position interrupts  
    void position_display();            // loop through the current positions

  public:
  
    void run();                         // activate the plan
    void display();                     // dump the details of our object
    void status(bool do_print=false);   // current running status, always print when do_print=true, else do normal wait for time.

    Playbook(Light **lights,  
            uint8_t num_lights, 
            InterruptDev **positions,
            uint8_t      num_positions, 
            InterruptDev *on_off_dev, 
            Motor *motor,
            ZeroCrossDimmer *zdimmer) :
              lights(lights), 
              num_lights(num_lights), 
              positions(positions),
              num_positions(num_positions), 
              on_off_dev(on_off_dev), 
              motor(motor),
              zdimmer(zdimmer)
       {
            Serial.println(F("Playbook Constructor Starting"));
            last_position_int = get_sum_interrupts(positions, num_positions);
            last_on_off_int = on_off_dev->get_num_interrupts();
            cur_ms = millis();
            for (uint8_t i=0; i<num_lights; i++) {
              lights[i]->level = DIMM_LEVEL_OFF;
            }
            start();
            Serial.println(F("Playbook Constructor Complete"));
       }
};

// get the some of interrupts for all the devices in this list
uint16_t Playbook::get_sum_interrupts(InterruptDev **dev_list, uint8_t num_devs) {
  uint16_t int_sum = 0;
  for (uint8_t i=0; i<num_devs; i++) {
    int_sum += dev_list[i]->get_num_interrupts();
  }
  return(int_sum);
}

// are any of the pins really pushed right now
uint8_t Playbook::get_pin_states(InterruptDev **dev_list, uint8_t num_devs) {
  for (uint8_t i=0; i<num_devs; i++) {
    if(dev_list[i]->get_pin_state() == INTERRUPT_DEV_ON) {
      return(INTERRUPT_DEV_ON);
    }
  }
  return(INTERRUPT_DEV_OFF);
}

// actively look for external inputs (stop start and position milestones).  
// Then apply the appropriate next step
void Playbook::run() {

  static uint32_t last_ms=0;
  static uint32_t num_interrupts;

  if (cur_ms_wrap) {
    Serial.print(F("%%%%% cur_ms wrapped! %%%%%"));
    delay_pos_int_until_ms=cur_ms;
    timed_step_delay_until_ms=cur_ms;
    for (uint8_t i=0; i<num_lights; i++) {
      lights[i]->dimm_delay_until_ms = cur_ms;
    }
    last_status_ms=cur_ms;
  }

  // check for position switch, ignore interrupts until delay_pos_int_until_ms has been reached 
  if (cur_ms >= delay_pos_int_until_ms) {
    // clear out old interrupts on our first pass after the delay is over
    if (delay_pos_waiting == true) {   
      delay_pos_waiting = false;
      last_position_int = get_sum_interrupts(positions, num_positions);     
    }
    
    // do we have new position interrupts
    num_interrupts = get_sum_interrupts(positions, num_positions); 
    if (num_interrupts > last_position_int) {
      
      // must find at least one pin currently pushed
      if (get_pin_states(positions, num_positions) == INTERRUPT_DEV_ON) {
        if (skips_left > 0) {
          skips_left = skips_left-1;
          Serial.print(F("\n\n\n"));
          Serial.println(F("%%% SKIPPING INTERRUPT skips_left="));
          Serial.println(skips_left);
          Serial.print(F("\n\n\n"));
        } else {
          Serial.println(F("%%%% NEW INTERRUPT NEW INTERRUPT %%%"));
          new_position = true;          
        }
      
      } else {
        position_display();
        Serial.println(F("%%% FASLE POSITION INTERRUPT %%%"));
      }
      last_position_int = num_interrupts;   // always reset this when we have processed thorugh our options for new interrupts
    }
  } 

  // check for on_off_switch push
  num_interrupts = on_off_dev->get_num_interrupts();
  if (num_interrupts > last_on_off_int) {
    if (on_off_dev->get_pin_state() == INTERRUPT_DEV_ON) {
      new_on_off = true;
    } else {
      on_off_dev->display();
      Serial.println(F("%%% FASLE ON_OFF INTERRUPT %%%"));
    }
    last_on_off_int = num_interrupts;
  }

  status();

  // did we get an on off request?
  if (new_on_off) {
    Serial.println(F("%%% NEW ON_OF %%%"));
    new_on_off = false;   // turn off the switch
    if (is_active) {
      zdimmer->blink(2);
      stop();
    } else {
      zdimmer->blink(1);
      start();
    }
  }

  switch (cur_step->waiting_for) {
    case position: 
              if (new_position) {
                Serial.println(F("%%% NEW POSITION REACHED %%%"));
                new_position = false;   // turn off the switch
                next_step();               
              }
              break;

    case duration: 
              if (cur_ms > timed_step_delay_until_ms) {
                Serial.print(F("%%% STEP DURATION REACHED %%% cur_ms="));
                Serial.print(cur_ms);
                Serial.print(F(", timed_step_delay_until_ms="));
                Serial.println(timed_step_delay_until_ms);
                Serial.println(F("Current Step Info: -------------"));
                status(true);
                Serial.println(F("End Current Step Info ----------"));
                next_step();
              }
              break;
              
    case on_off:
              // handled above in global on_off switch
              break;
  }

  // apply any changes to the dimmer if it is time.
  adjust_dimmer(); 
  
}

/* 
 *  start the device and put it into active mode
 */
void Playbook::start() {

  Serial.println(F("%%% Playbook::start() STARTING %%%"));

  new_on_off = false;
  new_position = false;
  last_on_off_int = on_off_dev->get_num_interrupts();
  last_position_int=0;
  last_position_int = get_sum_interrupts(positions, num_positions); 
  is_active = true;                       // we are processing active steps
  cur_step_list = active_steps;           // set a pointer to the active (not inactive) list of steps
  cur_step = cur_step_list;               // set a pointer to the current step
  
  activate_current_step();
}

/*
 * stop the device, put it in inactive mode
 */
void Playbook::stop() {
  
  Serial.println(F("%%% Playbook::stop() STOPPING %%%"));

  new_on_off = false;
  new_position = false;
  last_on_off_int = on_off_dev->get_num_interrupts();
  last_position_int=0;
  last_position_int = get_sum_interrupts(positions, num_positions); 
  is_active = false;                  // we are processing active steps
  cur_step_list = inactive_steps;     // this is a start, so we will be executing off the active_steps list
  cur_step = cur_step_list;          // this_step points to the first step in the current_step_list
    
  activate_current_step();

}

void Playbook::next_step() {

  Serial.println(F("%%% Playbook::next_step() Starting New Step %%%"));

  uint8_t next_step = cur_step->next_step_id;
  cur_step = cur_step_list + next_step;    // set the cur_step to the next step in the list, could wrap around to the beginning
  activate_current_step();
  
}


/*
 * load the step instructions into the class model values to start executing this step
 */
void Playbook::activate_current_step() {

  Serial.println(F("END Playbook::activate_current_step() pre status"));
//  display_step(cur_step);
//  status(true);
  
  cur_step_lights = cur_step->dimmers;     // set a pointer to the current list of light definitions

  dimm_cycle_end_ms = cur_ms + cur_step->dimm_length_ms;  // this is when each light should reach the end of it's cycle

  skips_left = cur_step->num_skip_interrupts;

  // setup the next dimming delay time
  // dimm_delay_until_ms = cur_ms + cur_step->dimm_delay_ms;
  for (uint8_t i=0; i<num_lights; i++) {
    lights[i]->dimm_target = constrain(cur_step_lights[i]->target_level, cur_step_lights[i]->dimm_max, cur_step_lights[i]->dimm_min);
    lights[i]->dimm_direction = cur_step_lights[i]->dimm_direction_init;
    lights[i]->is_oscellating = cur_step_lights[i]->is_oscellating;
    lights[i]->set_dimm_delay_size_ms(cur_step->dimm_length_ms);
    if (i==0) {
      Serial.println(F(""));
      Serial.print(F("activate_current_step i=0: dimm_target="));
      Serial.println(lights[i]->dimm_target);
    }
  }
  
  // setup the wait for position interrups
  if (cur_step->timed_step_duration_ms > 0) { delay_pos_waiting = true; } else { delay_pos_waiting = false; }
  delay_pos_int_until_ms = cur_ms + MOTOR_START_INTERRUPT_IGNORE_MS;
  
  // setup up the amount of time to delay when using wait_for = duration
  timed_step_delay_until_ms=cur_ms;         // assume 0 wait
  if (cur_step->waiting_for == duration) {  // extend the wait for duration steps
    timed_step_delay_until_ms = timed_step_delay_until_ms + cur_step->timed_step_duration_ms;
  }

  // start the motor if called for
  if (cur_step->motor_on) {
    motor->start();
  } else {
    motor->stop();
  }

  
  Serial.println(F("END Playbook::activate_current_step() post status"));
//  display_step(cur_step);
//  status(true);
  Serial.println(F("END Playbook::activate_current_step()"));
}

/*
 *  adjust the level value for each dimmer in the lights, then setup the next dimmer time, check for dimmers that need to be cycled
 */
void Playbook::adjust_dimmer() {

  for (uint8_t i=0; i<num_lights; i++) {

    lights[i]->adjust_dimmer(cur_step_lights[i], cur_step);

  }
}

/*
 * Display active information about this playbook
 */
void Playbook::display() {

  Serial.println(F("Playbook::display() Playbook: "));
  Serial.print(F("    num_lights: "));
  Serial.println(num_lights);
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
  Serial.println(F(""));
  for (uint8_t i=0; i<num_lights; i++) {
    lights[i]->display();
  }
  for (uint8_t i=0; i<num_positions; i++) {
    positions[i]->display();
  }
  on_off_dev->display();
  motor->display();
  display_step(cur_step);
}



   
/*
 * used to dump the configuration of the dimmer at setup time, helps validate the structure is correct. 
 */

void Playbook::position_display() {
  for (uint8_t i=0; i<num_positions; i++) {
    positions[i]->display();
  }
} 

void Playbook::status(bool do_print=false) {

  if (cur_ms_wrap) last_status_ms = cur_ms;
  if (do_print == false && cur_ms - last_status_ms < status_delay_ns) return;
  if (cur_ms - last_status_ms >= status_delay_ns) last_status_ms = cur_ms;

  Serial.println(F(""));
  Serial.print(F("Playbook::status(): "));
  Serial.print(F(" is_active="));
  Serial.print(is_active);
  Serial.print(F(", new_position="));
  Serial.print(new_position);
  Serial.print(F(", new_on_off="));
  Serial.print(new_on_off);
  Serial.print(F(", on_of_dev_interrupts="));
  Serial.print(on_off_dev->get_num_interrupts());
  Serial.print(F(", get_sum_interrupts(positions)="));
  Serial.print(get_sum_interrupts(positions, num_positions));
  Serial.print(F(", skips_left="));
  Serial.print(skips_left);
  Serial.println(F(""));

  Serial.print(F("    cur_step->dimm_length_ms="));
  Serial.println(cur_step->dimm_length_ms);
  Serial.print(F("    delay_pos_int_until_ms="));
  Serial.println(delay_pos_int_until_ms);
  Serial.print(F("    pos_int_delay_left="));
  Serial.println( cur_ms > delay_pos_int_until_ms > 0 ? 0 : delay_pos_int_until_ms - cur_ms);
  Serial.print(F("    delay_pos_waiting="));
  Serial.println(delay_pos_waiting);
  Serial.print(F("    timed_step_delay_until_ms="));
  Serial.print(timed_step_delay_until_ms);
  Serial.print(F("    timed_step_delay_left="));
  Serial.println(cur_ms > timed_step_delay_until_ms ? 0 : timed_step_delay_until_ms - cur_ms);
  
  Serial.print(F("    cur_step info: step_id="));
  Serial.print(cur_step->step_id);
  Serial.print(F(", waiting_for="));
  Serial.println(WAIT_FOR_STRING[cur_step->waiting_for]);

  motor->display();
  on_off_dev->display();
  // position devices display
  for (uint8_t i=0; i<num_positions; i++) {
    positions[i]->display();  
  }  
  
  for (uint8_t i=0; i<num_lights; i++) {
    lights[i]->display();
  }
  zdimmer->display();
  display_step(cur_step);
}

#endif
