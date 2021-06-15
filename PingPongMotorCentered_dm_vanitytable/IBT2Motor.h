#ifndef IBT2MOTOR_H
#define IBMT2MOTOR_H

///////////////////////////////////////////////////////////
//
// Class to manage motor Half Bridge Motor break out board 
// IBT_2 motor
//
/////////////////////////////////////////////////////////// 


class IBT2Motor: public GenericMotor {
  public:
    void stop();                // stop the motor by bring pwm down to 0
    void start();               // if current speed is less then start speed, make it start speed
    void run();                 // increment motor speed to max speed in SPEED_INCREMENT jumps over multiple calls, does nothing if motor is disabled
    void reverse();
    void go_left();
    void go_right();
    boolean is_stopped();
    boolean is_disabled();
    char current_direction();
    void enable();
    void disable();
    void status();
    int speed = 0;
    char *get_motor_name();
    void set_target_speed(unsigned int new_target_speed);
    unsigned int get_target_speed();
    unsigned int get_max_speed();

    // p_motor_name                         name of this motor (for status messages)
    // m_speed_left                         max speed going left
    // m_speed_right                        max speed going right
    // p_pwml                               left pwm pin
    // p_pwmr                               right pwm pin
    // p_motor_start_speed                  start speed when start() is called
    // p_speed_increment                    pmw increment when starting the motor
    // p_stop_delay                         delay this many milliseconds when every n th itteration
    // p_stop_delay_every_n_left            number of itterations going left to wait before delaying p_stop_delay
    // p_stop_delay_every_n_right           number of itterations going right to wait before delaying p_stop_delay  
    // p_reverse_delay_millis               delay this number of milliseconds when reversing direction
    // p_needs_jumpstart_left               do we jump start when going left (add extra power, and then wind down)
    // p_needs_jumpstart_right              do we jump start when going right (add extra pwer, and then slow down)
    // p_jump_millis_duration1              amount of time to run at higher faster speed during jumpstart period 1
    // p_jumpstart_speed1                   max speed to run during jumpstart1
    // p_jump_millis_duration2              amount of time to run at higher faster speed during jumpstart period 2, must be larger then p_jump_millis_duration1
    // p_jumpstart_speed2                   max speed to run during jumpstart2, should be less then or equal to p_jumpstart_speed1
    // p_max_time_to_limit_switch_seconds   max seconds we expect to reach opssite limit switch, allows us to reserver if motor is stuck
    IBT2Motor(char *p_motor_name, 
        int m_speed_left, int m_speed_right,  
        int p_pwml, int p_pwmr, 
        int p_motor_start_speed, int p_speed_increment,
        int p_stop_delay, int p_stop_delay_every_n_left, int p_stop_delay_every_n_right, 
        unsigned long p_reverse_delay_millis,
        boolean p_needs_jumpstart_left, boolean p_needs_jumpstart_right,
        unsigned long p_jump_millis_duration1, int p_jumpstart_speed1,
        unsigned long p_jump_millis_duration2, int p_jumpstart_speed2,
        unsigned int p_max_time_to_limit_switch_seconds ) :
      motor_name(p_motor_name),
      max_speed_left(m_speed_left),
      max_speed_right(m_speed_right),
      pin_pwml(p_pwml),
      pin_pwmr(p_pwmr),
      motor_start_speed(p_motor_start_speed),
      speed_increment(p_speed_increment),
      stop_delay(p_stop_delay),
      stop_delay_every_n_left(p_stop_delay_every_n_left),
      stop_delay_every_n_right(p_stop_delay_every_n_right),
      reverse_delay_millis(p_reverse_delay_millis),
      needs_jumpstart_left(p_needs_jumpstart_left),
      needs_jumpstart_right(p_needs_jumpstart_right),
      jump_millis_duration1(p_jump_millis_duration1),
      jumpstart_speed1(p_jumpstart_speed1),
      jumpstart_speed2(p_jumpstart_speed2),
      jump_millis_duration2(p_jump_millis_duration2)
      { 
          pinMode(pin_pwmr, OUTPUT);
          digitalWrite(pin_pwmr, 0);
          pinMode(pin_pwml, OUTPUT);
          digitalWrite(pin_pwml, 0);
          
          max_time_to_limit_switch_millis = 1000 * p_max_time_to_limit_switch_seconds;   // number of milliseconds to have passed before we reset motor
          
          Serial.print(F("CONSTRUCTOR::IBT2Motor motor_name"));
          Serial.print(motor_name);
          Serial.print(F(", max_speed_left="));
          Serial.print(max_speed_left);
          Serial.print(F(", max_speed_right="));
          Serial.print(max_speed_right);
          Serial.print(F(", pin_pwml="));
          Serial.print(pin_pwml);
          Serial.print(F(", pin_pwmr="));
          Serial.print(pin_pwmr);
          Serial.print(F(", motor_start_speed="));
          Serial.print(motor_start_speed);
          Serial.print(F(", speed_increment="));
          Serial.println(speed_increment);
          
          Serial.print(F("        stop_delay="));
          Serial.print(stop_delay);
          Serial.print(F(", stop_delay_every_n_left="));
          Serial.print(stop_delay_every_n_left);
          Serial.print(F(", stop_delay_every_n_right="));
          Serial.print(stop_delay_every_n_right);
          Serial.print(F(", reverse_delay_millis="));
          Serial.print(reverse_delay_millis);
          Serial.print(F(", needs_jumpstart_left="));
          Serial.print(bool_tostr(needs_jumpstart_left));
          Serial.print(F(", needs_jumpstart_right="));
          Serial.println(bool_tostr(needs_jumpstart_right));
          
          Serial.print(F("        jump_millis_duration1="));
          Serial.print(jump_millis_duration1);
          Serial.print(F(", jumpstart_speed1="));
          Serial.print(jumpstart_speed1);
          Serial.print(F(", jumpstart_speed2="));
          Serial.print(jumpstart_speed2);
          Serial.print(F(", jump_millis_duration2="));
          Serial.print(jump_millis_duration2);
          Serial.print(F(", max_time_to_limit_switch_millis="));
          Serial.println(max_time_to_limit_switch_millis);

          // initialize some initial state in the motor
          current_pwm_pin = pin_pwml;
          alt_pwm_pin = pin_pwmr;
          motor_is_disabled = false,
          max_speed = 0;
          enable(); 
          go_left(); 
       }
    
  private:
    const char *motor_name;                    // name of this motor printed during status messages
    int pin_pwml;                         // left pin
    int pin_pwmr;                         // right pin  
    int current_pwm_pin;                  // current_pin switches between the two above
    int alt_pwm_pin;                      // the other pin, this pin will be set to 0 while we use the current to drive the motor
    int max_speed;                        // max speed for the current direction, set in go_left and go_right from max_speed_left/right
    int max_speed_left;                   // max speed to go when going right
    int max_speed_right;                  // max speed to go when going left
    bool needs_jumpstart;                 // mapped from needs_jumpstart_left and needs_jumpstart_right in go_left and go_right
    boolean needs_jumpstart_left;         // do we jumpstart when going left
    boolean needs_jumpstart_right;        // do we jumpstart when going right

    boolean motor_is_disabled = 0;        // has disable() been called, cleared by enable()

    unsigned long jump_enabled_millis=0;  // set in run() to the current milliseconds value millis()
    unsigned long jump_diff_millis=0;     // how long has the current jump window been active.
    
    unsigned long jump_millis_duration1;  // number of milliseconds to run jump window 1
    int jumpstart_speed1;                 // jump window 1 max speed, mapped into max_speed during this window
    unsigned long jump_millis_duration2;  // number of milliseconds to run jump window 2, must be larger then window 1 to run. 
    int jumpstart_speed2;                 // jump window 1 max speed, mapped into max_speed during this window, should be less then or equal to jumpstart_speed1

    int stop_delay;                         
    int stop_delay_every_n = 10;                    // when we are stopping the speed after this many itters, delay for stop_delay milliseconds, mapped from n_left, n_right
    unsigned long max_time_to_limit_switch_millis;  // number of seconds to have passed before we reset motor
    unsigned long stop_delay_every_n_left;          // delay after this many itterations going left
    unsigned long stop_delay_every_n_right;         // delay after this many itterations going right
                                                                                     
    unsigned long millis_motor_start=0;   // millis when motor was last started, fixed some bug, review...
    unsigned long millis_since_start=0;   // number of millis since motor was last started, fixed some bug, review...

    int move_left = 0;                    // we are moving left,  move_right should be 0, set in go_left, cleared in go_right, returned by current_direction
    int move_right = 0;                   // we are moving right, move_left should be 0,  set in go_right, cleared go_left, returned by current_direction

    int motor_start_speed;                // speed to start motor at, set in go_left, go_right, also used for jumpstart1, jumpstart2
    int speed_increment;                  // speed increment, set in go_left, go_right, also used for jumpstart1, jumpstart2
    unsigned long reverse_delay_millis;   // number of milliseconds to delay when reversing
};

char *IBT2Motor::get_motor_name() {return motor_name; }

// bring the motor down in a controlled manor
void IBT2Motor::stop() {
  
  boolean motor_was_running = false;

  Serial.println("IBT2Motor::stop");
  #ifdef IS_CHATTY
    Serial.print("debug before stop: current_pwm_pin=");
    Serial.print(current_pwm_pin);
    Serial.print(", alt_pwm_pin=");
    Serial.print(alt_pwm_pin);
    Serial.print(", Speed=");
    Serial.println(speed);
  #endif

  // trim down the motor speed; if speed > motor_start_speed
  for (int i=speed; i!=0; i--) {
    motor_was_running = true;
    analogWrite(current_pwm_pin, i);
    digitalWrite(alt_pwm_pin, 0);
    if ((i % stop_delay_every_n) == 0) {
      Serial.println(i);
      delay(stop_delay);
    }
    if (i < motor_start_speed) { break; }
  }
  digitalWrite(current_pwm_pin, 0);
  digitalWrite(alt_pwm_pin, 0);
  // only do this final dealy when really stoppin the motor
  if (motor_was_running) {
    delay(100);
  }
  speed = 0;

  #ifdef IS_CHATTY
    Serial.print("debug after stop: current_pwm_pin=");
    Serial.print(current_pwm_pin);
    Serial.print(", alt_pwm_pin=");
    Serial.print(alt_pwm_pin);
    Serial.print(", Speed=");
    Serial.println(speed);
  #endif
  
}

void IBT2Motor::start() { 
    
    Serial.println("IBT2Motor::start");
    Serial.print("before start Speed=");
    Serial.println(speed); 
    
    if (speed < motor_start_speed) {
      speed = motor_start_speed;
    }

    Serial.print("after start Speed=");
    Serial.println(speed); 
}

// keep increasing the motor speed until max speed
void IBT2Motor::run() {
  int run_max_speed=0;
  unsigned long current_millis=0;

  #ifdef IS_CHATTY
    Serial.println(F("IBT2Motor::run() starting"));
  #endif

  
  if ( ! is_disabled() ) {
    
    run_max_speed = max_speed; // default the current normal max speed

    // check if we need to override max speed for a jump start 
    if (needs_jumpstart == true) { 
      // this motor is just getting started, let's jump start it!
      if (speed <= motor_start_speed ) { 
        jump_enabled_millis=millis();   // start the jump start duration timer
      }
      
      current_millis = millis();
      // check for millis wraparound
      if (current_millis < jump_enabled_millis) { 
        jump_enabled_millis = current_millis;
      }
      // how long have we been jumpstarting?
      jump_diff_millis = current_millis - jump_enabled_millis;
      
      // are we still in the jump start window?, use the higher speed
      if (jump_diff_millis < jump_millis_duration1) {
        run_max_speed = jumpstart_speed1;
        #ifdef IS_CHATTY
          Serial.print(F("In jump start1"));
        #endif 
      } else if (jump_diff_millis < jump_millis_duration2) {
         run_max_speed = jumpstart_speed2;
        #ifdef IS_CHATTY
          Serial.print(F("In jump start2"));
        #endif       
      } else {
        run_max_speed = max_speed;
      }
   
      #ifdef IS_CHATTY
        Serial.print(F(", jump_enabled_millis="));
        Serial.print(jump_enabled_millis);
        Serial.print(F(", jump_diff_millis="));
        Serial.print(jump_diff_millis);
      #endif
      
    }
    
 
    
    if (speed <= motor_start_speed) {
      millis_motor_start=millis();
    }
    current_millis=millis();
    // guard against millis wrap around
    if (current_millis < millis_motor_start){
      millis_motor_start=current_millis;
    }
    millis_since_start = current_millis -  millis_motor_start; 
    if (millis_since_start > max_time_to_limit_switch_millis) {
      // something is wrong we have taken way too long to reach the current switch
      // reset and let the main routine sort it all out.
      Serial.println(F("WARNING: Taking too long to reach limit switch, restarting motor..."));
      stop();
      start();
      return;
    }
//    Serial.print(F(", run_max_speed="));
//    Serial.println(run_max_speed);
    
    if (speed < run_max_speed) {
      speed += speed_increment;
      if (speed > run_max_speed) { speed = run_max_speed; } 
      digitalWrite(alt_pwm_pin, 0);
      analogWrite(current_pwm_pin, speed);
      if ((speed % 1) == 0) {
        delay(50);
      }
    } else if (speed > run_max_speed) {
      speed -= speed_increment;
      if (speed < run_max_speed) { speed = run_max_speed; } 
      digitalWrite(alt_pwm_pin, 0);
      analogWrite(current_pwm_pin, speed);
      if ((speed % 1) == 0) {
        delay(50);
      }
    }
  }
  #ifdef IS_CHATTH
    Serial.println(F("IBT2Motor::run() end"));
  #endif
}

void IBT2Motor::reverse() {
  if (current_direction() == 'l') {
    go_right(); 
  } else {
    go_left();
  }
}


// continues left if already going left, else stops and restart left
void IBT2Motor::go_left() {
  int orig_speed = speed;
  if (current_direction() != 'l') {
    Serial.println("IBT2Motor::go_left: reversing direction");
    stop();
    move_left = 1;
    move_right = 0;
    current_pwm_pin = pin_pwml;
    alt_pwm_pin = pin_pwmr;
    max_speed = max_speed_left;
    needs_jumpstart=needs_jumpstart_left;
    stop_delay_every_n = stop_delay_every_n_left;
    if (orig_speed != 0) {
      delay(reverse_delay_millis);
      start();
    }
  }
}

// contines right if already going right, else stops and restarts rights
void IBT2Motor::go_right() {
  int orig_speed = speed;
  if (current_direction() != 'r') {
    Serial.println("IBT2Motor::go_right: reversing direction");
    stop();
    move_left = 0;
    move_right = 1;
    current_pwm_pin = pin_pwmr;
    alt_pwm_pin = pin_pwml;
    max_speed = max_speed_right;
    needs_jumpstart=needs_jumpstart_right;
    stop_delay_every_n = stop_delay_every_n_right;
    if (orig_speed != 0) {
      delay(reverse_delay_millis);
      start();
    }  
  }
}

// the current direction of the motor 
char IBT2Motor::current_direction() {
  if (move_left == 1) {
    return 'l';
  } else if (move_right == 1) {
    return 'r';
  } else {
    return '?';
  }
}

void IBT2Motor::enable() {
  if (motor_is_disabled == true) {
    Serial.println("enabeling motor");
    // start();
    motor_is_disabled = false;
  }
}

void IBT2Motor::disable() {
  if (motor_is_disabled == false) {
    Serial.println("disabeling motor");
    stop();
    motor_is_disabled = true;
  }
}

boolean IBT2Motor::is_disabled() {
  return motor_is_disabled;
}

boolean IBT2Motor::is_stopped() {
  return (speed == 0);
}

void IBT2Motor::status() {
  Serial.print(F("IBT2Motor:: motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", is_disabled="));
  Serial.print(bool_tostr(is_disabled()));
  Serial.print(F(", is_stopped="));
  Serial.print(bool_tostr(is_stopped()));
  Serial.print(F(", move_left="));
  Serial.print(move_left);
  Serial.print(F(", move_right="));
  Serial.print(move_right);
  Serial.print(F(", direction="));
  Serial.println(current_direction());
  
  Serial.print(F("        max_speed="));
  Serial.print(max_speed);
  Serial.print(F(", needs_jumpstart="));
  Serial.print(bool_tostr(needs_jumpstart));
  Serial.print(F(", jump_diff_millis="));
  Serial.print(jump_diff_millis);
  Serial.print(F(", max_time_to_limit_switch_millis="));
  Serial.print((unsigned long) max_time_to_limit_switch_millis);
  Serial.print(F(", millis_since_start="));
  Serial.print(millis_since_start);
  Serial.print(F(", speed="));
  Serial.println(speed);  

}

// run the motor by just changing the speed
void IBT2Motor::set_target_speed(unsigned int new_target_speed)
{
  // limit speed to max_speed
  if (new_target_speed > max_speed) {
    Serial.print(F("WARNING: IBT2Motor::set_target_speed speed request of "));
    Serial.print(new_target_speed);
    Serial.print(F(" exceeds max_speed; speed throttled"));
    new_target_speed = max_speed;      
  }
  
  Serial.print(F("IBT2Motor::set_target_speed motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", Speed="));
  Serial.print(speed);
  Serial.print(F(", old_target_speed="));
  Serial.print(max_speed);
  Serial.print(F(", new_target_speed="));
  Serial.println(new_target_speed);

  max_speed = new_target_speed;

//  // make sure our direction pins are both enabled
//  digitalWrite(pin_left, move_left);
//  digitalWrite(pin_right, move_right);
}

unsigned int IBT2Motor::get_target_speed() { return max_speed; }

unsigned int IBT2Motor::get_max_speed() { return max_speed; }

////////////////////// end IBT2Motor //////////////////////////


#endif
