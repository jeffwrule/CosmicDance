#include <arduino.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/config.h>

#include </Users/jrule/Documents/Arduino/remoteDimmer/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/Dimmer.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/Dancer.h>

// control the print speed
PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

// initialize the different dimmers 
// these were the dimmers for nostay (the book shelf)

  // p_dimm_start         The value between dimm_min/max where you want to start
  // p_dimm_end           The value between dimm_min/max where you want to end
  // p_dimm_min           The bottom setting on the dimmer scale
  // p_dimm_max           Full on step on the dimmer scale
Dimmer bookshelf1(
    String("bookshelf"),  // p_dimmer_name        The name of this dimmer for debug
    PMW1,                 // p_pwm_pin            The pwm pin assigned to this dimmer
    0,                    // p_switch_after_sec   Time to wait after dance starts before we start dimming up
    6,                    // p_dimm_duration_sec  The amount of time to take between start and end dimming
    40,                   // p_dimm_start         The value between dimm_min/max where you want to start
    100,                  // p_dimm_end           The value between dimm_min/max where you want to end
    0,                    // p_dimm_min           The bottom setting on the dimmer scale
    100                   // p_dimm_max           Full on value on the dimmer scale
    );

// create a list of all the dimmers to make the code easier to work with
#define NUM_DIMMERS 1
Dimmer dimmer_list[NUM_DIMMERS] = { bookshelf1 };

Dancer *my_dancer = new Dancer (    
                          String("nostay"), // dancer_name      name this dancer
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


