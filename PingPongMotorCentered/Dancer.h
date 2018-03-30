#ifndef DANCER_H
#define DANCER_H

#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/GenericMotor.h>

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
    // pt             PrinterTimer pointer
    // p_limits_name  name to give the l/r/c Limits object
    // p_left_limit   left limit switch
    // p_right_limit  right limit switch
    // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    // p_my_dance_moves       list of dance moves to execute
    // p_num_dance_moves      number of dance moves
    // p_delay_dance_seconds  one time delay at beginning of dance 
    // p_extend_dance_seconds number of seconds to extend the dance when remote is done and home
    // p_extend_speed         speed to run the extend opearation at
    // p_center_speed         speed to run the motor when centering
    Dancer( const char* p_dancer_name,
            int p_dancer_pin, 
            GenericMotor *p_motor, 
            PrintTimer *pt,
            const char* p_limits_name,
            GenericLimitSwitch *p_left_limit,
            GenericLimitSwitch *p_right_limit,
            GenericLimitSwitch *p_center_limit,
            DanceMove **p_my_dance_moves,
            unsigned short p_num_dance_moves,
            unsigned long p_delay_dance_seconds,
            unsigned long p_extend_dance_seconds,
            unsigned int  p_extend_speed,
            unsigned int  p_center_speed) :
      dancer_name(p_dancer_name),
      dancer_pin(p_dancer_pin), 
      motor(p_motor),
      pt(pt),
      num_dance_moves(p_num_dance_moves),
      dance_move_index(0),
      my_dance_moves(p_my_dance_moves),
      delay_dance_millis(p_delay_dance_seconds * 1000L),
      extend_dance_millis(p_extend_dance_seconds * 1000L),
      extend_speed( min(p_extend_speed, p_motor->get_max_speed()) ),
      center_speed( min(p_center_speed, p_motor->get_max_speed()) )
      { 
        pinMode(dancer_pin, INPUT);
        
        // make it look like the remote dance just ended, reset and extend on power
        remote_is_dancing=false;       // is the remote asking for us to dance?
        dance_just_ended=false;       // this lets it switch the value when it is done
        is_complete=false;             // make sure it looks like we are not home
        center_passed = false;
                
        current_move = my_dance_moves[dance_move_index];
        current_move->reset(0);
                
        current_limits = new Limits(
            p_limits_name, // p_limits_name        The name of this limits combo
            p_left_limit,             // p_left_limit         The left limit switch structure
            p_right_limit,            // p_right_limit        The right limit switch structure
            p_center_limit            // p_center_limit       The center limit swtich struct (AKA the home switch) can be a pointer to left_limit or right_limit if the represent home
          );

        center_is = current_limits->center_is();

        Serial.print(F("CONSTRUCTOR::Dancer dancer_name="));
        Serial.print(dancer_name);
        Serial.print(F(", dancer_pin="));
        Serial.print(dancer_pin);
        Serial.print(F(" motor_name="));
        Serial.println(motor->get_motor_name());
        
        Serial.print(F("        limits-name="));
        Serial.print(p_limits_name);
        Serial.print(F(", left_limit_name="));
        Serial.print(p_left_limit->get_switch_name());
        Serial.print(F(", right_limit_name="));
        Serial.print(p_right_limit->get_switch_name());
        Serial.print(F(", center_limit_name="));
        Serial.println(p_center_limit->get_switch_name());
        
        Serial.print(F("        dance_delay_millis="));
        Serial.print(current_move->get_delay_duration_millis()/1000);
        Serial.print(F(", dance_move_millis="));
        Serial.print(current_move->get_move_duration_millis()/1000);
        Serial.print(F(", num_dance_moves"));
        Serial.print(num_dance_moves);
        Serial.print(F(", extend_dance_millis="));
        Serial.print(extend_dance_millis);
        Serial.print(F(", extend_speed="));
        Serial.print(extend_speed);
        Serial.print(F(", center_speed="));
        Serial.print(center_speed);
        Serial.print(F(", center_is="));
        Serial.println(center_is);     

        pt->print_now();
        update();       // lets read the pins and set status for the first time
        remote_is_dancing = !remote_is_dancing;  // swap states to get a full reset
        status();
        Serial.println(F("CONSTRUCTOR::Dancer end"));
      }
      

  private:
    //long    current_millis;             // number of milliseconds at last update call
    PrintTimer *pt;                     // the current print timer

    // initialization values
    const char* dancer_name;                 // dancer name
    int     dancer_pin;                 // pin to check for remote dancer
    char    center_is;                  // what positoin is the center position l,r,c
    char home_position;                 // home is what position l,r,c (left, right, center)  

    // dancer states in this order
    boolean remote_is_dancing;          // is the remote asking for us to dance?
    boolean dance_just_started;         // signals a new dance was started (remote_is_dancing switched to state on);
    boolean dance_just_ended;           // a dance just completed (remote_is_dancing switched to state off)
    boolean is_delaying;                // one time dance delay is complete
    boolean is_delayed;
    boolean is_dancing;                 // delay complete and we now executing dance moves
    // the following states are only reached after we stop dancing
    boolean is_centering;               // remote stopped dancing and we are looking for our center switch
    boolean is_centered;                // we found our center
    boolean is_extending;               // actively extending
    boolean is_extended;                // have we completed the extend
    boolean is_complete;                // this dance is totally and completely done

    // manage the limit switches
    Limits *current_limits;             // left right center limits for this dancer
    boolean center_passed;              // have we passed the center

    // handle to our motor
    GenericMotor *motor;                // the motor attached to this dancer,  A dancer can only control one motor to have  more motors requires more dancers 

    // manage the dance delay values 
    unsigned long delay_dance_millis;       // number of millis to delay after remote_is_dancing is set;
    unsigned long delay_start_millis;       // time that the remote started dancing
    unsigned long delay_dance_total_millis; // total number of millis we were actually delayed

    // manage the center step
    unsigned int center_speed;               // the speed to run the motor when centering 0-255

    // manage the extend dance 
    unsigned long extend_dance_millis;        // number of seconds to extend the dance remote stops dancing
    unsigned long extend_start_millis;        // millisonds when we are finally home (extend beyond home position)
    unsigned long extend_dance_total_millis;  // total number of millis we actually extended
    unsigned int extend_speed;                // speed to run the final extend at

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
  current_move->reset(delay_start_millis);

  // stop the motor between moves, the new move has a delay
  if (current_move->get_delay_amount_millis() > 0) {
    motor->stop();
  }

  // set the move direction
  if (motor->current_direction() != current_move->get_move_direction()) {
    if (current_move->get_move_direction() == 'l' ) {
      motor->go_left();
    }
    if (current_move->get_move_direction() == 'r') {
      motor->go_right();
    }
  }
  // we don't want to move until we have passed the delay check
  motor->set_target_speed(0);
}

void Dancer::update() {

  int current_remote_status;

  //delayMicroseconds(PIN_READ_DELAY_MS);
  // check our remote status
  current_remote_status = (digitalReads(dancer_pin,5) == 1);

  #ifdef ALWAYS_BE_DANCING
    current_remote_status = true;
  #endif

  #ifdef NEVER_BE_DANCING
    current_remote_status = false;
  #endif

  current_limits->update(motor->current_direction());

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
  
  boolean reverse_motor=false;  // assume we are going the correct way before we check

  if (current_limits->sum_limits() > 1) {
    if (!motor->is_stopped()) {
      motor->stop();    
    }
    if (pt->get_do_print()) {
          Serial.print(F("ERROR: Dancer::dance() dancer_name="));
          Serial.print(dancer_name);
          Serial.println(F(" too_many_limits_active stopping motors"));
    }
    return;
  }

  
  // remote started dancing, but we are home, so we need clear
  // the wind up starts: delay dance if necessary
  if (dance_just_started) {
    Serial.print(F("Dancer::dance() dancer_name="));
    Serial.print(dancer_name);
    Serial.print(F(" NEW_DANCE starting; START_DELAY BEGINNING delay_dance_seconds="));
    Serial.println(delay_dance_millis/1000);
    
    // other bits of the code depend on this being set, do it first.
    delay_start_millis = pt->current_millis;       // time that the remote started dancing
    
    dance_move_index = num_dance_moves; // forces next_move() in the next command to reset to beginning of dance move list
    next_move();
    
    pt->print_now();
    current_move->status();
    dance_just_started=false;
    is_delaying=true;                 // one time dance delay is is progress
    is_delayed=false;                 // we have not completed the extend
    is_dancing=false;                 // delay complete and we now executing dance moves
    is_centering=false;               // remote stopped dancing and we are looking for our center switch
    is_centered=false;                // we found our center
    is_extending=false;               // motor is being run to extend the dance 
    is_extended=false;                // extend is complete
    is_complete=false;                // this dance is totally and completely done

    
  }
  
  // remote just stopped dancing, setup for a finish
  if (dance_just_ended) {
    dance_just_ended=false;           // we don't want to do this multiple times
    is_delaying=false;                // one time dance delay is complete
    is_delayed=true;                  // our delay is considered complete when we are stopped
    is_dancing=false;                 // delay complete and we now executing dance moves
    is_centering=true;                // remote stopped dancing and we are looking for our center switch
    is_centered=false;                // center is not completed
    is_extending=false;               // motor was run for extended duration after centered 
    is_extended=false;                // extend complete
    is_complete=false;                // this dance is totally and completely done

    Serial.print(F("Dancer::dance() dancer_name="));
    Serial.print(dancer_name);
    Serial.println(F(" CURRENT_DANCE ENDING; STARTED_CENTERING"));
  }

  // if we are in our one-time startup delay, has it ended?
  if (is_delaying) {
    delay_dance_total_millis = pt->current_millis - delay_start_millis;
  
    if (delay_dance_total_millis >= delay_dance_millis) {
      Serial.print(F("Dancer::dance() dancer_name="));
      Serial.print(dancer_name);
      Serial.println(F(" DANCER STARTUP DELAY COMPLETE, starting to apply moves"));
      is_delaying = false;
      is_dancing = true;
    }
  }
 
  // skip the run block during delays, then run until complete
  if (is_delaying==false && is_complete == false) {
    
    // remote is dancing and delay complete, update our move info and switch if it is time.
    if (is_dancing) {
      
      // apply the current dance move
      current_move->update(pt->current_millis);

      // set our target speed for this move 
      // never go above the max limit on the motor
      if (current_move->is_moving && min(current_move->get_move_speed(), motor->get_max_speed()) !=  motor->get_target_speed()) { 
        motor->set_target_speed(min(current_move->get_move_speed(), motor->get_max_speed())); 
        }

      // is our move completed yet?
      if (current_move->is_complete) {
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name);
        Serial.println(F("current MOVE COMPLETE"));
        Serial.print(F("    DanceMove ENDING "));
        current_move->status();
        next_move();
        current_move->update(pt->current_millis);
        Serial.print(F("    DanceMove STARTING "));
        current_move->status(); 
      }
    }

    // are we centering? is it complete, are we headed in the correct direction?
    if (is_centering) {

      // have we reached the center yet?
      if (current_limits->is_centered()) {
        pt->print_now();
        current_limits->status(pt->get_do_print());
        is_centering=false;
        is_centered=true;
        is_extending=true;
        extend_start_millis = pt->current_millis;
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name);
        Serial.println(F(" CENTERED is_centered=true, starting to EXTEND DANCE"));
      }
      
      // not centered are we headed in the right direction?
      if (!is_centered) {
        // are we going the wrong direction?
        if (center_is == 'c' && center_passed) {
          motor->reverse();
        } else if (motor->current_direction() != center_is) { // center is l/r and we are not going in the direction, then go back
          motor->reverse();
        }

        // set our target speed for centerning. 
        if (motor->get_target_speed() != center_speed) { 
          motor->set_target_speed(center_speed); 
        }
      } // !is_centered
    }

    // are we extending
    if (is_extending) {
      extend_dance_total_millis = pt->current_millis - extend_start_millis;

      // note if we extend so long we hit the limit switch, it will continue to extend back in the other direction       
      if (extend_dance_total_millis >= extend_dance_millis) {
        is_extending = false;
        is_extended = true;
        is_complete = true;
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name);
        Serial.println(F(" EXTENDED DANCE IS_COMPLETE"));
        if (!motor->is_stopped()) {
          motor->stop();
        }
      } // is extended?

      if (is_extending) {
        // set our taret speed for extending.
        if (motor->get_target_speed() != extend_speed) { 
          motor->set_target_speed(extend_speed); 
          }       
      } // is_extending
      
    }  // still extending? 

    // makes the motor go with some conditation if we are not in a comlete state
    if (!is_complete) {
      
      // if remote stopped and not complete we need to go
      if (remote_is_dancing==false) {
        move_dancer();
      }
      
      // if remote_is_dancing and primary delay passed; and current_move delay passed; we dance!
      if (is_dancing && current_move->is_moving) {
        move_dancer();
      }  
           
    }
    
  } // delaying == false && is_complete == false 
}  // dance 

// we are free to move
void Dancer::move_dancer() {

  // check the left limit first
  if (current_limits->is_max_left() && motor->current_direction() == 'l') {
    Serial.println(F("INFO: Dancer::move_dancer() current_move direction is left and at max left, skpping to next move"));
    pt->print_now();
    current_limits->status(pt->get_do_print());
    if (!motor->is_stopped()) {
      pt->print_now();
      motor->status();
      motor->stop();
    }
    // motor->go_right();
    center_passed = false;
    next_move();  // keep looking for moves until we get one going back the other direction
  }

  // check the right limit next
  if(current_limits->is_max_right() && motor->current_direction() == 'r') {
    Serial.println(F("INFO: Dancer::move_dancer() current_move direction is right and at max right, skpping to next move"));
    pt->print_now();
    current_limits->status(pt->get_do_print());
    if (!motor->is_stopped()) {
      pt->print_now();
      motor->status();
      motor->stop();
    }
    // motor->go_left();
    center_passed = false;
    next_move();
  }  

//  // we should be running in this section
//  if (motor->is_stopped()) {
//    motor->start();
//  }
  
  motor->run();   // this takes care of things like variable speeds etc

}

// print the current dancer status
void Dancer::status() {
  if (! pt->get_do_print()) {
    return;
  }
  Serial.println("----- Dancer status() Group Start ---");
  motor->status();
  current_limits->status(pt->get_do_print());
  Serial.print(F("Dancer: name="));
  Serial.print(dancer_name);
  Serial.print(F(", remote_is_dancing="));
  Serial.print(bool_tostr(remote_is_dancing));
  Serial.print(F(", current_millis="));
  Serial.println(pt->current_millis); 
  
  Serial.print(F("        dance_just_started="));
  Serial.print(bool_tostr(dance_just_started)); 
  Serial.print(F(", is_delaying="));
  Serial.print(bool_tostr(is_delaying));
  Serial.print(F(", delay_dance_millis="));
  Serial.print(delay_dance_millis);  
  Serial.print(F(", delay_start_millis="));
  Serial.print(delay_start_millis);  
  Serial.print(F(", delay_dance_total_millis="));
  Serial.print(delay_dance_total_millis);
  Serial.print(F(", is_delayed="));
  Serial.println(bool_tostr(is_delayed));

  // finally print our extended info
  Serial.print(F("        dance_just_ended="));
  Serial.print(bool_tostr(dance_just_ended)); 
  Serial.print(F(", is_centering="));
  Serial.print(bool_tostr(is_centering)); 
  Serial.print(F(", center_passed="));
  Serial.print(bool_tostr(center_passed)); 
  Serial.print(F(", is_centered="));
  Serial.println(bool_tostr(is_centered)); 

  Serial.print(F("        is_exending="));
  Serial.print(bool_tostr(is_extending));
  Serial.print(F("        extend_dance_millis="));
  Serial.print(extend_dance_millis);  
  Serial.print(F(", extend_start_millis="));
  Serial.print(extend_start_millis);  
  Serial.print(F(", extend_dance_total_millis="));
  Serial.print(extend_dance_total_millis);
  Serial.print(F(", is_extended="));
  Serial.print(bool_tostr(is_extended)); 
  Serial.print(F(", is_complete="));
  Serial.println(bool_tostr(is_complete)); 

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
