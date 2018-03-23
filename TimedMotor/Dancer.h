#ifndef DANCER_H
#define DANCER_H

///////////////////////////////////////////////////////////
//
// Class to manage remote dancer stack input 
//
/////////////////////////////////////////////////////////// 
class Dancer {

  public:

    void update();      // update the pin state
    void dance();       // check the limits and motor setting and decide how to direct the motor
    void status();      // dump the Dancer status
    void next_move();   // queue up the next move
    void move_dancer(); // if we are running, do the limit switch checks etc
    
    // p_dancer_name  name of this dancer, for status messages
    // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    // p_motor        pointer to our motor structure
    // p_my_dance_moves       list of dance moves to execute
    // p_num_dance_moves      number of dance moves
    // p_delay_dance_seconds  one time delay at beginning of dance 
    Dancer( const char *p_dancer_name,
            int p_dancer_pin, 
            GenericMotor *p_motor, 
            PrintTimer *pt,
            DanceMove **p_my_dance_moves,
            unsigned short p_num_dance_moves,
            unsigned long p_delay_dance_seconds) :
      dancer_name(p_dancer_name),
      dancer_pin(p_dancer_pin), 
      motor(p_motor),
      pt(pt),
      num_dance_moves(p_num_dance_moves),
      dance_move_index(0),
      my_dance_moves(p_my_dance_moves),
      delay_dance_millis(p_delay_dance_seconds * 1000L)
      { 
        pinMode(dancer_pin, INPUT);
        
        // make it look like the remote dance just ended, reset and extend on power
        remote_is_dancing=false;       // is the remote asking for us to dance?
        dance_just_started=false;
        dance_just_ended=false; 
              
        is_delaying=false;
        is_dancing=false;      
        is_complete=false;             
        
        current_move = my_dance_moves[dance_move_index];
        current_move->reset();
        
        Serial.print(F("CONSTRUCTOR::Dancer dancer_name="));
        Serial.print(dancer_name);
        Serial.print(F(", dancer_pin="));
        Serial.print(dancer_pin);
        Serial.print(F(", motor_name="));
        Serial.print(motor->get_motor_name());
        Serial.print(F(", dance_delay_millis="));
        Serial.print(current_move->get_delay_duration_millis()/1000);
        Serial.print(F(", dance_move_millis="));
        Serial.print(current_move->get_move_duration_millis()/1000);
        Serial.print(F(", num_dance_moves="));
        Serial.println(num_dance_moves);
 
        // pt->print_now();
        // update();       // lets read the pins and set status for the first time
        // remote_is_dancing = !remote_is_dancing;  // swap states to get a full reset
        // status();
        Serial.println(F("CONSTRUCTOR::Dancer end"));
      }
      

  private:
    //long    current_millis;             // number of milliseconds at last update call
    PrintTimer *pt;                     // the current print timer

    // initialization values
    const char    *dancer_name;               // dancer name
    int     dancer_pin;                 // pin to check for remote dancer
    char    center_is;                  // what positoin is the center position l,r,c
    char home_position;                 // home is what position l,r,c (left, right, center)  

    // dancer states in this order
    boolean remote_is_dancing;          // is the remote asking for us to dance?
    boolean dance_just_started;         // signals a new dance was started (remote_is_dancing switched to state on);
    boolean dance_just_ended;           // a dance just completed (remote_is_dancing switched to state off)
    boolean is_delaying;                // one time dance delay is complete
    boolean is_dancing;                 // delay complete and we now executing dance moves
    // the following states are only reached after we stop dancing
    boolean is_complete;                // this dance is totally and completely done


    // handle to our motor
    GenericMotor *motor;                // the motor attached to this dancer,  A dancer can only control one motor to have  more motors requires more dancers 

    // manage the dance delay values 
    unsigned long delay_dance_millis;       // number of millis to delay after remote_is_dancing is set;
    unsigned long delay_start_millis;       // time that the remote started dancing
    unsigned long delay_dance_total_millis; // total number of millis we were actually delayed

    // manage the list of dance moves to execute
    DanceMove **my_dance_moves;         // pointer to an array/list of dance moves 
    unsigned short num_dance_moves=0;   // number of dance moves in the array
    unsigned short dance_move_index;  // the current move in the list we are executing.
    DanceMove *current_move;            // pointer in the the list of the current dance move
};


// start the next move in the dance move list
void Dancer::next_move() {
  // move on to the next move
  dance_move_index++;
  if (dance_move_index >= num_dance_moves) { dance_move_index = 0;}
  current_move=my_dance_moves[dance_move_index];
  current_move->reset();

  // stop the motor between moves, the new move has a delay
  if (current_move->get_delay_amount_millis() > 0) {
    motor->stop();
  }

  motor->set_start_speed(current_move->get_start_speed());
  motor->set_target_speed(current_move->get_move_speed());


//  // we don't want to move until we have passed the delay check
//  motor->set_target_speed(0);
}

void Dancer::update() {

  int current_remote_status;

  // check our remote status
  current_remote_status = (digitalReads(dancer_pin,5) == 1);

  #ifdef ALWAYS_BE_DANCING
    current_remote_status = true;
  #endif

  #ifdef NEVER_BE_DANCING
    current_remote_status = false;
  #endif

  if (remote_is_dancing != current_remote_status) {
    if (current_remote_status) {
      Serial.print(F("Dancer::update "));
      Serial.print(dancer_name);
      Serial.println(F(" remote dancer STARTED dancing"));
      dance_just_started=true;            // signal the dance() routine we had a status change
      dance_just_ended=false;             // these can not both be true at the same time
    } else {
      dance_just_started=false;           // this can not both be true at the same time
      dance_just_ended=true;              // signal the dance() routine we had a status change
      Serial.print(F("Dancer::update "));
      Serial.print(dancer_name);
      Serial.println(F(" remote dancer STOPPED dancing"));
    }
  }

  remote_is_dancing = current_remote_status;

}

void Dancer::dance() {
    
  // remote started dancing, but we are home, so we need clear
  // the wind up starts: delay dance if necessary
  if (dance_just_started) {
    Serial.print(F("Dancer::dance() dancer_name="));
    Serial.print(dancer_name);
    Serial.print(F(" NEW_DANCE starting; START_DELAY BEGINNING delay_dance_seconds="));
    Serial.println(delay_dance_millis/1000);

    dance_move_index = num_dance_moves; // forces next_move() in the next command to reset to beginning of dance move list
    next_move();
    pt->print_now();
    current_move->status();
    dance_just_started=false;
    is_delaying=true;                                             // one time dance delay is is progress
    is_dancing=false;                                             // delay complete and we now executing dance moves
    is_complete=false;                                            // this dance is totally and completely done
    delay_start_millis = pt->current_millis;                      // time that the remote started dancing    
  }
  
  // remote just stopped dancing, setup for a finish
  if (dance_just_ended) {
    dance_just_ended=false;           // we don't want to do this multiple times
    is_delaying=false;                // one time dance delay is complete
    is_dancing=false;                 // delay complete and we now executing dance moves
    is_complete=true;                // this dance is totally and completely done
    motor->stop();

    Serial.print(F("Dancer::dance() dancer_name="));
    Serial.print(dancer_name);
    Serial.println(F(" CURRENT_DANCE ENDED"));
    
  }

  // if we are in our one-time startup delay, has it ended?
  if (is_delaying) {
    delay_dance_total_millis = pt->current_millis - delay_start_millis;
  
    if (delay_dance_total_millis >= delay_dance_millis) {
      Serial.print(F("Dancer::dance() dancer_name="));
      Serial.print(dancer_name);
      Serial.print(F(" DANCER STARTUP DELAY COMPLETE, delay_dance_total_millis="));
      Serial.print(delay_dance_total_millis);
      Serial.print(F(", delay_dance_millis="));
      Serial.println(delay_dance_millis);
      is_delaying = false;
      is_dancing = true;
      motor->start();
    }
  }
 
  // skip the run block during delays, then run until complete
  if (is_delaying==false && is_complete == false) {
    
    // remote is dancing and delay complete, update our move info and switch if it is time.
    if (is_dancing) {
      
      // apply the current dance move
      current_move->update(pt->current_millis);

      // is our move completed yet?
      if (current_move->is_complete) {
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name);
        Serial.println(F(" current MOVE COMPLETE"));
        Serial.print(F("    DanceMove ENDING "));
        current_move->status();
        next_move();
        current_move->update(pt->current_millis);
        Serial.print(F("    DanceMove STARTING "));
        current_move->status(); 
      }

      // is the dance move in a run mode (delay passed etc 
      if (current_move->is_moving) { move_dancer(); }
    }
    
  } // delaying == false && is_complete == false 
}  // dance 

// we are free to move
void Dancer::move_dancer() {
  
  motor->run();   // this takes care of things like variable speeds etc

}

// print the current dancer status
void Dancer::status() {
  if (! pt->get_do_print()) {
    return;
  }
  Serial.println("----- Dancer status() Group Start ---");
   pt->status();
  motor->status(pt->get_do_print(), pt->current_millis);
  Serial.print(F("Dancer: name="));
  Serial.print(dancer_name);
  Serial.print(F(", remote_is_dancing="));
  Serial.print(bool_tostr(remote_is_dancing));
  Serial.print(F(", current_millis="));
  Serial.println(pt->current_millis); 
  
  Serial.print(F("    dance_just_started="));
  Serial.print(bool_tostr(dance_just_started)); 
  Serial.print(F(", is_delaying="));
  Serial.print(bool_tostr(is_delaying));
  Serial.print(F(", is_complete="));
  Serial.println(bool_tostr(is_complete)); 
  
  Serial.print(F("    delay_dance_millis="));
  Serial.print(delay_dance_millis);  
  Serial.print(F(", delay_start_millis="));
  Serial.print(delay_start_millis);  
  Serial.print(F(", delay_dance_total_millis="));
  Serial.println(delay_dance_total_millis);

 

  // print the current dance move info
  Serial.print(F("Current DanceMove#: "));
  Serial.print(dance_move_index);
  Serial.print(F(", "));
  if (remote_is_dancing) {
    current_move->status();
  } else {
    Serial.println(F(" remote_is_dancing=false, skipping current_move status..."));
  }
}

////////////////////// end Dancer //////////////////////////

#endif
