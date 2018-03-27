#ifndef AAB_PASTPRESENTFUTURE_H
#define AAB_PASTPRESENTFUTURE_H

#include </Users/jrule/Documents/Arduino/remoteDimmer/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/Dancer.h>

#include </Users/jrule/Documents/Arduino/remoteDimmer/AAB_pastpresentfuture_config.h>

// control the print speed
PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

// initialize the different dimmers 
// these were the dimmers for nostay (the book shelf)
  
#define NUM_DIMMER_STATES 4

// with these dimmers basically the third dimmer never happens
// they delay is much longer than the music so the dimmer never happens
// when the music finishes we reset to step 1 and do not start the 
// clock until the music starts; Step 1 is our steady state state when not playing.

////////////////////////// 
////// PAST DIMMERS //////
////////////////////////// 
  
  // past pendulum dimmer steps
  DimmerStep *past_pend_steps[NUM_DIMMER_STATES] = { 
      new DimmerStep(
              "pp_init",       // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait is running
              ),
      new DimmerStep(
              "pp_down1",      // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_target           The value between dimm_min/max where you want to end
              33,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
      new DimmerStep(
              "pp_up2",        // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              231,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
               ),
      new DimmerStep(
              "pp_down2",      // step_name          The name of this dimmer for debug
              DIMMER_ON,             // dimm_target           The value between dimm_min/max where you want to end
              60*60,                  // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ) 
  };

  // past pendulum dimmer
  Dimmer past_pend(
      "past_pend",          // p_dimmer_name
      PMW2,                 // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      past_pend_steps,      // list of dimmer states
      NUM_DIMMER_STATES     // number of states in the lsit
      );
      

  // past object steps
  DimmerStep *past_object_steps[NUM_DIMMER_STATES] = {
      new DimmerStep(
              "po_init",     // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_target           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait is running
              ),
      new DimmerStep(
              "po_up1",      // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              38,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
      new DimmerStep(
              "po_down2",    // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_target           The value between dimm_min/max where you want to end
              225,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
      new DimmerStep(
              "po_up2",      // step_name          The name of this dimmer for debug
              DIMMER_OFF,              // dimm_target           The value between dimm_min/max where you want to end
              60*60,                  // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              )   
  };

  // past object dimmer 
  Dimmer past_object(
      "past_object",      // p_dimmer_name
      PMW1,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      past_object_steps,  // list of dimmer states
      NUM_DIMMER_STATES   // number of dimmer states in the list
      );


///////////////////////////////
//////// PRESENT DIMMERS //////
///////////////////////////////

  // present pendulum dimmer states 
  DimmerStep *present_pend_steps[NUM_DIMMER_STATES] = { 
      new DimmerStep(
              "prp_init",    // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_start         The value between dimm_min/max where you want to start 
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait is running
              ),
      new DimmerStep(
              "prp_down1",   // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_start         The value between dimm_min/max where you want to start 
              90,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
      new DimmerStep(
              "prp_up2",     // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              231,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
      new DimmerStep(
              "prp_down2",   // step_name          The name of this dimmer for debug
              DIMMER_ON,             // dimm_end           The value between dimm_min/max where you want to end
              60*60,                   // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ) 
  };

  // present pendulum dimmer
  Dimmer present_pend(
      "present_pend",     // p_dimmer_name
      PMW4,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      present_pend_steps, // list of dimmer states
      NUM_DIMMER_STATES   // number of states in the list
      );

  // present object dimmer states
  DimmerStep *present_object_steps[NUM_DIMMER_STATES] = {
      new DimmerStep(
              "pro_init",    // step_name          The name of this dimmer for debug
              DIMMER_OFF,               // dimm_target           The value between dimm_min/max where you want to end
              0,                        // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              false                     // wait is running
              ),
      new DimmerStep(
              "pro_up1",     // step_name          The name of this dimmer for debug
              DIMMER_ON,                // dimm_target           The value between dimm_min/max where you want to end
              96,                       // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait is running
              ),
      new DimmerStep(
              "pro_down2",   // step_name          The name of this dimmer for debug
              DIMMER_OFF,               // dimm_target           The value between dimm_min/max where you want to end
              225,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,        // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait is running
              ),
      new DimmerStep(
              "pro_up2",     // step_name          The name of this dimmer for debug
              DIMMER_OFF,                // dimm_target           The value between dimm_min/max where you want to end
              60*60,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait is running
              )   
  };

  // present object pendulum
  Dimmer present_object(
      "present_object",     // p_dimmer_name
      PMW3,                 // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      present_object_steps,  // list of dimmer states
      NUM_DIMMER_STATES     // number of states in this dimmer list
      );

//////////////////////////
//// FUTURE DIMMERS //////
//////////////////////////

  // future pendulum dimmer steps
  DimmerStep *future_pend_steps[NUM_DIMMER_STATES] = { 
      new DimmerStep(
              "fp_init",     // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_end           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait for running
              ),
      new DimmerStep(
              "fp_down1",               // step_name          The name of this dimmer for debug
              DIMMER_OFF,               // dimm_end           The value between dimm_min/max where you want to end
              145,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait for running
              ),
      new DimmerStep(
              "fp_up2",      // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_end           The value between dimm_min/max where you want to end
              231,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait for running
              ),
      new DimmerStep(
              "fp_down2",    // step_name          The name of this dimmer for debug
              DIMMER_ON,             // dimm_end           The value between dimm_min/max where you want to end
              60*60,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait for running
              ) 
  };

  // future pendulum dimmer
  Dimmer future_pend(
      "future_pend",      // p_dimmer_name
      PMW6,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      future_pend_steps,  // list of dimmer states
      NUM_DIMMER_STATES   // number of states in this dimmer list
      );

  // future object dimmer steps
  DimmerStep *future_object_steps[NUM_DIMMER_STATES] = {
      new DimmerStep(
              "fo_init",   // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_end           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait_is_running
               ),
      new DimmerStep(
              "fo_up1",    // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_end           The value between dimm_min/max where you want to end
              152,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait_is_running
               ),
      new DimmerStep(
              "fo_down2",  // step_name          The name of this dimmer for debug
              DIMMER_OFF,                     // dimm_end           The value between dimm_min/max where you want to end
              225,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
      new DimmerStep(
              "fo_up2",                   // step_name          The name of this dimmer for debug
              DIMMER_OFF,                 // dimm_end           The value between dimm_min/max where you want to end
              60*60,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,        // dimm_duration_sec  The amount of time to take between start and end dimming
              true                        // wait is running
              )   
  };

  // future object dimmer
  Dimmer future_object(
      "future_object",      // p_dimmer_name
      PMW5,                 // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      future_object_steps,  // list of dimmer states
      NUM_DIMMER_STATES     // number of states in this dimmer list
      );

// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 6
Dimmer dimmer_list[NUM_DIMMERS] = { 
            past_object, past_pend, 
            present_object, present_pend, 
            future_object, future_pend
            };

Dancer *my_dancer = new Dancer (    
                          "PastPresentFuture",         // dancer_name      name this dancer
                          DANCER_PIN,       // dance_pin        pin to read if remote is dancing
                          dimmer_list,      // dimmer_list      list of dimmer strings to control
                          NUM_DIMMERS       // num_dummers      number of dimmers in the list
                          );
                          
  
void setup()
{
  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("Start setup"));
  Serial.println(F("remoteDimmer.ino"));

  // print the setup vaues for the timer and get set it's first value
  print_timer->init_values();
  print_timer->print_now();

  // print the setup values for the dancer
  my_dancer->init_values();
  
  // print the setup values for each dimmer
  for (int i=0; i<NUM_DIMMERS; i++) {
    dimmer_list[i].init_values();
  }

  // print the dimmer status as we star
  print_timer->print_now();

  // print the dancer and dimmer status
  my_dancer->status(print_timer);
 
  // dump the pwm tables (there was a bug) for each dimmer
  for (int i=0; i<NUM_DIMMERS; i++) {
    dimmer_list[i].dump_pwm_values();
  }  
  Serial.println(F("End setup")); 
}

#endif

