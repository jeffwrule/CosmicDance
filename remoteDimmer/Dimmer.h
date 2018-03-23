#ifndef DIMMER_H
#define DIMMER_H

class Dimmer {
  public:
    void status(PrintTimer *pt);
    void init_values();
    void dump_pwm_values();
    void reset(unsigned long current_ms);
    void start(unsigned long current_ms);
    void update(unsigned long current_ms);
    const char* dimmer_name;                // the name for this dimmed light source

  // this entry point assumes the min/max and start/stop are the same
  // it is a fully on or fully dimmer
  // p_dimmer_name        The name of this dimmer for debug
  // p_pwm_pin            The pwm pin assigned to this dimmer
  // p_switch_after_sec   Time to wait after dance starts before we start dimming up
  // p_dimm_duration_sec  The amount of time to take between start and end dimming
  // p_dimm_start         Starting dimmer level 0-255 can dimm up or down
  // p_dimm_end           Ending dimmer leveo 0-255 can dimm up or down
  Dimmer( const char*     p_dimmer_name,
          unsigned short  p_pwm_pin, 
          unsigned long   p_switch_after_sec, 
          unsigned int    p_dimm_duration_sec,
          unsigned short  p_dimm_start, 
          unsigned short  p_dimm_end) :
    dimmer_name(p_dimmer_name),
    pwm_pin(p_pwm_pin),
    ms_switch_after(p_switch_after_sec * 1000),
    ms_dimm_duration(p_dimm_duration_sec * 1000),
    dimm_start(p_dimm_start),
    dimm_end(p_dimm_end),
    dimm_min(p_dimm_start),
    dimm_max(p_dimm_end)
    {   
        pinMode(pwm_pin, OUTPUT);
        analogWrite(pwm_pin, 0);
        num_steps = max(dimm_min, dimm_max) - min(dimm_min, dimm_max);
        total_steps = num_steps;
        ms_step_duration = ms_dimm_duration / num_steps;
        // Calculate the R variable (only needs to be done once at setup)
        // from this article: https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
        R = (total_steps * log10(2))/(log10(255));
        reset(millis());
        // adjust_power();
    }

  // This dimmer allowes the min/max and start/stop to not be the same
  // NOTE: it is probably not a good idea to hold a value not min or max while 
  // the music is play as values between these can cause lots of audio noise
  // p_dimmer_name        The name of this dimmer for debug
  // p_pwm_pin            The pwm pin assigned to this dimmer
  // p_switch_after_sec   Time to wait after dance starts before we start dimming up
  // p_dimm_duration_sec  The amount of time to take between start and end dimming
  // p_dimm_start         The value between dimm_min/max where you want to start
  // p_dimm_end           The value between dimm_min/max where you want to end
  // p_dimm_min           The bottom setting on the dimmer scale
  // p_dimm_max           Full on step on the dimmer scale
  Dimmer( const char*     p_dimmer_name,
          unsigned short  p_pwm_pin, 
          unsigned long   p_switch_after_sec, 
          unsigned int    p_dimm_duration_sec,
          unsigned short  p_dimm_start, 
          unsigned short  p_dimm_end,
          unsigned short  p_dimm_min,
          unsigned short  p_dimm_max) :
    dimmer_name(p_dimmer_name),
    pwm_pin(p_pwm_pin),
    ms_switch_after(p_switch_after_sec * 1000),
    ms_dimm_duration(p_dimm_duration_sec * 1000),
    dimm_start(p_dimm_start),
    dimm_end(p_dimm_end),
    dimm_min(p_dimm_min),
    dimm_max(p_dimm_max)
    {   
        pinMode(pwm_pin, OUTPUT);
        analogWrite(pwm_pin, 0);
        if (dimm_start < dimm_end) {
          if (dimm_start < dimm_min) { dimm_start = dimm_min; }
        } else {
          if (dimm_end < dimm_min) { dimm_end = dimm_min; }
        }

        // bounds checking
        if (dimm_start > dimm_end) {
          if (dimm_start > dimm_max) { dimm_start = dimm_max; }
        } else {
          if (dimm_end > dimm_max) { dimm_end = dimm_max; }
        }
        if (dimm_start == dimm_end) {
          if (dimm_start > dimm_max ) { dimm_start = dimm_end = dimm_max; }
          if (dimm_start < dimm_min ) { dimm_start = dimm_end = dimm_min; }
        }
        
        num_steps = max(dimm_start, dimm_end) - min(dimm_start, dimm_end);
        total_steps = max(dimm_min, dimm_max) - min(dimm_min, dimm_max);
        ms_step_duration = ms_dimm_duration / num_steps;
        // Calculate the R variable (only needs to be done once at setup)
        // from this article: https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
        R = (total_steps * log10(2))/(log10(255));
        reset(millis());
        // adjust_power();
    }
    
  private:
    // non-changing values
    unsigned short  pwm_pin;           // which pin are we controlling, should be a pwm enabled pin
    unsigned long   ms_switch_after;   // switch state after this many milliseconds 1 second = 1000 milliseconds
    unsigned long   ms_dimm_duration;  // total lenght of dimm start->finish
    unsigned short  dimm_min;          // the 0th dimmer step (100% off)
    unsigned short  dimm_max;          // the max dimmer stop (100% on)
    unsigned short  dimm_start;        // dimmer level starting value values between dimm_min and dimm_max
    unsigned short  dimm_end;          // dimmer level ending value values between dimm_min and dimm_max
    unsigned long   ms_step_duration;  // the lenght of each step in ms
    double          R;                 // the graph value for this brightness interval;
    unsigned int    num_steps;         // numbers of steps in the dimm progression dimm_start to dimm_end
    unsigned int    total_steps;       // total number of steps between min_max

    // changing status
    unsigned short  dimm_target = 0;        // The target dimm value (step number)
    unsigned short  dimm_current = 0;       // The current dimm value (step number)
    unsigned short  pwm_current = 0;        // the current pwm value from our curve function;
    boolean         is_running = false;     // the clock is running, starting counting off the time to switch to end_pwm
    unsigned long   ms_last_update = 0;     // number of milliseconds when we last updated 
    unsigned long   ms_start_diff = 0;      // number of seconds since this time was started
    unsigned long   dimmer_start_ms = 0;    // the time we started this run.
    boolean         dimmer_tripped = false; // has this dimmer been tripped yet.
    boolean         in_flux = false;        // are we actively dimming.

    void adjust_power();

};

// send the dimmer back to it's start position
void Dimmer::reset(unsigned long current_ms) {
  dimm_target = dimm_start;
  ms_last_update = current_ms;
  is_running = false;
  dimmer_tripped = false;
  ms_start_diff = 0;
}

// send the dimmer back to it's start position
void Dimmer::start(unsigned long current_ms) {
  dimmer_start_ms = current_ms;
  ms_last_update = current_ms;
  is_running = true; 
}

// set the new power pwm_number
void Dimmer::adjust_power() {
      unsigned int pwm_new = pow (2, (dimm_current / R)) - 1;
      
      // sometimes the formula does not return a full 255 or 0 for the first and last value
      // if we want to override here and full off or fully on the PWM 
      if (dimm_current == max(dimm_min, dimm_max)) {
        Serial.print(F("Dimmer::adjust_power() reached dimm_max, power_curve_val="));
        Serial.print(pwm_new);
        Serial.println(F(", overidding to pwm_new=255"));
        pwm_new = 255; // somtimes the formula comes just shy of fully on, we don't want that 
      } 
      if (dimm_current == min(dimm_min, dimm_max)) {
        Serial.print(F("Dimmer::adjust_power() reached dimm_min, power_curve_val="));
        Serial.println(pwm_new);
        Serial.println(F(", overidding to pwm_new=255"));
        pwm_new = 0;  // somtimes the formula comes just shy of fully off, we don't want that 
      }

      if (pwm_new != pwm_current) {
        Serial.print(F("Dimmer::adjust_power() light level changing dimm_current="));
        Serial.print(dimm_current);
        Serial.print(F(", old_pwm="));
        Serial.print(pwm_current);
        Serial.print(F(", new_pwm="));
        Serial.println(pwm_new);
      }

      pwm_current = pwm_new;
      
      analogWrite(pwm_pin, pwm_current);
}

// update new power levels as time passes
void Dimmer::update(unsigned long current_ms) {
      
  if (ms_last_update > current_ms) { ms_last_update = 0; } // ms wrapped around, just reset it to 0
  
  // has our timer passed; switch the lighting level
  if (is_running) {
    if (!dimmer_tripped) {
      ms_start_diff = current_ms - dimmer_start_ms;
      if (ms_start_diff >= ms_switch_after) {
        #ifdef IS_BRIEF
          Serial.print(F("TIMER TRIPPED: Switching Dimmer state for dimmer: "));
          Serial.print(dimmer_name);
          Serial.print(F(", ms_switch_after: "));
          Serial.print(ms_switch_after);
          Serial.print(F(", ms_start_diff: "));
          Serial.println(ms_start_diff);
        #endif
        dimm_target = dimm_end;
        dimmer_tripped = true;
      }
    }
  }

  if (!in_flux && dimm_target != dimm_current) {
    #ifdef IS_BRIEF
      Serial.print(F("DIMM STARTED: a dimer change has been requested: "));
      Serial.print(dimmer_name);
      Serial.print(F(", start_ms: "));
      Serial.println(current_ms);
    #endif    
    in_flux = true;      
  }
    
  // if we are not at target, is it time to do a step...
  if (dimm_target != dimm_current) {
    if ((current_ms - ms_last_update) >= ms_step_duration) { // we waited long enough for next update...
      #ifdef IS_CHATTY
        Serial.print(F("POWER CHANGE: Chaning Dimmer power for dimmer: "));
        Serial.print(dimmer_name);
        Serial.print(F(", ms_step_duration: "));
        Serial.print(ms_step_duration);
        Serial.print(F(", current delay:"));
        Serial.println(current_ms - ms_last_update);
      #endif    
      // we are doing this now...
      ms_last_update = current_ms;
      // compute the new pwm percentage
      if (dimm_target > dimm_current) {  // going up....
        dimm_current++;
      } else {  // going down
        dimm_current--;
      }
      adjust_power();
    } 
  } 
  
  // dimm level reached, clear in_flux state
  if (in_flux  && dimm_target == dimm_current) {
    #ifdef IS_BRIEF
      Serial.print(F("DIMM COMPLETED: a dimer change has been completed: "));
      Serial.print(dimmer_name);
      Serial.print(F(", end_ms: "));
      Serial.println(current_ms);
    #endif    
    in_flux = false;         
  }
  
}

// dump values used to init our dimmer
void Dimmer::init_values() {
  Serial.print(F("Dimmer_INIT:: dimmer_name: "));
  Serial.print(dimmer_name);
  Serial.print(F(", pwm_pin: "));
  Serial.print(pwm_pin);
  Serial.print(F(", ms_switch_after: "));
  Serial.print(ms_switch_after);
  Serial.print(F(", ms_dimm_duration: "));
  Serial.println(ms_dimm_duration);
  Serial.print(F("    dimm_start: "));
  Serial.print(dimm_start);
  Serial.print(F(", dimm_end: "));
  Serial.print(dimm_end);
  Serial.print(F(", dimm_min: "));
  Serial.print(dimm_min);
  Serial.print(F(", dimm_max: "));
  Serial.print(dimm_max);
  Serial.print(F(", num_steps: "));
  Serial.print(num_steps);
  Serial.print(F(", total_steps: "));
  Serial.print(total_steps);
  Serial.print(F(", ms_step_duration: "));
  Serial.print(ms_step_duration);
  Serial.print(F(", R: "));
  Serial.println(R,5);
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
  Serial.println(ms_start_diff);
  Serial.print(F("                is_running: "));
  Serial.print(bool_tostr(is_running));
  Serial.print(F(", dimmer_tripped: "));
  Serial.print(bool_tostr(dimmer_tripped));
  Serial.print(F(", ms_last_update: "));
  Serial.print(ms_last_update);
  Serial.print(F(", in_flux: "));
  Serial.print(in_flux);
  Serial.print(F(", dimmer_start_ms: "));
  Serial.println(dimmer_start_ms);
}

void Dimmer::dump_pwm_values() {
  unsigned short new_pwm;
  Serial.print(F("Dimmer::dump_pwm_values() dimmer_name="));
  Serial.println(dimmer_name);
  for (int i=dimm_min; i<=dimm_max; i++) {
    new_pwm=pow (2, (i / R)) - 1; 
    Serial.print(F("Step# "));
    Serial.print(i);
    Serial.print(F(", pwm="));
    Serial.println(new_pwm);
  }
}

#endif
