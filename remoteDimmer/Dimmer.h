#ifndef DIMMER_H
#define DIMMER_H

#include </Users/jrule/Documents/Arduino/remoteDimmer/DimmerStep.h>

//
//  A dimmer has an initial value, which is set when the dimmer is first activated.
//  It then has a list of dimmer levels and delays.
//  when the 
class Dimmer {
  public:
    void status(PrintTimer *pt);              // dump the current status for this dimmer
    void init_values();                       // dump the CONSTRUCTOR values for this dimmer
    void dump_pwm_values();                   // dump the pwm values (helps with debug)
    void reset(unsigned long current_ms);     // set everyting back to initial state before time is started
    void start(unsigned long current_ms);     // music is started, time to start the timer sequence
    void update(unsigned long current_ms);    // update the values for the timer as time passes
    const char* dimmer_name;                  // the name for this dimmed light source
    
  // p_dimmer_name        The name of this dimmer for debug
  // p_pwm_pin            The pwm pin assigned to this dimmer
  // dimmer_init_target   The initial pwm level for this dimmer when in steady state
  // dimmer_init_seconds  Number of seconds from *DIMMER START* (not the start of this step) to dealy before we adjust dimm level
  // dimmer_min           value to represent 0 PWM value (fully off)
  // dimmer_max           value to represent 0 PWM value (fully off)
  // dimmer_steps         list of DimmerStep(s) and delays to execute
  // num_dimmer_steps     number of steps in the list, we loop back to step 1 when we get to the end of the list or back to init values when music stops
  Dimmer( const char*     dimmer_name,          // dimmer_name          The name of this dimmer
          unsigned short  pwm_pin,              // pwm_pin              The pwm pin used to adjust this dimmer
          unsigned int    dimmer_min,           // dimmer_min           value to represent 0 PWM value (fully off)
          unsigned int    dimmer_max,           // dimmer_min           value to represent 0 PWM value (fully off)
          DimmerStep      **dimmer_steps,       // dimmer_steps         list of DimmerStep(s) and delays to execute
          unsigned int    num_dimmer_steps      // num_dimmer_steps     number of steps in the list, we loop back to step 1 when we get to the end of the list or back to init values when music stops
    ) :
    dimmer_name(dimmer_name),
    pwm_pin(pwm_pin),
    dimmer_min(dimmer_min),
    dimmer_max(dimmer_max),
    dimmer_steps(dimmer_steps),
    num_dimmer_steps(num_dimmer_steps)
    {   
        pinMode(pwm_pin, OUTPUT);
        analogWrite(pwm_pin, 0);
        last_target_level = dimmer_min;   // set current dimmer level to match pwm_pin level 
        dimm_current = 0;               // the current dimmer step value
        pwm_current = 0;                // set the current pwm value 

        // Calculate the R variable (only needs to be done once at setup)
        // from this article: https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
        total_levels = max(dimmer_min, dimmer_max) - min(dimmer_min, dimmer_max);
        R = (total_levels * log10(2))/(log10(255));
 
        // reset back to initial state
        reset(millis());
    }
  
  private:
    // non-changing values
    unsigned short  pwm_pin;                // which pin are we controlling, should be a pwm enabled pin
    unsigned short  dimmer_min;               // the 0th dimmer step (100% off)
    unsigned short  dimmer_max;               // the max dimmer stop (100% on)
    double          R;                      // the graph value for this brightness interval;
    unsigned int    total_levels;           // total number of steps between min_max

    // dimmer list info static info
    DimmerStep **dimmer_steps;            // list of all dimmer steps
    unsigned int  num_dimmer_steps;        // number of dimmers in the list

    // update with each dimmer step switch or reset
    unsigned long   last_target_level;         // the last target level of the dimmer, set on reset and next_dimmer switches
    unsigned long   ms_switch_after;        // switch state after this many milliseconds 1 second = 1000 milliseconds
    unsigned long   ms_dimm_duration;       // total lenght of dimm start->finish
    boolean         wait_is_running;        // do we wait for a running state to continue

    // values update with set_dimm_target() fuction at each dimmer switch or reset
    unsigned short  dimm_target;            // dimmer level ending value values between dimmer_min and dimmer_max
    unsigned long   ms_step_duration;       // the lenght of each step in ms
    unsigned int    num_levels;             // numbers of steps in the dimm progression last_target_level to dimm_end
    
    // updated with dimmer switches and reset
    DimmerStep  *current_dimmer_step;     // pointer to the current dimmer 
    unsigned int dimmer_index;            // index of the current dimmer
    char         *step_name;             // name of the current step 

    // states we go through in order
    // NOTE: we go back to is_running state each time when a new dimmer step is started
    // boolean is_inited;       // set to true after initial dimmer level has been reached
    boolean is_running;     // set to true when music starts playing (is_dancing)
    boolean is_delayed;     // set to true after the delay period has passed
    boolean is_dimmed;     // set to true we have finished dimming   

    // changing status
    unsigned short  dimm_current = 0;       // The current dimm value (step number)
    unsigned short  pwm_current = 0;        // the current pwm value from our curve function;
    unsigned long   ms_last_update = 0;     // number of milliseconds when we last updated 
    unsigned long   ms_start_diff = 0;      // number of milliseconds since this timer was started
    unsigned long   dimmer_start_ms = 0;    // the time we started this run.

    void adjust_power(unsigned long current_ms);          // change the power levels when in a running state
    void next_dimmer();                                   // update to the next dimmer timeout and level
    void set_dimm_target(unsigned int new_dimm_target);   // update the dimmer target value and recompute associated values

};

// set the new dimm target and update the step delay
void Dimmer::set_dimm_target(unsigned int new_dimm_target) {

  last_target_level = dimm_target;
  dimm_target = new_dimm_target;

  // how many steps betwen where we are and where we are going
  num_levels = max(last_target_level, dimm_target) - min(last_target_level, dimm_target);
  // the delay for each step
  if (num_levels == 0) { 
    ms_step_duration = 0; 
  } else {
    ms_step_duration = ms_dimm_duration / num_levels;
  }
   
}

// send the dimmer back to it's start position
void Dimmer::reset(unsigned long current_ms) {  

  // is_inited = false;       // set to true after initial dimmer level has been reached
  is_running = false;     // set to true when music starts playing (is_dancing)
  is_delayed = false;     // set to true after the delay period has passed
  is_dimmed = false;     // set to true when the dimmer delay time has passed

  dimmer_index = num_dimmer_steps;
  next_dimmer();
  
  // put some initial values here
  dimmer_start_ms = current_ms;
  ms_last_update = current_ms;
  ms_start_diff = 0;
}

void Dimmer::next_dimmer() {
  dimmer_index++;
  if (dimmer_index >= num_dimmer_steps) {
    dimmer_index = 0;
  }
  current_dimmer_step = dimmer_steps[dimmer_index];

  // copy the input values for this step int the the dimmer
  last_target_level = dimm_target;
  step_name = current_dimmer_step->step_name;
  ms_switch_after = current_dimmer_step->get_ms_swtch_after();    // set the new total delay before we dimm
  ms_dimm_duration = current_dimmer_step->get_ms_dimm_duration();  // set the new duration for the dimm  up / down
  wait_is_running = current_dimmer_step->get_wait_is_running();
  set_dimm_target(current_dimmer_step->get_dimm_target());
  
  // new delay and new target
  is_delayed = false;     // set to true after the delay period has passed
  is_dimmed = false;     // set to true when the dimmer delay time has passed

}

// send the dimmer back to it's start position
void Dimmer::start(unsigned long current_ms) {
  dimmer_start_ms = current_ms;
  ms_last_update = current_ms;
  is_running = true; 
  Serial.print(F("Dimmer::start dimmer_name="));
  Serial.println(dimmer_name);
  // just start with the first step when starting...
}

// set the new power pwm_number
void Dimmer::adjust_power(unsigned long current_ms) {
  
  unsigned int pwm_new;

  // need to wait longer....
  if ((current_ms - ms_last_update) < ms_step_duration) { return; }

  ms_last_update = current_ms;
        
  // compute the new pwm percentage
  if (dimm_target > dimm_current) {  // going up....
    dimm_current++;
  } else {  // going down
    dimm_current--;
  }

  pwm_new = pow (2, (dimm_current / R)) - 1;
  
  // are we at the top of or dimming scale, override pwm to 255
  if (dimm_current == dimmer_max) {
    if (pwm_new != 255) {
      Serial.print(F("Dimmer::adjust_power() reached dimmer_max, power_curve_val="));
      Serial.print(pwm_new);
      Serial.println(F(", overidding to pwm_new=255"));
      pwm_new = 255; // somtimes the formula comes just shy of fully on, we don't want that 
    }
  } 

  // are we are the bottom of our dimming scale, override pwm to 0
  if (dimm_current == dimmer_min) {
    if (pwm_new != 0) {
      Serial.print(F("Dimmer::adjust_power() reached dimmer_min, power_curve_val="));
      Serial.print(pwm_new);
      Serial.println(F(", overidding to pwm_new=0"));
      pwm_new = 0;  // somtimes the formula comes just shy of fully off, we don't want that 
    }
  }
  
  if (pwm_new != pwm_current) {
    Serial.print(F("Dimmer::adjust_power() light level changing dimm_current="));
    Serial.print(dimm_current);
    Serial.print(F(", old_pwm="));
    Serial.print(pwm_current);
    Serial.print(F(", new_pwm="));
    Serial.print(pwm_new);
    Serial.print(F(", ms_step_duration="));
    Serial.println(ms_step_duration);
  }
  
  pwm_current = pwm_new;
  
  analogWrite(pwm_pin, pwm_current);
}

// update new power levels as time passes
void Dimmer::update(unsigned long current_ms) {
      
  if (ms_last_update > current_ms) { ms_last_update = 0; } // ms wrapped around, just reset it to 0
  if (dimmer_start_ms > current_ms) { dimmer_start_ms = 0; } // ms wrapped around, just reset it to 0

  // current step needs running so we wait...
  if (!is_running && wait_is_running) {return;}
  
  // has our timer passed; switch the lighting level
  if (!is_delayed) {
    ms_start_diff = current_ms - dimmer_start_ms;
    if (ms_start_diff >= ms_switch_after) {
      #ifdef IS_BRIEF
        Serial.print(F("DELAY COMPLETED: starting to DIMM: "));
        Serial.print(dimmer_name);
        Serial.print(F(", ms_switch_after: "));
        Serial.print(ms_switch_after);
        Serial.print(F(", ms_start_diff: "));
        Serial.println(ms_start_diff);
      #endif
      // set the new dimmer target
      is_delayed = true;
    }
  }
    
  // if we are not at target, is it time to do a step...
  if (is_delayed && !is_dimmed) {
    // adjust power
    if (dimm_target != dimm_current) {
        adjust_power(current_ms);
    } 

    // we are done, time to move on to the next step
    if (dimm_target == dimm_current) {
      Serial.println(F("DIMM TARGET REACHED"));
      is_dimmed = true; 
      next_dimmer();
      Serial.print(F("   NEW_DIMMER_STEP: "));
      current_dimmer_step->init_values();
      // if we looped around reset the dimmer clock
      if (dimmer_index == 0) {
        start(current_ms);
      }
    }
  }
}
    
// dump values used to init our dimmer
void Dimmer::init_values() {
  Serial.print(F("Dimmer_INIT:: dimmer_name="));
  Serial.print(dimmer_name);
  Serial.print(F(", pwm_pin="));
  Serial.print(pwm_pin);
  Serial.print(F(", dimmer_min="));
  Serial.print(dimmer_min);
  Serial.print(F(", dimmer_max="));
  Serial.print(dimmer_max);
  Serial.print(F(", total_levels="));
  Serial.print(total_levels);
  Serial.print(F(", R="));
  Serial.print(R,5);
  Serial.print(F(", num_dimmer_steps="));
  Serial.println(num_dimmer_steps);
  
  for (int i=0; i<num_dimmer_steps; i++) {
    Serial.print("    ");
    dimmer_steps[i]->init_values();
  }
}

// repeating status we can print
void Dimmer::status(PrintTimer *pt) {
  if (!pt->get_do_print()) {
    return;
  }
  Serial.print(F("Dimmer_status:: dimmer_name: "));
  Serial.print(dimmer_name); 
  Serial.print(F(", current_ms: "));
  Serial.print(pt->current_millis);
  Serial.print(F(", dimm_target: "));
  Serial.print(dimm_target);
  Serial.print(F(", dimm_current: "));
  Serial.print(dimm_current);
  Serial.print(F(", pwm_current: "));
  Serial.print(pwm_current);
  Serial.print(F(", ms_switch_after: "));
  Serial.print(ms_switch_after);
  Serial.print(F(", ms_start_diff: "));
  Serial.print(ms_start_diff);
  Serial.print(F(", ms_last_update: "));
  Serial.print(ms_last_update);
  Serial.print(F(", dimmer_start_ms: "));
  Serial.println(dimmer_start_ms);

  Serial.print(F("    num_levels="));
  Serial.print(num_levels);
  Serial.print(F(",  ms_step_duration="));
  Serial.print(ms_step_duration);
  Serial.print(F(", last_target_level="));
  Serial.print(last_target_level);
  Serial.print(F(", ms_dimm_duration="));
  Serial.print(ms_dimm_duration);
  Serial.print(F(", wait_is_running="));
  Serial.println(wait_is_running);
  
  Serial.print(F("    is_running="));
  Serial.print(bool_tostr(is_running));
  Serial.print(F(", is_delayed="));
  Serial.print(bool_tostr(is_delayed));
  Serial.print(F(", is_dimmed="));
  Serial.print(bool_tostr(is_dimmed));
  Serial.print(F(", step_name="));
  Serial.println(step_name); 
}



void Dimmer::dump_pwm_values() {
  unsigned short new_pwm;
  Serial.print(F("Dimmer::dump_pwm_values() dimmer_name="));
  Serial.println(dimmer_name);
  for (int i=dimmer_min; i<=dimmer_max; i++) {
    new_pwm=pow(2, (i / R)) - 1; 
    Serial.print(F("Step# "));
    Serial.print(i);
    Serial.print(F(", pwm="));
    Serial.println(new_pwm);
  }
}

#endif
