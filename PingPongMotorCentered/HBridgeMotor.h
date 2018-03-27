#ifndef HBRIDGEMOTOR_H
#define HBRIDGEMOTOR_H

#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/GenericMotor.h>

///////////////////////////////////////////////////////////
//
// Class to manage motor H-BRIDGE break out board 
// SparkFun Part: TB6612FNG (1A)
//
/////////////////////////////////////////////////////////// 
class HBridgeMotor: public GenericMotor {
  public:
    void stop();
    void start();
    void run();
    void reverse();
    void go_left();
    void go_right();
    boolean is_stopped();
    boolean is_disabled();
    char current_direction();
    void enable();
    void status();
    char* get_motor_name();
    void set_target_speed(unsigned int new_target_speed);
    unsigned int get_target_speed();
    unsigned int get_max_speed();

    // ARCHIVED:
    // p_stop_delay               delay this many milliseconds every_nth itteration (starting and stopping), smooths out start and stop
    // p_stop_delay_every_n_left  during start() delay(p_stop_delay) after this many increments
    // p_stop_delay_every_n_right during stop() delay(p_stop_delay) after this many increments

    
    // p_motor_name                 name of this motor (for status messages)
    // m_speed_left                 max speed when going left
    // m_speed_right                max_speed when going right
    // p_speed                      pwm speed pin
    // p_standby                    stand by pin (high=enabled, low=disabled)
    // p_left                       pin for left direction 
    // p_right                      pin for right direction
    // p_motor_start_speed          start() uses this as the first pmw speed when starting, must be *LESS* then m_speed_(left|right)
    // p_speed_increment            increment the pmw by this ammount when starting the motor
    // p_speed_up_increment_delay   delay to add between increments to make speedup smoother
    // p_slow_down_increment_delay  delay to add between increments to make slowdown smoother
    // p_reverse_delay_millis       wait this many milliseconds when reversing direction   
    HBridgeMotor(const char* p_motor_name, int m_speed_left,int  m_speed_right, int p_speed, int p_standby, int p_left, int p_right,
        int p_motor_start_speed, int p_speed_increment, 
        int p_speed_up_increment_delay, int p_slow_down_increment_delay,
        unsigned long p_reverse_delay_millis) :
      motor_name(p_motor_name),
      max_speed_left(m_speed_left), 
      max_speed_right(m_speed_right),
      pin_speed(p_speed),
      pin_standby(p_standby),
      pin_left(p_left),
      pin_right(p_right),
      motor_start_speed(p_motor_start_speed),
      speed_increment(p_speed_increment),
      speed_up_increment_delay(p_speed_up_increment_delay),
      slow_down_increment_delay(p_slow_down_increment_delay),
      reverse_delay_millis(p_reverse_delay_millis)
      { 
          pinMode(p_left, OUTPUT);
          digitalWrite(p_left, LOW);
          pinMode(p_right, OUTPUT);
          digitalWrite(p_right, LOW);
          pinMode(p_speed, OUTPUT);
          digitalWrite(p_speed, LOW);
          pinMode(p_standby, OUTPUT);
          digitalWrite(p_standby, LOW);

          Serial.print(F("CONSTRUCTOR::HBridgeMotor: motor_name="));
          Serial.print(motor_name);
          Serial.print(F(", max_speed_left="));
          Serial.print(max_speed_left);
          Serial.print(F(", max_speed_right="));
          Serial.print(max_speed_right);
          Serial.print(F(", pin_speed="));
          Serial.print(pin_speed);
          Serial.print(F(" pin_left="));
          Serial.print(pin_left);
          Serial.print(F(", pin_right="));
          Serial.print(pin_right);
          Serial.print(F(", pin_standby="));
          Serial.println(pin_standby);
          Serial.print(F("        motor_start_speed="));
          Serial.print(motor_start_speed);
          Serial.print(F(", speed_increment="));
          Serial.print(speed_increment);
          Serial.print(F(", speed_up_increment_delay="));
          Serial.print(speed_up_increment_delay);          
          Serial.print(F(", slow_down_increment_delay="));
          Serial.print(p_slow_down_increment_delay);
          Serial.print(F(", reverse_delay_millis="));
          Serial.println(reverse_delay_millis);

          // set other local variables
          target_speed=0;
          enable();
          go_left();
          Serial.println(F("end CONSTRUCTOR::HBridgeMotor"));
       }
    
  private:
    const char* motor_name;                  // name of this motor for status messages
    int pin_speed;                      // motor pmw speed pin
    int pin_left;                       // move motor left pin (HIGH=enabled) 
    int pin_right;                      // move motor right pin (HIGH=enabled)
    int pin_standby;                    // the standby pin
    int max_speed;                      // current max speed, coppied speed left and right
    int max_speed_left;                 // max speed left
    int max_speed_right;                // max speed right
    boolean motor_is_disabled = true;  // is the motor disabled?

    int move_left = 0;        // move left enable/disable
    int move_right = 0;       // move right enable/disable
    int speed = 0;            // pmw speed
    int target_speed=0;       // new target PMW speed
    
    // new variables this edit
    int speed_up_increment_delay;         // delay after each increment when speeding up
    int slow_down_increment_delay;        // delay after each increment when slowing down
//    int stop_delay;                       // milliseconds to after each n itterations
//    int stop_delay_every_n_left;          // delay after this many itternations
//    int stop_delay_every_n_right;         // delay after this many itternations
//    int stop_delay_every_n = 50;          // delay this many milliseconds after each n itterations on stop, set from n_left and n_right
    int motor_start_speed;                // speed to use on start(), must be *LESS* then max_speed_(left|right)
    int speed_increment;                  // increment for increasing pwm speed
    unsigned long reverse_delay_millis;   // milliseconds to delay on reverse

};

char* HBridgeMotor::get_motor_name() {return motor_name; }

// run the motor by just chaning the speed
void HBridgeMotor::set_target_speed(unsigned int new_target_speed)
{
  // limit speed to max_speed
  if (new_target_speed > max_speed) {
    Serial.print(F("WARNING: HBridgeMotor::set_target_speed speed request of "));
    Serial.print(new_target_speed);
    Serial.print(F(" exceeds max_speed; speed throttled"));
    new_target_speed = max_speed;      
  }
  
  Serial.print(F("HBridgeMotor::set_target_speed motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", Speed="));
  Serial.print(speed);
  Serial.print(F(", old_target_speed="));
  Serial.print(target_speed);
  Serial.print(F(", new_target_speed="));
  Serial.println(new_target_speed);

  target_speed = new_target_speed;

  // make sure our direction pins are both enabled
  digitalWrite(pin_left, move_left);
  digitalWrite(pin_right, move_right);
}

unsigned int HBridgeMotor::get_target_speed() { return target_speed; }

unsigned int HBridgeMotor::get_max_speed() { return max_speed; }

// bring the motor down in a controlled manor
void HBridgeMotor::stop() {
  
  boolean motor_was_running = false;

  Serial.print(F("HBridgeMotor::stop motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", speed="));
  Serial.print(speed);
  Serial.println(F(", new_target_speed=0"));

  target_speed=0;
  while (speed != 0 ) { run(); }

}

void HBridgeMotor::start() { 
    
    Serial.print(F("HBridgeMotor::start motor_name="));
    Serial.print(motor_name);
    Serial.print(F(", speed="));
    Serial.print(speed); 
    Serial.print(F(", target_speed="));
    Serial.println(max_speed);
    
    target_speed=max_speed;

    // make sure our direction pins are both enabled
    digitalWrite(pin_left, move_left);
    digitalWrite(pin_right, move_right);
    run();
}

// keep increasing/decrease the motor speed until target_speed reached
void HBridgeMotor::run() {
  
  if ( ! is_disabled() ) {

    // make sure our pins are active if we are going to changing speed
    if (speed != target_speed) {
      #ifdef IS_CHATTY
        Serial.print(F("HBridgeMotor::run() not at speed, pin_left="));
        Serial.print(pin_left);
        Serial.print(F(", move_left="));
        Serial.print(move_left);
        Serial.print(F(", pin_right="));
        Serial.print(pin_right);
        Serial.print(F(", move_right="));
        Serial.println(move_right);
      #endif
      digitalWrite(pin_left, move_left);
      digitalWrite(pin_right, move_right);
    }
    
    // some bounds checking on target speed
    if (target_speed > 0 && target_speed < motor_start_speed) { target_speed = motor_start_speed; } // can't have a target (other then 0, below start speed 
    if (target_speed > max_speed) {target_speed = max_speed; }                          // can't have a target speed greater then max_speed
    if (target_speed < 0) { target_speed = 0; }                                         // can't have a target < 0

    // increment speed
    if (speed < target_speed) {
      speed += speed_increment;
      if (speed < motor_start_speed) { speed = motor_start_speed; }   // always start at a minimum here
      if (speed > target_speed) { speed = target_speed; } // don't go above max
      analogWrite(pin_speed, speed);
      delay(speed_up_increment_delay);
      Serial.print(F("HBridgeMotor::run motor_name="));
      Serial.print(motor_name);
      Serial.print(F(" speed INCREASED to: "));
      Serial.println(speed);
    }

    // decrement speed
    if (speed > target_speed) {
      speed -= speed_increment;
      if (speed < motor_start_speed) { speed = 0; }
      analogWrite(pin_speed, speed);
      delay(slow_down_increment_delay);
      Serial.print(F("HBridgeMotor::run motor_name="));
      Serial.print(motor_name);
      Serial.print(F("speed DECREASED to: "));
      Serial.println(speed);
    }
  }
}

void HBridgeMotor::reverse() {
  if (current_direction() == 'l') {
    go_right(); 
  } else {
    go_left();
  }
}

// continues left if already going left, else stops and restart left
void HBridgeMotor::go_left() {
  int orig_speed = speed;
  int orig_target = target_speed;
  if (current_direction() != 'l') {
    Serial.print(F("HBridgeMotor::go_left: motor_name="));
    Serial.print(motor_name);
    Serial.print(F(", current_speed="));
    Serial.print(speed);
    Serial.print(F(", current_target_speed="));
    Serial.print(target_speed);
    Serial.println(F(" reversing direction"));

    if (speed > 0 ) { stop(); }
    move_left = 1;
    move_right = 0;
    max_speed = max_speed_left;
    if (orig_speed != 0) { delay(reverse_delay_millis); }
    set_target_speed(orig_target);
  }
  Serial.println(F("HBridgeMotor::go_left: end"));
}

// contines right if already going right, else stops and restarts rights
void HBridgeMotor::go_right() {
  int orig_speed = speed;
  int orig_target = target_speed;
  if (current_direction() != 'r') {
    Serial.print(F("HBridgeMotor::go_right: motor_name="));
    Serial.print(motor_name);
    Serial.print(F(", current_speed="));
    Serial.print(speed);
    Serial.print(F(", current_target_speed="));
    Serial.print(target_speed);
    Serial.println(F(" reversing direction"));
    if (speed > 0 ) { stop();}
    move_left = 0;
    move_right = 1;
    max_speed = max_speed_right;
    if (orig_speed != 0) { delay(reverse_delay_millis); }
    set_target_speed(orig_target); 
  }
  Serial.println(F("HBridgeMotor::go_right: end"));
}

// the current direction of the motor 
char HBridgeMotor::current_direction() {
  if (move_left == 1) {
    return 'l';
  } else {
    return 'r';
  }
}

void HBridgeMotor::enable() {
  Serial.print(F("HBridgeMotor::enable() motor_name="));
  Serial.println(motor_name);
  digitalWrite(pin_standby, HIGH);
  motor_is_disabled = false;
}

boolean HBridgeMotor::is_disabled() {
  return motor_is_disabled;
}

boolean HBridgeMotor::is_stopped() {
  return (speed == 0);
}

// print our motor status (moving values)
void HBridgeMotor::status() {
  Serial.print(F("HBridgeMotor:: motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", is_disabled="));
  Serial.print(bool_tostr(is_disabled()));
  Serial.print(F(", move_left="));
  Serial.print(move_left);
  Serial.print(F(", move_right="));
  Serial.print(move_right);
  Serial.print(F(", is_stopped="));
  Serial.print(bool_tostr(is_stopped()));
  Serial.print(F(", direction="));
  Serial.print(current_direction());
  Serial.print(F(", max_speed="));
  Serial.print(max_speed);
  Serial.print(F(", target_speed="));
  Serial.print(target_speed);  
  Serial.print(F(", speed="));
  Serial.println(speed);  
}

////////////////////// end HBridgeMotor //////////////////////////


#endif
