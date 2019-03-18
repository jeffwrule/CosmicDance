#ifndef AAB_PASTPRESENTFUTURE_H
#define AAB_PASTPRESENTFUTURE_H

#include "PrintTimer.h"
#include "Dimmer.h"
#include "Dancer.h"
#include "PWMPin.h"

#include "AAB_pastpresentfuture_config.h"

// control the print speed
PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

// initialize the different dimmers 
// these were the dimmers for nostay (the book shelf)
  
#define NUM_DIMMER_STATES 4
DimmerStep *past_pend_steps[NUM_DIMMER_STATES]; 
DimmerStep *past_object_steps[NUM_DIMMER_STATES]; 

DimmerStep *present_pend_steps[NUM_DIMMER_STATES]; 
DimmerStep *present_object_steps[NUM_DIMMER_STATES]; 

DimmerStep *future_pend_steps[NUM_DIMMER_STATES]; 
DimmerStep *future_object_steps[NUM_DIMMER_STATES]; 

// with these dimmers basically the third dimmer never happens
// they delay is much longer than the music so the dimmer never happens
// when the music finishes we reset to step 1 and do not start the 
// clock until the music starts; Step 1 is our steady state state when not playing.


  
// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 6
Dimmer *dimmer_list[NUM_DIMMERS];
Dancer *my_dancer;  

PWMPin *my_pin1;      
PWMPin *my_pin2;      
PWMPin *my_pin3;      
PWMPin *my_pin4;      
PWMPin *my_pin5;      
PWMPin *my_pin6;      
  
void setup()
{
  int i;
  int j;
  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("Start setup"));
  Serial.println(F("remoteDimmer.ino"));
  Serial.println(F("PASTPRESENTFUTURE CONFIG"));


  // print the setup vaues for the timer and get set it's first value
  print_timer->init_values();
  print_timer->print_now();

  ////////////////////////// 
  ////// PAST DIMMERS //////
  ////////////////////////// 

  i=0;
  // past pendulum dimmer steps
  past_pend_steps[i++] = new DimmerStep(
              "pp_init",       // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait is running
              );
  past_pend_steps[i++] = new DimmerStep(
              "pp_down1",      // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_target           The value between dimm_min/max where you want to end
              33,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );
  past_pend_steps[i++] = new DimmerStep(
              "pp_up2",        // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              231,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
               );
  past_pend_steps[i++] = new DimmerStep(
              "pp_down2",      // step_name          The name of this dimmer for debug
              DIMMER_ON,             // dimm_target           The value between dimm_min/max where you want to end
              60*60,                  // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );
              
   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  my_pin1 = new PWMPin(PWM2);
  j=0;
  // past pendulum dimmer
  dimmer_list[j++] = new Dimmer (
      "past_pend",          // p_dimmer_name
      my_pin1,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      past_pend_steps,      // list of dimmer states
      NUM_DIMMER_STATES     // number of states in the lsit
      );
      

  // past object steps
  i=0;
  past_object_steps[i++] = new DimmerStep(
              "po_init",     // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_target           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait is running
              );
  past_object_steps[i++] = new DimmerStep(
              "po_up1",      // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              38,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
  past_object_steps[i++] = new DimmerStep(
              "po_down2",    // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_target           The value between dimm_min/max where you want to end
              225,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              ),
  past_object_steps[i++] = new DimmerStep(
              "po_up2",      // step_name          The name of this dimmer for debug
              DIMMER_OFF,              // dimm_target           The value between dimm_min/max where you want to end
              60*60,                  // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );  
              
   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  my_pin2 = new PWMPin(PWM1);
  
  // past object dimmer 
  dimmer_list[j++] = new Dimmer (
      "past_object",      // p_dimmer_name
      my_pin2,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      past_object_steps,  // list of dimmer states
      NUM_DIMMER_STATES   // number of dimmer states in the list
      );


  ///////////////////////////////
  //////// PRESENT DIMMERS //////
  ///////////////////////////////

  // present pendulum dimmer states 
  i=0;
  present_pend_steps[i++] = new DimmerStep(
              "prp_init",    // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_start         The value between dimm_min/max where you want to start 
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait is running
              );
  present_pend_steps[i++] = new DimmerStep(
              "prp_down1",   // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_start         The value between dimm_min/max where you want to start 
              90,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );
  present_pend_steps[i++] = new DimmerStep(
              "prp_up2",     // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_target           The value between dimm_min/max where you want to end
              231,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );
  present_pend_steps[i++] = new DimmerStep(
              "prp_down2",   // step_name          The name of this dimmer for debug
              DIMMER_ON,             // dimm_end           The value between dimm_min/max where you want to end
              60*60,                   // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );
              
   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  my_pin3 = new PWMPin(PWM4);
  
  // present pendulum dimmer
  dimmer_list[j++] = new Dimmer (
      "present_pend",     // p_dimmer_name
      my_pin3,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      present_pend_steps, // list of dimmer states
      NUM_DIMMER_STATES   // number of states in the list
      );

  // present object dimmer states
  i=0;
  present_object_steps[i++] = new DimmerStep(
              "pro_init",    // step_name          The name of this dimmer for debug
              DIMMER_OFF,               // dimm_target           The value between dimm_min/max where you want to end
              0,                        // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              false                     // wait is running
              );
  present_object_steps[i++] = new DimmerStep(
              "pro_up1",     // step_name          The name of this dimmer for debug
              DIMMER_ON,                // dimm_target           The value between dimm_min/max where you want to end
              96,                       // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait is running
              );
  present_object_steps[i++] = new DimmerStep(
              "pro_down2",   // step_name          The name of this dimmer for debug
              DIMMER_OFF,               // dimm_target           The value between dimm_min/max where you want to end
              225,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,        // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait is running
              );
  present_object_steps[i++] = new DimmerStep(
              "pro_up2",     // step_name          The name of this dimmer for debug
              DIMMER_OFF,                // dimm_target           The value between dimm_min/max where you want to end
              60*60,                    // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait is running
              );

   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  my_pin4 = new PWMPin(PWM3);
  // present object dimmer
  dimmer_list[j++] = new Dimmer (
      "present_object",     // p_dimmer_name
      my_pin4,                 // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      present_object_steps,  // list of dimmer states
      NUM_DIMMER_STATES     // number of states in this dimmer list
      );

  //////////////////////////
  //// FUTURE DIMMERS //////
  //////////////////////////

  // future pendulum dimmer steps
  i=0;
  future_pend_steps[i++] = new DimmerStep(
              "fp_init",     // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_end           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait for running
              );
      new DimmerStep(
              "fp_down1",               // step_name          The name of this dimmer for debug
              DIMMER_OFF,               // dimm_end           The value between dimm_min/max where you want to end
              145,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                      // wait for running
              );
      new DimmerStep(
              "fp_up2",      // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_end           The value between dimm_min/max where you want to end
              231,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait for running
              );
      new DimmerStep(
              "fp_down2",    // step_name          The name of this dimmer for debug
              DIMMER_ON,             // dimm_end           The value between dimm_min/max where you want to end
              60*60,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait for running
              );

   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  my_pin5 = new PWMPin(PWM6);
  // future pendulum dimmer
  dimmer_list[j++] = new Dimmer (
      "future_pend",      // p_dimmer_name
      my_pin5,               // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      future_pend_steps,  // list of dimmer states
      NUM_DIMMER_STATES   // number of states in this dimmer list
      );

  // future object dimmer steps
  i=0;
  future_object_steps[i++] = new DimmerStep(
              "fo_init",   // step_name          The name of this dimmer for debug
              DIMMER_OFF,             // dimm_end           The value between dimm_min/max where you want to end
              0,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              false                   // wait_is_running
               );
  future_object_steps[i++] = new DimmerStep(
              "fo_up1",    // step_name          The name of this dimmer for debug
              DIMMER_ON,              // dimm_end           The value between dimm_min/max where you want to end
              152,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_UP_SECONDS,    // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait_is_running
               );
  future_object_steps[i++] = new DimmerStep(
              "fo_down2",  // step_name          The name of this dimmer for debug
              DIMMER_OFF,                     // dimm_end           The value between dimm_min/max where you want to end
              225,                     // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,      // dimm_duration_sec  The amount of time to take between start and end dimming
              true                    // wait is running
              );
  future_object_steps[i++] = new DimmerStep(
              "fo_up2",                   // step_name          The name of this dimmer for debug
              DIMMER_OFF,                 // dimm_end           The value between dimm_min/max where you want to end
              60*60,                      // switch_after_sec   Time to wait after dance starts before we start dimming up
              DIMMER_DOWN_SECONDS,        // dimm_duration_sec  The amount of time to take between start and end dimming
              true                        // wait is running
              );

   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  my_pin6 = new PWMPin(PWM5);
  // future object dimmer
  dimmer_list[j++] = new Dimmer (
      "future_object",      // p_dimmer_name
      my_pin6,                 // p_pwm_pin 
      DIMMER_MIN,           // pmw level to represent dimmer off state
      DIMMER_MAX,           // pwm level to represent dimmer fully on state,
      future_object_steps,  // list of dimmer states
      NUM_DIMMER_STATES     // number of states in this dimmer list
      );

   if (NUM_DIMMERS != j) {
    Serial.print(F("ERROR: NUM_DIMMERS != j,  NUM_DIMMERS="));
    Serial.print(NUM_DIMMERS);
    Serial.print(F(", j="));
    Serial.println(j);
   }

  // OUR DANCER TO CONTROL EVERYTING
  my_dancer = new Dancer (    
                    "PastPresentFuture",         // dancer_name      name this dancer
                    DANCER_PIN,       // dance_pin        pin to read if remote is dancing
                    dimmer_list,      // dimmer_list      list of dimmer strings to control
                    NUM_DIMMERS       // num_dummers      number of dimmers in the list
                    );
                    
  // print the setup values for the dancer
  my_dancer->init_values();
  
  // print the setup values for each dimmer
  for (int i=0; i<NUM_DIMMERS; i++) {
    dimmer_list[i]->init_values();
  }

  // print the dimmer status as we star
  print_timer->print_now();

  // print the dancer and dimmer status
  my_dancer->status(print_timer);
 
  // dump the pwm tables (there was a bug) for each dimmer
  for (int i=0; i<NUM_DIMMERS; i++) {
    dimmer_list[i]->dump_pwm_values();
  }  
  Serial.println(F("End setup")); 
}

#endif

