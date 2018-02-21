// this code was used in a arduion nano
// to dimm 1-6 strings of lights
// the lights are hooked to PMW pins
// via a mosfet.
// This dimmer is configred
// with a timer that consecutive 
// dims lights up and or down when a certain amount
// of time has passed.
// each light has a home configuration
// then a new configuration to be used after
/// a certain amount of time has passed

#define SERIAL_SPEED  250000
#define IS_BRIEF True
// #define IS_CHATTY True 

// these are our dimmer pins on an UNO or Nano
#define PMW1 3
#define PMW2 5
#define PMW3 6
#define PMW4 9
#define PMW5 10
#define PMW6 11

#define DANCER_PIN A0

boolean is_dancing = false;       // is someone currently dancing
//unsigned long g_current_ms;         // the current ms for the system, computed off of the micros counter (more accurate);

#define PRINT_EVER_NTH_ITTER 50000
int num_loops = 1;          // used to limit prints 
boolean do_print = false;   // used to limit prints

// only print every so often, becaus it is really to much every loop...
void slow_down_prints() {
  num_loops += 1;
  if ((num_loops % PRINT_EVER_NTH_ITTER) == 0) {
    do_print = true;
    num_loops = 1; // stop roll over
  } else {
    do_print = false;
  }
}

String bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

class Dimmer {
  public:
    void status(unsigned long current_ms);
    void init_values();
    void reset(unsigned long current_ms);
    void start(unsigned long current_ms);
    void update(unsigned long current_ms);

  Dimmer( String          p_dimmer_name,
          unsigned short  p_pmw_pin, 
          unsigned long    p_switch_after_sec, 
          unsigned long    p_dimm_duration_sec,
          unsigned long  p_dimm_start, 
          unsigned long  p_dimm_end) :
    dimmer_name(p_dimmer_name),
    pmw_pin(p_pmw_pin),
    ms_switch_after(p_switch_after_sec * 1000),
    ms_dimm_duration(p_dimm_duration_sec * 1000),
    dimm_start(p_dimm_start),
    dimm_end(p_dimm_end)
    {   
        pinMode(pmw_pin, OUTPUT);
        analogWrite(pmw_pin, 0);
        unsigned int num_steps = max(dimm_start, dimm_end) - min(dimm_start, dimm_end);
        if (num_steps == 0) { num_steps = 1; } // stop divide by zero problems;
        ms_step_duration = ms_dimm_duration / num_steps;
        // Calculate the R variable (only needs to be done once at setup)
        // from this article: https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms/
        R = (dimmer_total_steps * log10(2))/(log10(255));
        reset(millis());
        adjust_power();
    }

  private:
    // non-changing values
    const unsigned short dimmer_total_steps = 100;  // total number of dimmer steps from off to on
    String          dimmer_name;       // the name for this dimmed light source
    unsigned short  pmw_pin;           // which pin are we controlling, should be a pmw enabled pin
    unsigned long   ms_switch_after;   // switch state after this many milliseconds 1 second = 1000 milliseconds
    unsigned long   ms_dimm_duration;  // total lenght of dimm start->finish
    unsigned short  dimm_start;        // dimmer level starting value 0-100, 0=off
    unsigned short  dimm_end;          // dimmer level end level, after time trips, 0-100, 0=off
    unsigned long   ms_step_duration;  // the lenght of each step in ms
    double          R;                 // the graph value for this brightness interval;

    // changing status
    unsigned short  dimm_target = 0;     // this is where we are headed to
    unsigned short  dimm_current = 0;    // current pmw value
    unsigned short  pmw_current = 0;    // the current pmw value from our curve function;
    boolean         is_running = false; // the clock is running, starting counting off the time to switch to end_pmw
    unsigned long   ms_last_update = 0; // number of milliseconds when we last updated 
    unsigned long   ms_start_diff = 0;  // number of seconds since this time was started
    unsigned long   dimmer_start_ms = 0; // the time we started this run.
    boolean         dimmer_tripped = false;  // has this dimmer been tripped yet.
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

void Dimmer::adjust_power() {
      pmw_current = pow (2, (dimm_current / R)) - 1; 
      analogWrite(pmw_pin, pmw_current);
}

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
      // compute the new pmw percentage
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

// values used to init our dimmer
void Dimmer::init_values() {
  Serial.print(F("Dimmer_INIT:: dimmer_name: "));
  Serial.print(dimmer_name);
  Serial.print(F(", pin: "));
  Serial.print(pmw_pin);
  Serial.print(F(", ms_switch_after: "));
  Serial.print(ms_switch_after);
  Serial.print(F(", ms_dimm_duration: "));
  Serial.print(ms_dimm_duration);
  Serial.print(F(", dimm_start: "));
  Serial.print(dimm_start);
  Serial.print(F(", dimm_end: "));
  Serial.print(dimm_end);
  Serial.print(F(", ms_step_duration: "));
  Serial.print(ms_step_duration);
  Serial.print(F(", R: "));
  Serial.println(R,5);
}

// repeating status we can print
void Dimmer::status(unsigned long current_ms) {
  if (!do_print) {
    return;
  }
  Serial.print(F("Dimmer_status:: dimmer_name: "));
  Serial.print(dimmer_name); 
  Serial.print(F(", current_ms: "));
  Serial.print(current_ms);
  Serial.print(F(", dimm_target: "));
  Serial.print(dimm_target);
  Serial.print(F(", dimm_current: "));
  Serial.print(dimm_current);
  Serial.print(F(", pmw_current: "));
  Serial.print(pmw_current);
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


// initialize the different dimmers 
Dimmer past_clock("past_clock", PMW1, 15, 6, 0,    100);
Dimmer past_pend("past_pend",   PMW2, 15, 6, 100,  0);

Dimmer present_clock("present_clock",  PMW3, 30, 6, 0,    100);
Dimmer present_pend("present_pend",    PMW4, 30, 6, 100,  0);

Dimmer future_clock("future_clock", PMW5, 60, 6, 0,   100);
Dimmer future_pend("future_pend",   PMW6, 60, 6, 100, 0);

// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 6
Dimmer dimmer_list[NUM_DIMMERS] = { 
      past_clock , past_pend, 
      present_clock, present_pend, 
      future_clock, future_pend
      };


void setup() {

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  Serial.println("remoteDimmer.ino");
  
  pinMode(DANCER_PIN, INPUT);
  is_dancing = false;

  // print the initial values once for each dimmer
  for (int i=0; i<NUM_DIMMERS; i++) {
      dimmer_list[i].init_values();
  }

  // reset each dimmer 
  for (int i=0; i<NUM_DIMMERS; i++) {
      dimmer_list[i].reset(millis());
  }
  is_dancing = false;
  
  Serial.println(F("End setup"));
}

// dump the status of the limit pins
void print_status(unsigned long current_ms) {
  
  if (! do_print) {
    return;
  }
  for (int i=0; i<NUM_DIMMERS; i++) {
      dimmer_list[i].status(current_ms);
  }

}

/*
 * check the network to see if we any new directions 
 * from the director
 */
void check_for_direction(unsigned long current_ms) {

  #if defined IS_CHATTY
    if (do_print) {
      Serial.println(F("checking for direction..."));      
    }
  #endif

  int remote_is_dancing = digitalRead(DANCER_PIN);

  if (is_dancing && remote_is_dancing == 0) {
    // stop dancing
    #if defined IS_BRIEF
      Serial.print(F("DANCE_STOPPED: current_ms: "));
      Serial.println(current_ms);
    #endif
    for (int i=0; i<NUM_DIMMERS; i++) {
      dimmer_list[i].reset(current_ms);
    }
    is_dancing = false;
  } else if (!is_dancing && remote_is_dancing == 1 ) {
    // start dancing
    #if defined IS_BRIEF
      Serial.print(F("DANCE_STARTED: current_ms: "));
      Serial.println(current_ms);
    #endif
    for (int i=0; i<NUM_DIMMERS; i++) {
      dimmer_list[i].start(current_ms);
    }
    is_dancing = true;
  }
}

void status(unsigned long current_ms) {

  if (!do_print) {
    return;
  }
  
  Serial.print(F("Dancer::status() is_dancing: "));
  Serial.print(is_dancing);
  Serial.print(F(", g_current_ms: "));
  Serial.println(current_ms);
  // print a status for each dimmer too..
  for (int i=0; i<NUM_DIMMERS; i++) {
    dimmer_list[i].status(current_ms);
  }
  
}

// start and stop dancers, keep the show moving along....
void loop() {

  unsigned long g_current_ms;
  
  g_current_ms = millis();   // get the current milliseconds

  // set global to control printer very nth itteration
  slow_down_prints();

  // is the remote dancing or not? (start/stop local timers)
  check_for_direction(g_current_ms);

  // update the power numbers for internal dimmers
  for (int i=0; i<NUM_DIMMERS; i++) {
    dimmer_list[i].update(g_current_ms);
  }

  // numbers after check and update
  status(g_current_ms);

}

