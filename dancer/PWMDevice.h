#ifndef PWMDEVICE_H
#define PWMDEVICE_H

// this device was created to be always dimming and brightening....

// uncomment the VARY_SPEED define on if you want to have a variable speed motor 
// you must attach PIN3 to the mosfet to make this work.
// you also need to cut the traces for pin three on the MP3 card or you will conflict with the MP3 card.
// The speed of the motor will continiously vary between MIN and MAX speed
// The VARY_SECONDS defines how long it takes to go from MIN to MAX or vise versa
// To create a motor that starts above or below the variable range set INIT speed, above MAX (typical) or below MIN. 
// The montor will then be ramped down or up to MIN/MAX and then vary between MIN/MAX until music ends.
// If you want a montor that just has a jump start set INIT high and MIN = MAX.  
//    This will ramp down(typical)/up the speed until it reaches MIN/MAX which are equal and will not move again.
// SET VARY_SECONDS to set the number of seconds between MIN/MAX values
// SET VARY_SECONDS_INIT to set the number of seconds between INIT and reaching either MIN if INIT is below min or MAX if INIT is above MAX.

//#define VARY_SPEED

//#define VARY_MIN_SPEED 40L   // string wave (oval piece) 
//#define VARY_MAX_SPEED 40L   // string wave (oval piece)
//#define VARY_INIT_SPEED 60L  // string wave (oval piece)
#define VARY_MIN_SPEED 109L    // string ripple 
#define VARY_MAX_SPEED 109L    // string ripple
#define VARY_INIT_SPEED 119L   // string ripple 
#define VARY_PIN 3             // D3 pin, if you are going to use this you must cut the trace on sparkfun mp3 card for MIDI-IN
#define VARY_OFF 0             // OFF Speed for monto (do not change this)
#define VARY_SECONDS 20L       // number of seconds between min and max
#define VARY_SECONDS_INIT 5L  // number of seconds to vary the init value down/up to min/max

// global variables for VARY speed feature
float vary_step = 1.0;                    // default, how big a step to take when incrementing or decrementing
float vary_step_init = 1.0;               // default, how big a step to take when incrementing or decrementing
float vary_current_speed = VARY_OFF;      // current motor speed
unsigned long vary_prev_millis;           // mills last time we incremented or decremented
char vary_direction = 'i';                // i = increasing; d = decreasing
boolean vary_jumped = false;              // true when we are jumpstarting the motor, less < MIN OR > MAX
unsigned int vary_min = VARY_MIN_SPEED;   // default this value
unsigned int vary_max = VARY_MAX_SPEED;   // default this value
unsigned int vary_init = VARY_INIT_SPEED; //default this value


// vary the speed of the motor
// This routine will continuiously vary the montor speed between min and max, 
// To obtain a constant speed set min and max to the same value
// The init speed tells us if we want to start above or below this variable range.
void do_vary_speed()
{
  unsigned long cur_millis;
  float my_step;

  // are we still in the jumped phase?
  if (vary_jumped) {
    if (vary_init < vary_min && vary_current_speed >= vary_min) { vary_jumped = false;}
    if (vary_init > vary_max && vary_current_speed <= vary_max) { vary_jumped = false;}
  }

  // choose our step size based on jumped or regular phase
  if (vary_jumped) {
    my_step = vary_step_init;
  } else {
    my_step = vary_step;
  }

  // are we increasing or decreasing...
  if (vary_direction = 'i' && vary_current_speed >= vary_max) {
    vary_direction = 'd';
  } else if (vary_direction = 'd' && vary_current_speed <= vary_min) {
    vary_direction = 'i';
  }
  
  cur_millis = millis();
  if (cur_millis < vary_prev_millis) { vary_prev_millis = cur_millis;} // handle millis wrapping

  // adjust the speed in my_step intervals
  if (cur_millis - vary_prev_millis > 1000) {
    if (vary_direction = 'i') {
      vary_current_speed += my_step;
    } else {
      vary_current_speed -= my_step;
    }
    if (vary_jumped == false) {
      if (vary_current_speed > vary_max) { vary_current_speed = vary_max; vary_direction = 'd'; }
      if (vary_current_speed < vary_min) { vary_current_speed = vary_min; vary_direction = 'i'; }      
    }
    #if defined IS_CHATTY
      Serial.print(F("do_vary_speed: vary_current_speed: "));
      Serial.print(vary_current_speed,4);
      Serial.print(F(", vary_direction: "));
      Serial.print(vary_direction);
      Serial.print(F(", mills_diff: "));
      Serial.println(cur_millis - vary_prev_millis);
    #endif
    analogWrite(VARY_PIN, (int) vary_current_speed);
    vary_prev_millis = cur_millis;
  }
}


#endif

