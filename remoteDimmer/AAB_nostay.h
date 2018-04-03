#ifndef AAB_NOSTAY_H
#define AAB_NOSTAY_H

#include "PrintTimer.h"
#include "Dimmer.h"
#include "Dancer.h"

#include "AAB_nostay_config.h"

// control the print speed
PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

#define NUM_DIMMER_STATES 3
// past pendulum dimmer steps
DimmerStep *notstay_steps[NUM_DIMMER_STATES];

// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 1
Dimmer *dimmer_list[NUM_DIMMERS];

Dancer *my_dancer;  

void setup()
{
  int i;
  
  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("Start setup"));
  Serial.println(F("remoteDimmer.ino"));
  Serial.println(F("NOSTAY CONFIG"));

  i=0;
  notstay_steps[i++] = new DimmerStep(
            "ns_init",                  // step_name          The name of this dimmer for debug
            NOSTAY_DIMM,                // dimm_target           The value between dimm_min/max where you want to end
            0,                          // switch_after_sec   Time to wait after dance starts before we start dimming up
            NOSTAY_DIMM_CHANGE_SECONDS_SHORT, // dimm_duration_sec  The amount of time to take between start and end dimming
            false                       // wait is running
            );
    
  notstay_steps[i++] = new DimmerStep(
            "ns_on",                         // step_name          The name of this dimmer for debug
            NOSTAY_BRIGHT,                   // dimm_target           The value between dimm_min/max where you want to end
            0,                               // switch_after_sec   Time to wait after dance starts before we start dimming up
            NOSTAY_DIMM_CHANGE_SECONDS_LONG, // dimm_duration_sec  The amount of time to take between start and end dimming
            true                             // wait is running
            );
  notstay_steps[i++] = new DimmerStep(
            "hold_on",        // step_name          The name of this dimmer for debug
            NOSTAY_BRIGHT,    // dimm_target           The value between dimm_min/max where you want to end
            60*60,            // switch_after_sec   Time to wait after dance starts before we start dimming up
            0,                // dimm_duration_sec  The amount of time to take between start and end dimming
            true              // wait is running
             );
   if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  i=0;
  dimmer_list[i++] = new Dimmer(
    "bookshelf",  // p_dimmer_name        The name of this dimmer for debug
    PMW1,                   // p_pwm_pin            The pwm pin assigned to this dimmer
    NOSTAY_DIMMER_MIN,      // p_dimm_min           The bottom setting on the dimmer scale
    NOSTAY_DIMMER_MAX,      // p_dimm_max           Full on value on the dimmer scale
    notstay_steps,          // dimmer_steps         list of DimmerStep(s) and delays to execute     
    NUM_DIMMER_STATES       // num_dimmer_steps     number of steps in the list, we loop back to step 1 when we get to the end of the list or back to init values when music stops
    );

   if (NUM_DIMMERS != i) {
    Serial.print(F("ERROR: NUM_DIMMERS != i,  NUM_DIMMERS="));
    Serial.print(NUM_DIMMERS);
    Serial.print(F(", i="));
    Serial.println(i);
   }

  // print the setup vaues for the timer and get set it's first value
  print_timer->init_values();
  print_timer->print_now();

  my_dancer = new Dancer (
      "nostay", // dancer_name      name this dancer
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
