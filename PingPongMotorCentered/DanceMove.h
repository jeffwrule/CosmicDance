#ifndef DANCEMOVE_H
#define DANCEMOVE_H

///////////////////////////////////////////////////////////
//
// Class to create a dance move
//
/////////////////////////////////////////////////////////// 

class DanceMove {

  public: 
    void update(unsigned long current_millis);        // update the current dance move and status  
    void reset(unsigned long dance_started_millis);   // clear all the values back to starting value 
    void status();                                    // print the current DanceMove status

    // NOTE: You must periodically call dance() to update these booleans
    boolean is_new;                                           // is ready to execute  
    boolean is_delayed;                                       // are we in the middle of the daly     
    boolean is_moving;                                        // delay passed and we are now moving
    boolean is_complete;                                      // start delay and move duration have completed
    
    char get_move_direction();                                // what direction is this move
    int get_move_speed();                                     // what speed is this move

    unsigned long get_delay_amount_millis();                  //  accumulated delay time 
    unsigned long get_move_amount_millis();                   //  accumulated move time 
    
    unsigned long get_delay_duration_millis();                // dealy 
    unsigned long get_move_duration_millis();                 // how long are we supposed to move

    unsigned long get_current_delay_millis();                 // how long have we been delaying
    unsigned long geet_current_move_duration_millis();        // how long have we been moving


    // move_name              internal name for debug messages
    // move_delay_seconds     number of seconds to delay before we start moving
    // move_direction         l,r (l is typically up)
    // move_speed             speed to run motor 0-255
    // move_duration_seconds  how long to run the motor
    DanceMove(
        const char*  move_name,            // move_name            The name to use for debugging
        int     delay_amount_seconds, // delay_amount_seconds The number of seconds to delay  
        char    move_direction,       // move_direction       Direction to spin motor
        int     move_speed,           // move_spped           How fast to spin the motor
        int     move_amount_seconds,  // move_amount_seconds  How long to move the motor (will also stop if limit is reached)
        unsigned char delay_type      // delay_type           DELAY_TYPE_DANCE (delay_amount_seconds) is from start of dance, DELAY_TYPE_MOVE (dealy is from start of move) 
        ) :
      move_name(move_name),
      delay_amount_millis(delay_amount_seconds*1000L),
      move_direction(move_direction),
      move_speed(move_speed),
      move_amount_millis(move_amount_seconds*1000L),
      delay_type(delay_type) {
        
        reset(0); 

        Serial.print(F("CONSTRUCTOR::DanceMove move_name: "));
        Serial.print(move_name);
        Serial.print(F(", delay_amount_millis: "));
        Serial.print(delay_amount_millis);
        Serial.print(F(", move_direction: "));
        Serial.print(move_direction);
        Serial.print(F(", move_speed: "));
        Serial.print(move_speed);
        Serial.print(F(", move_amount_millis: "));
        Serial.print(move_amount_millis);
        Serial.print(F(", delay_type="));
        Serial.println((char)delay_type);
      }

  private:

    const char*  move_name;                      // the name of this move, for status messages

    boolean delay_is_complete();            // has delay duration passed?
    boolean move_is_complete();             // has move seconds passed?

    char move_direction;                    // direction to move the motor l,r (typically l=dow, r=up)
    int  move_speed;                        // speed to move at 0-255 (pwm speed)

    unsigned long delay_amount_millis;      // total time to delay
    unsigned long move_amount_millis;       // total time to move (in this direction)
    unsigned char delay_type;               // type of delay dance based 'd' or move based 'm'
    
    unsigned long delay_duration_millis;          // accumulated delay time (only computed for actual delay).
    unsigned long move_duration_millis;           // accumulated move time (only computed while moving)
    
    unsigned long delay_start_millis;             // when did the dealy start
    unsigned long move_start_millis;              // when did the move start

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
  Serial.print(F(", move_direction: "));
  Serial.print(move_direction);
  Serial.print(F(", move_speed: "));
  Serial.println(move_speed);
  Serial.print(F("        delay_amount_millis: "));
  Serial.print(delay_amount_millis);
  Serial.print(F(", delay_start_millis: "));
  Serial.print(delay_start_millis);
  Serial.print(F(", delay_duration_millis: "));
  Serial.print(delay_duration_millis);
  Serial.print(F(", delay_type="));
  Serial.print((char)delay_type);
  Serial.print(F(", move_amount_millis: "));
  Serial.print(move_amount_millis);
  Serial.print(F(", move_start_millis: "));
  Serial.print(move_start_millis);
  Serial.print(F(", move_duration_millis: "));
  Serial.println(move_duration_millis);
}

char DanceMove::get_move_direction() { return move_direction; }
int DanceMove::get_move_speed() { return move_speed; }
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

void DanceMove::update(unsigned long current_millis) {

  // reset happened, but have not started delay 
  if (is_new) {
    is_new = false;
    is_delayed = false;
    // if DELAY_TYPE_DANCE; set during reset()
    if (delay_type == DELAY_TYPE_MOVE) {
      delay_start_millis = current_millis;      
    }
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
    if (move_is_complete()) {
      is_moving = false;
      is_complete = true;
      Serial.print(F("DanceMove::dance move_name="));
      Serial.print(move_name);
      Serial.println(F(" COMPLETE"));
    }
  }
}

void DanceMove::reset(unsigned long dance_start_millis) {
    is_new=true;            // is ready to execute
    is_delayed=false;       // is the delay complete
    is_moving=false;        // is in flight 
    is_complete=false;      // move is fully complete

    delay_start_millis = dance_start_millis;
    move_start_millis=0;

    delay_duration_millis = 0;
    move_duration_millis=0;
}


#endif
