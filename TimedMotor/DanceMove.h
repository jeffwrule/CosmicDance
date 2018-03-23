#ifndef DANCEMOVE_H
#define DANCEMOVE_H

///////////////////////////////////////////////////////////
//
// Class to create a dance move
//
/////////////////////////////////////////////////////////// 

#define PUSH_DELAY_MILLIS 500

volatile unsigned int pin2_rotations=0;
volatile unsigned int pin2_bounces=0;
volatile unsigned long pin2_last_interrupt_millis=0;

volatile unsigned int pin3_rotations=0;
volatile unsigned int pin3_bounces=0;
volatile unsigned long pin3_last_interrupt_millis=0;

// interrupt service routine
void pin2_handler(){  
 if(millis() - pin2_last_interrupt_millis  > PUSH_DELAY_MILLIS)  {
     pin2_last_interrupt_millis = millis();
     pin2_rotations += 1;
 } else {
  pin2_bounces += 1;
 }
}

// interrupt service routine
void pin3_handler(){  
 if(millis() - pin3_last_interrupt_millis  > PUSH_DELAY_MILLIS)  {
     pin3_last_interrupt_millis = millis();
     pin3_rotations += 1;
 } else {
  pin3_bounces += 1;
 }
}


class DanceMove {

  public: 
    void update(unsigned long current_millis);    // update the current dance move and status  
    void reset();                                 // clear all the values back to starting value 
    void status();                                // print the current DanceMove status

    // NOTE: You must periodically call dance() to update these booleans
    boolean is_new;                                           // is ready to execute  
    boolean is_delayed;                                       // are we in the middle of the daly     
    boolean is_moving;                                        // delay passed and we are now moving
    boolean is_complete;                                      // start delay and move duration have completed
    
    unsigned int get_start_speed();                                     // what speed is this move
    unsigned int get_move_speed();                                     // what speed is this move

    unsigned long get_delay_amount_millis();                  //  accumulated delay time 
    unsigned long get_move_amount_millis();                   //  accumulated move time 
    
    unsigned long get_delay_duration_millis();                // dealy 
    unsigned long get_move_duration_millis();                 // how long are we supposed to move

    unsigned long get_current_delay_millis();                 // how long have we been delaying
    unsigned long geet_current_move_duration_millis();        // how long have we been moving

    // move_name              internal name for debug messages
    // move_delay_seconds     number of seconds to delay before we start moving
    // start_speed            speed to start the motor at
    // move_speed             speed to run motor 0-255
    // move_duration_seconds  how long to run the motor
    DanceMove(
        const char      *move_name,             // move_name              internal name for debug messages
        int             delay_amount_seconds,   // move_delay_seconds     number of seconds to delay before we start moving
        unsigned int    start_speed,            // start_speed            speed to start the motor at
        unsigned int    move_speed,             // move_speed             speed to run motor 0-255
        unsigned int    move_amount_seconds     // move_duration_seconds  how long to run the motor
        ) :
      move_name(move_name),
      delay_amount_millis(delay_amount_seconds*1000L),
      start_speed(start_speed),
      move_speed(move_speed),
      move_amount_millis(move_amount_seconds*1000L),
      signal_pin(0), 
      stop_after_rotations(0) {
        reset();

        Serial.print(F("CONSTRUCTOR1::DanceMove move_name: "));
        Serial.print(move_name);
        Serial.print(F(", delay_amount_millis="));
        Serial.print(delay_amount_millis);
        Serial.print(F(", start_speed="));
        Serial.print(start_speed);
        Serial.print(F(", move_speed="));
        Serial.print(move_speed);
        Serial.print(F(", move_amount_millis="));
        Serial.print(move_amount_millis);
        Serial.print(F(", signal_pin="));
        Serial.print(signal_pin);
        Serial.print(F(", stop_after_rotations="));
        Serial.println(stop_after_rotations);
      }

    // move_name              internal name for debug messages
    // move_delay_seconds     number of seconds to delay before we start moving
    // start_speed            speed to start the motor at
    // move_speed             speed to run motor 0-255
    // move_duration_seconds  how long to run the motor
    // signal_pin             which interrupt enabled pin is our rotation swith attached to
    // stop_after_rotations   number of rotations to execute before we stop
    DanceMove(
        const char      *move_name,             // move_name              internal name for debug messages
        int             delay_amount_seconds,   // move_delay_seconds     number of seconds to delay before we start moving
        unsigned int    start_speed,            // start_speed            speed to start the motor at
        unsigned int    move_speed,             // move_speed             speed to run motor 0-255
        int             move_amount_seconds,    // move_amount_seconds    how long to run the motor
        unsigned short  signal_pin,             //  which interrupt enabled pin is our rotation swith attached to
        unsigned int    stop_after_rotations    // number of rotations to execute before we stop
        ) :
      move_name(move_name),
      delay_amount_millis(delay_amount_seconds*1000L),
      start_speed(start_speed),
      move_speed(move_speed),
      move_amount_millis(move_amount_seconds*1000L),
      signal_pin(signal_pin), 
      stop_after_rotations(stop_after_rotations) {
        reset(); 

        pinMode(signal_pin, INPUT_PULLUP);

        Serial.print(F("CONSTRUCTOR2::DanceMove move_name: "));
        Serial.print(move_name);
        Serial.print(F(", delay_amount_millis: "));
        Serial.print(delay_amount_millis);
        Serial.print(F(", start_speed: "));
        Serial.print(start_speed);
        Serial.print(F(", move_speed: "));
        Serial.print(move_speed);
        Serial.print(F(", move_amount_millis: "));
        Serial.print(move_amount_millis);
        Serial.print(F(", signal_pin="));
        Serial.print(signal_pin);
        Serial.print(F(", stop_after_rotations="));
        Serial.println(stop_after_rotations);
      }
      
  private:

    const char*  move_name;                 // the name of this move, for status messages

    boolean delay_is_complete();            // has delay duration passed?
    boolean move_is_complete();             // has move seconds passed?
    boolean rotations_complete();           // have we execute the expected number of rotations?

    unsigned int  start_speed;              // speed to move at 0-255 (pwm speed)
    unsigned int  move_speed;               // speed to move at 0-255 (pwm speed)

    unsigned long delay_amount_millis;      // total time to delay
    unsigned long move_amount_millis;       // total time to move (in this direction)
    
    unsigned long delay_duration_millis;    // accumulated delay time (only computed for actual delay).
    unsigned long move_duration_millis;     // accumulated move time (only computed while moving)
    
    unsigned long delay_start_millis;       // when did the dealy start
    unsigned long move_start_millis;        // when did the move start

    unsigned int signal_pin;
    unsigned int stop_after_rotations;
    volatile unsigned int *rotations;
    volatile unsigned int *bounces;
    volatile unsigned long *last_interrupt_millis;

    unsigned int dummy_int=0;
    unsigned long dummy_long=0;

};


  
// dump a status line for this move
void DanceMove::status() {
  Serial.print(F("DanceMove::status move_name: "));
  Serial.print(move_name);
  Serial.print(F("  is_new: "));
  Serial.print(bool_tostr(is_new));
  Serial.print(F(", is_delayed: "));
  Serial.print(bool_tostr(is_delayed));
  Serial.print(F(", is_moving: "));
  Serial.print(bool_tostr(is_moving));
  Serial.print(F(", is_complete: "));
  Serial.print(bool_tostr(is_complete));
  Serial.print(F(", start_speed: "));
  Serial.print(start_speed);
  Serial.print(F(", move_speed: "));
  Serial.println(move_speed);
  Serial.print(F("        delay_amount_millis: "));
  Serial.print(delay_amount_millis);
  Serial.print(F(", delay_start_millis: "));
  Serial.print(delay_start_millis);
  Serial.print(F(", delay_duration_millis: "));
  Serial.print(delay_duration_millis);
  Serial.print(F(", move_amount_millis: "));
  Serial.print(move_amount_millis);
  Serial.print(F(", move_start_millis: "));
  Serial.print(move_start_millis);
  Serial.print(F(", move_duration_millis: "));
  Serial.println(move_duration_millis);
  if (signal_pin != 0) {
    Serial.print(F("    rotations="));
    Serial.print(*rotations);
    Serial.print(F(", bounces="));
    Serial.print(*bounces);
    Serial.print(F(", last_interrupt_millis="));
    Serial.print(*last_interrupt_millis);
    Serial.print(F(", last_interrupt_millis_diff="));
    Serial.println(millis() - *last_interrupt_millis);
  }
}
    
unsigned int DanceMove::get_start_speed() { return start_speed; }
unsigned int DanceMove::get_move_speed() { return move_speed; }
unsigned long DanceMove::get_delay_amount_millis() { return delay_amount_millis; }
unsigned long DanceMove::get_move_amount_millis() { return move_amount_millis; }
unsigned long DanceMove::get_delay_duration_millis() { return delay_duration_millis; }
unsigned long DanceMove::get_move_duration_millis() { return move_duration_millis; }

boolean DanceMove::delay_is_complete() {
  return delay_duration_millis >= delay_amount_millis;
}

boolean DanceMove::move_is_complete() {
  return move_duration_millis >= move_amount_millis;
}

boolean DanceMove::rotations_complete() {
  if (signal_pin != 0 ) {
    if (*rotations >= stop_after_rotations) {
      return true;
    }
  }
  return false;
}

void DanceMove::update(unsigned long current_millis) {

  // reset happened, but have not started delay 
  if (is_new) {
    is_new = false;
    is_delayed = false;
    delay_start_millis = current_millis;
    Serial.print(F("DanceMove::dance move_name="));
    Serial.print(move_name);
    Serial.println(F(" STARTED, now delaying..."));
  }

  // track the total delay time
  if (is_delayed == false) {
    delay_duration_millis = current_millis - delay_start_millis;
    if (delay_is_complete()) {
      is_delayed = true;
      is_moving = true;
      move_start_millis = current_millis;
      Serial.print(F("DanceMove::dance  move_name="));
      Serial.print(move_name);
      Serial.println(F(" DELAY COMPLETE, now moving..."));
    }
  }

  // track total moving time as long as wel are moving
  if (is_moving) {
    move_duration_millis = current_millis - move_start_millis;
    boolean m_complete = move_is_complete();
    boolean r_complete = rotations_complete();
    if (m_complete || r_complete) {
      is_moving = false;
      is_complete = true;
      Serial.print(F("DanceMove::dance COMPLETE, move_name="));
      Serial.print(move_name);
      Serial.print(F(", move_is_complete()="));
      Serial.print(bool_tostr(m_complete));
      Serial.print(F(", move_duration_millis="));
      Serial.print(move_duration_millis);
      Serial.print(F(", rotations_complete()="));
      Serial.print(bool_tostr(r_complete));
      Serial.print(F(", rotations="));
      Serial.println(*rotations);
    }
  }
}

void DanceMove::reset() {
    is_new=true;            // is ready to execute
    is_delayed=false;       // is the delay complete
    is_moving=false;        // is in flight 
    is_complete=false;      // move is fully complete

    delay_start_millis = 0;
    move_start_millis=0;

    delay_duration_millis = 0;
    move_duration_millis=0;

    if (signal_pin == 2) {
      attachInterrupt(digitalPinToInterrupt(signal_pin), pin2_handler, FALLING);
      rotations = &pin2_rotations;
      bounces = &pin2_bounces;
      last_interrupt_millis = pin2_last_interrupt_millis; 
    } else if (signal_pin == 3) {
      attachInterrupt(digitalPinToInterrupt(signal_pin), pin3_handler, FALLING);
      rotations = &pin3_rotations;
      bounces = &pin3_bounces;
      last_interrupt_millis = pin3_last_interrupt_millis; 
    } else {
      signal_pin=0;                       // the only valid values are 0, 2, 3
      rotations = &dummy_int;
      bounces = &dummy_int;
      last_interrupt_millis = &dummy_long;
    }

    // clear the global values for these.
    if (signal_pin != 0) {
      *rotations = 0;
      *bounces = 0;
      *last_interrupt_millis = 0;       
    }
}


#endif
