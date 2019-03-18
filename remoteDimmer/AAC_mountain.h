#ifndef MOUNTAIN_H
#define MOUNTAIN_H

#include "PrintTimer.h"
#include "Dimmer.h"
#include "Dancer.h"
#include "PWMSFShield.h"

#include "AAC_mountain_config.h"

// control the print speed
PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

#define NUM_DIMMER_STATES 3
// past pendulum dimmer steps
DimmerStep *light1_steps[NUM_DIMMER_STATES];
DimmerStep *light2_steps[NUM_DIMMER_STATES];
DimmerStep *light3_steps[NUM_DIMMER_STATES];

// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 3
Dimmer *dimmer_list[NUM_DIMMERS];

PWMSFShield *my_pin0;
PWMSFShield *my_pin1;
PWMSFShield *my_pin2;

Dancer *my_dancer;

void setup()
{
  int i=0;
  int j=0;

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("Start setup"));
  Serial.println(F("remoteDimmer.ino"));
  Serial.println(F("MOUNTAIN CONFIG"));

  i = 0;
  light1_steps[i++] = new DimmerStep(
    "mtn_dimm1",               // step_name             The name of this dimmer for debug
    MTN_DIMM,                   // dimm_target          The value between dimm_min/max where you want to end
    0,                          // switch_after_sec     Time to wait after dance starts before we start dimming up
    MTN_DIMM_DOWN_CHANGE_SECONDS, // dimm_duration_sec  The amount of time to take between start and end dimming
    false                        // wait is running
  );  
  light1_steps[i++] = new DimmerStep(
    "mtn_bright1",                   // step_name          The name of this dimmer for debug
    MTN_BRIGHT,                   // dimm_target           The value between dimm_min/max where you want to end
    0,                            // switch_after_sec   Time to wait after dance starts before we start dimming up
    MTN_DIMM_UP_CHANGE_SECONDS,   // dimm_duration_sec  The amount of time to take between start and end dimming
    true                         // wait is running
  );
  light1_steps[i++] = new DimmerStep(
    "mtn_hold1",                // step_name          The name of this dimmer for debug
    MTN_BRIGHT,                   // dimm_target           The value between dimm_min/max where you want to end
    60*10,                        // switch_after_sec   song will be over before we wait this long, when song is over we restart at 1
    MTN_DIMM_UP_CHANGE_SECONDS,   // dimm_duration_sec  The amount of time to take between start and end dimming
    true                          // wait is running
  );
  if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
  }

  my_pin0 = new PWMSFShield(2);
  dimmer_list[j++] = new Dimmer(
    "light1",                   // p_dimmer_name        The name of this dimmer for debug
    my_pin0,                    // p_pwm_pin            The pwm pin assigned to this dimmer
    MTN_DIMMER_MIN,             // p_dimm_min           The bottom setting on the dimmer scale
    MTN_DIMMER_MAX,             // p_dimm_max           Full on value on the dimmer scale
    light1_steps,               // dimmer_steps         list of DimmerStep(s) and delays to execute
    NUM_DIMMER_STATES           // num_dimmer_steps     number of steps in the list, we loop back to step 1 when we get to the end of the list or back to init values when music stops
  );

// ------- dimmer2 -----------------
  i = 0;
  light2_steps[i++] = new DimmerStep(
    "mtn_dimm2",               // step_name          The name of this dimmer for debug
    MTN_DIMM,                   // dimm_target           The value between dimm_min/max where you want to end
    0,                          // switch_after_sec   Time to wait after dance starts before we start dimming up
    MTN_DIMM_DOWN_CHANGE_SECONDS, // dimm_duration_sec  The amount of time to take between start and end dimming
    false                        // wait is running
  );
  light2_steps[i++] = new DimmerStep(
    "mtn_bright2",                   // step_name          The name of this dimmer for debug
    MTN_BRIGHT,                   // dimm_target           The value between dimm_min/max where you want to end
    0,                            // switch_after_sec   Time to wait after dance starts before we start dimming up
    MTN_DIMM_UP_CHANGE_SECONDS,   // dimm_duration_sec  The amount of time to take between start and end dimming
    true                         // wait is running
  );
  light2_steps[i++] = new DimmerStep(
    "mtn_hold2",                // step_name          The name of this dimmer for debug
    MTN_BRIGHT,                   // dimm_target           The value between dimm_min/max where you want to end
    60*10,                        // switch_after_sec   song will be over before we wait this long, when song is over we restart at 1
    MTN_DIMM_UP_CHANGE_SECONDS,   // dimm_duration_sec  The amount of time to take between start and end dimming
    true                          // wait is running
  );
  if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
  }

  my_pin1 = new PWMSFShield(PWM1);
  dimmer_list[j++] = new Dimmer(
    "light2",  // p_dimmer_name        The name of this dimmer for debug
    my_pin1,                   // p_pwm_pin            The pwm pin assigned to this dimmer
    MTN_DIMMER_MIN,      // p_dimm_min           The bottom setting on the dimmer scale
    MTN_DIMMER_MAX,      // p_dimm_max           Full on value on the dimmer scale
    light2_steps,          // dimmer_steps         list of DimmerStep(s) and delays to execute
    NUM_DIMMER_STATES       // num_dimmer_steps     number of steps in the list, we loop back to step 1 when we get to the end of the list or back to init values when music stops
  );


// ------- dimmer3 -----------------
  i = 0;
  light3_steps[i++] = new DimmerStep(
    "mtn_dimm3",               // step_name          The name of this dimmer for debug
    MTN_DIMM,                   // dimm_target           The value between dimm_min/max where you want to end
    0,                          // switch_after_sec   Time to wait after dance starts before we start dimming up
    MTN_DIMM_DOWN_CHANGE_SECONDS, // dimm_duration_sec  The amount of time to take between start and end dimming
    false                        // wait is running
  );
  light3_steps[i++] = new DimmerStep(
    "mtn_bright3",                   // step_name         The name of this dimmer for debug
    MTN_BRIGHT,                   // dimm_target          The value between dimm_min/max where you want to end
    0,                            // switch_after_sec     Time to wait after dance starts before we start dimming up
    MTN_DIMM_UP_CHANGE_SECONDS,   // dimm_duration_sec    The amount of time to take between start and end dimming
    true                         // wait is running
  );
  light3_steps[i++] = new DimmerStep(
    "mtn_hold3",                // step_name            The name of this dimmer for debug
    MTN_BRIGHT,                   // dimm_target        The value between dimm_min/max where you want to end
    60*10,                        // switch_after_sec   song will be over before we wait this long, when song is over we restart at 1
    MTN_DIMM_UP_CHANGE_SECONDS,   // dimm_duration_sec  The amount of time to take between start and end dimming
    true                          // wait is running
  );
  if (NUM_DIMMER_STATES != i) {
    Serial.print(F("ERROR: NUM_DIMMER_STATES != i,  NUM_DIMMER_STATES="));
    Serial.print(NUM_DIMMER_STATES);
    Serial.print(F(", i="));
    Serial.println(i);
  }

  my_pin2 = new PWMSFShield(PWM2);
  dimmer_list[j++] = new Dimmer(
    "light3",  // p_dimmer_name        The name of this dimmer for debug
    my_pin2,                   // p_pwm_pin            The pwm pin assigned to this dimmer
    MTN_DIMMER_MIN,      // p_dimm_min           The bottom setting on the dimmer scale
    MTN_DIMMER_MAX,      // p_dimm_max           Full on value on the dimmer scale
    light3_steps,          // dimmer_steps         list of DimmerStep(s) and delays to execute
    NUM_DIMMER_STATES       // num_dimmer_steps     number of steps in the list, we loop back to step 1 when we get to the end of the list or back to init values when music stops
  );
  // check on dimmer list to make sure the numbers match.
  if (NUM_DIMMERS != j) {
    Serial.print(F("ERROR: NUM_DIMMERS != j,  NUM_DIMMERS="));
    Serial.print(NUM_DIMMERS);
    Serial.print(F(", j="));
    Serial.println(j);
  }

  // print the setup vaues for the timer and get set it's first value
  print_timer->init_values();
  print_timer->print_now();

  my_dancer = new Dancer (
    "mtn_dancer", // dancer_name      name this dancer
    DANCER_PIN,       // dance_pin        pin to read if remote is dancing
    dimmer_list,      // dimmer_list      list of dimmer strings to control
    NUM_DIMMERS       // num_dummers      number of dimmers in the list
  );

  // print the setup values for the dancer
  my_dancer->init_values();

//  // print the setup values for each dimmer
//  for (int i = 0; i < NUM_DIMMERS; i++) {
//    dimmer_list[i]->init_values();
//  }

  // print the dimmer status as we star
  print_timer->print_now();

  // print the dancer and dimmer status
  my_dancer->status(print_timer);

//  // dump the pwm tables (there was a bug) for each dimmer
//  for (int i = 0; i < NUM_DIMMERS; i++) {
//    dimmer_list[i]->dump_pwm_values();
//  }
  Serial.println(F("End setup"));
}

#endif
