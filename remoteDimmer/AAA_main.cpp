#include <arduino.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/config.h>

#include </Users/jrule/Documents/Arduino/remoteDimmer/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/Dimmer.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/Dancer.h>

// control the print speed
PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

// initialize the different dimmers 
// these were the dimmers for nostay (the book shelf)
  
// initialize the different dimmers 

////// PAST DIMMERS //////
  Dimmer past_pend(
      "past_pend",    // p_dimmer_name
      PMW2,           // p_pwm_pin 
      33,             // p_switch_after_sec
      4,              // p_dimm_duration_sec 
      100,            // p_dimm_start  
      0               // p_dimm_end
      );
  Dimmer past_clock(
      "past_clock",     // p_dimmer_name
      PMW1,             // p_pwm_pin 
      38,               // p_switch_after_sec
      4,                // p_dimm_duration_sec 
      0,                // p_dimm_start  
      100               // p_dimm_end
      );

////// PRESENT DIMMERS //////
  Dimmer present_pend(
      "present_pend",    // p_dimmer_name
      PMW4,           // p_pwm_pin 
      90,             // p_switch_after_sec
      4,              // p_dimm_duration_sec 
      100,            // p_dimm_start  
      0               // p_dimm_end
      );
  Dimmer present_clock(
      "present_clock",     // p_dimmer_name
      PMW3,             // p_pwm_pin 
      96,               // p_switch_after_sec
      4,                // p_dimm_duration_sec 
      0,                // p_dimm_start  
      100               // p_dimm_end
      );

////// FUTURE DIMMERS //////
Dimmer future_pend(
    "future_pend",    // p_dimmer_name
    PMW6,           // p_pwm_pin 
    145,             // p_switch_after_sec
    4,              // p_dimm_duration_sec 
    100,            // p_dimm_start  
    0               // p_dimm_end
    );
Dimmer future_clock(
    "future_clock",     // p_dimmer_name
    PMW5,             // p_pwm_pin 
    152,               // p_switch_after_sec
    4,                // p_dimm_duration_sec 
    0,                // p_dimm_start  
    100               // p_dimm_end
    );

// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 6
Dimmer dimmer_list[NUM_DIMMERS] = { 
            past_clock , past_pend, 
            present_clock, present_pend, 
            future_clock, future_pend
            };

Dancer *my_dancer = new Dancer (    
                          "nostay", // dancer_name      name this dancer
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

void loop()
{
  int num_dimmers = NUM_DIMMERS;

  print_timer->update();

  // is the remote dancing or not? (start/stop local timers)
  my_dancer->update(print_timer->current_millis);

  // numbers after check and update
  print_timer->status();
  my_dancer->status(print_timer);

}


