#ifndef DIMMERSTEP_H
#define DIMMERSTEP_H

class DimmerStep {

  public:
    void init_values();
    const char*     step_name;         // the name of this step
    unsigned int get_dimm_target();
    unsigned long get_ms_swtch_after();
    unsigned long get_ms_dimm_duration();
    boolean       get_wait_is_running();

    // step_name          The name of this dimmer for debug
    // dimm_target        Target dimm level for this step
    // switch_after_sec   Time to wait after dance starts before we start dimming up
    // dimm_duration_sec  The amount of time to take between start and end dimming
    // wait_is_running    wait to execute this move until started (allows for indefinate delay in play) 
     DimmerStep(const char*  step_name,           // step_name          The name of this dimmer for debug
            unsigned short  dimm_target,          // dimm_end           The value between dimm_min/max where you want to end
            unsigned long   switch_after_sec,     // switch_after_sec   Time to wait after dance starts before we start dimming up
            unsigned int    dimm_duration_sec,    // dimm_duration_sec  The amount of time to take between start and end dimming
            boolean         wait_is_running       // wait to execute this move until started (allows for indefinate delay in play) 
            ) :
      step_name(step_name),
      dimm_target(dimm_target),
      ms_switch_after(switch_after_sec * 1000),
      ms_dimm_duration(dimm_duration_sec * 1000),
      wait_is_running(wait_is_running)
      { 
      }

  private:
    // non-changing values
    unsigned int    dimm_target;       // target dimm level
    unsigned long   ms_switch_after;   // switch state after this many milliseconds 1 second = 1000 milliseconds
    unsigned long   ms_dimm_duration;  // total lenght of dimm start->finish
    boolean         wait_is_running;   // wait until we are in a running state to start executing this step 
};

unsigned int  DimmerStep::get_dimm_target() { return dimm_target; }
unsigned long DimmerStep::get_ms_swtch_after() {return ms_switch_after; }
unsigned long DimmerStep::get_ms_dimm_duration() { return ms_dimm_duration; }
boolean       DimmerStep::get_wait_is_running() { return wait_is_running; }

void DimmerStep::init_values() {
        Serial.print(F("DimmerStep::init_values() step_name="));
        Serial.print(step_name);
        Serial.print(F(", dimm_target="));
        Serial.print(dimm_target);
        Serial.print(F(", ms_switch_after="));
        Serial.print(ms_switch_after);
        Serial.print(F(", ms_dimm_duration="));
        Serial.print(ms_dimm_duration);  
        Serial.print(F(", wait_is_running="));
        Serial.println(bool_tostr(wait_is_running));  
}

#endif
