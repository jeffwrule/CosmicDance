
using namespace std;

// PingPong montor that returns to a center switch when switched off

#define SERIAL_SPEED 250000
#define PRINT_EVER_NTH_ITTER 20000    // use with SERIAL_SPEED 250000 
//#define SERIAL_SPEED 9600
//#define PRINT_EVER_NTH_ITTER 1      // may cause lock up of memory with SERIAL_SPEED 250000 (to much logging) if using a monitor window

#define IS_BRIEF True
// #define IS_CHATTY     // define this for extra debug information 

// debug tool, do not leave set for normal user
//#define ALWAYS_BE_DANCING     // uncomment for debug, normally commented out, set remote_is_dancing_ 1 (helps with floating line)
//#define NEVER_BE_DANCING        // uncomment for debug, normally commented out, sets remote_is_dancing_ 0 (helps with floating line)

unsigned long num_loops = 1; // used to limit prints 
boolean do_print = false;   // used to limit prints

String bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

//////// ABSTRACT CLASSES 

///////////////////////////////////////////////////////////
//
// Abstract Class to a single limit switch
//
/////////////////////////////////////////////////////////// 
class GenericLimitSwitch {

  public:
    virtual void update() =0;
    virtual void status() =0;
    virtual String get_limit_type() =0;
    virtual boolean get_is_at_limit() =0;
    virtual char get_limit_position() =0;
    virtual String get_switch_name() =0;

};


///////////////////////////////////////////////////////////
//
// Abstract Class to manage motor half_bridge and full bridge motors
//
/////////////////////////////////////////////////////////// 
class GenericMotor {

  public:
    virtual void stop() =0;
    virtual void start() =0;
    virtual void run() =0;
    virtual void go_left() =0;
    virtual void go_right() =0;
    virtual boolean is_stopped() =0;
    virtual boolean is_disabled() =0;
    virtual char current_direction() =0;
    virtual void enable() =0;
    virtual void disable() =0;
    virtual void status() =0;
    virtual String get_motor_name() =0;
    virtual void new_speed(unsigned int new_target_speed) =0;
};


//////////////////////////////////////////////////////////////
//
// Limit Switch Class Specific Classes..
//
/////////////////////////////////////////////////////////// 
/// Any digital limit switch which just has 0/1, input_pin will be set as INPUT_PULLUP
class DigitalLimitSwitch: public GenericLimitSwitch{

  public:
    void update();
    void status();
    String get_limit_type();
    boolean get_is_at_limit();
    char get_limit_position();
    String get_switch_name();

    // p_siwtch_name            name for this limit switch
    // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    DigitalLimitSwitch (
          String p_switch_name, 
          int p_digital_pin, 
          char p_limit_position, 
          String p_normally_open_closed) :
      switch_name(p_switch_name),
      digital_pin(p_digital_pin),      
      limit_position(p_limit_position)
      { if (limit_position != 'l' && limit_position != 'r' && limit_position != 'c') {
          Serial.print(F("ERROR: Limit Switch "));
          Serial.print(switch_name);
          Serial.println(F(" limit_position="));
          Serial.println(limit_position);
        }
        if (p_normally_open_closed == "NC") {
          limit_off = nc_off;
        } else {
          limit_off = no_off;
        }
        pinMode(digital_pin, INPUT_PULLUP);
        update();
        // print the init and set once values
        Serial.print(F("CONSTRUCTOR::DigitalLimitSwitch p_switch_name="));
        Serial.print(p_switch_name);
        Serial.print(F(", limit_type="));
        Serial.print(limit_type);
        Serial.print(F(", p_digital_pin: "));
        Serial.print(p_digital_pin);
        Serial.print(F(", p_limit_position: "));
        Serial.print(p_limit_position);
        Serial.print(F(", p_normally_open_closed "));
        Serial.print(p_normally_open_closed);
        Serial.print(F(", limit_off "));
        Serial.println(limit_off);
      }
      
  private:

    const String limit_type = "digital";
    boolean is_at_limit=false;
    char limit_position;
    String switch_name;

    const int nc_off=0;   // NC(normally closed), so no push is closed circuit to ground so 0
    const int no_off=1;   // NO(normally open), so not pushed and pulled up to 5v by INPUT_PULLUP resistor

    int digital_pin;
    int limit_off;        // limit switch in 'open'/'not pushed' state
};

String DigitalLimitSwitch::get_limit_type() {return limit_type; }
boolean DigitalLimitSwitch::get_is_at_limit() {return is_at_limit; }
char DigitalLimitSwitch::get_limit_position() {return limit_position; }
String DigitalLimitSwitch::get_switch_name() {return switch_name; };

void DigitalLimitSwitch::update() {
  is_at_limit = !digitalRead(digital_pin) == limit_off;
}

void DigitalLimitSwitch::status() {
  if (! do_print) {
    return;
  }
  Serial.print(F("DigitalLimitSwitch:: switch_name="));
  Serial.print(switch_name);
  Serial.print(F(", position="));
  Serial.print(limit_position);
  Serial.print(F(", is_at_limit: "));
  Serial.println(bool_tostr(is_at_limit));
}

///////// ACS Limit Switch   
/// base on code from here: http://www.microcontroller-project.com/acs712-current-sensor-with-arduino.html
class ACS712LimitSwitch: public GenericLimitSwitch {
  public:
  
    
    void update()=0;
    void status()=0;
    String get_limit_type();
    boolean get_is_at_limit();
    char get_limit_position();
    String get_switch_name();

    ACS712LimitSwitch(int p_analog_pin, String p_switch_name, char p_limit_position, int p_amperage) :
      analog_pin(p_analog_pin), 
      switch_name(p_switch_name),
      limit_position(p_limit_position),
      amperage(p_amperage)
      {
        if (amperage == 30) {
          acs_factor=acs_30a_factor;
        } else if (amperage == 20) {
          acs_factor=acs_20a_factor;
        } else {
          acs_factor=acs_5a_factor;
        }
        pinMode(analog_pin, INPUT);
        update();
        // print the init and set once values
        Serial.print(F("CONSTRUCTOR::ACS712LimitSwitch switch_name="));
        Serial.print(switch_name);
        Serial.print(F(", p_analog_pin="));
        Serial.print(p_analog_pin);
        Serial.print(F(", p_limit_position: "));
        Serial.print(p_limit_position);
        Serial.print(F(", p_amperage "));
        Serial.print(p_amperage);
        Serial.print(F(", acs_factor "));
        Serial.println(acs_factor, 3);
      }

  private:

    const String limit_type = "acs712";   // limit class
    boolean is_at_limit=false;            // have we reached out limit?
    char limit_position;                  // position where this limit is installed r,l,c
    String  switch_name;                  // give this limit a name (for status etc..)

    const float acs_off = 0.10;           // acs cutoff value values betwen -acs_off to acs_off, power is off
    const  float acs_30a_factor = 0.066;   // 30Amp chip factor
    const   float acs_20a_factor = 0.100;  // 20Amp chip factor
    const  float acs_5a_factor = 0.185;    // 5Amp chip factor
    
    int     analog_pin;           // analog pin hooked up to ACS breakout input
    int     amperage;             // amperage rating for ACS, so we can assign the correct acs_factor;
    float   acs_factor;           // constant factor to put into ACS sensor routine
    float   last_acs_value_f=0.0; // previous acs read value
    float   acs_value_f=0.0;      // current acs formula output
};

String ACS712LimitSwitch::get_limit_type() {return limit_type; }
boolean ACS712LimitSwitch::get_is_at_limit() {return is_at_limit; }
char ACS712LimitSwitch::get_limit_position() {return limit_position; }
String ACS712LimitSwitch::get_switch_name() {return switch_name; };

// read the ACS712 HAL censor to see if we have power (at limit or not)
// map the result into SWITCH_OPEN (when button not pushed) the motor is running, SWITCH_CLOSED the motor is off
void ACS712LimitSwitch::update() {
  unsigned int x=0;

  float acs_value=0.0,
        samples=0.0,
        avg_acs=0.0,
        acs_value_f=0.0;

  // sample this value a bunch of times to get an average
  for (int x=0; x < 150; x++) {
    acs_value = analogRead(analog_pin);
    // Serial.print("individual read=");
    // Serial.println(AcsValue);
    samples = samples + acs_value;
    delay(1);
  }
  avg_acs=samples/150.0;
  #ifdef IS_CHATTY
    Serial.print("samples=");
    Serial.println(samples);
    Serial.print("avg_acs=");
    Serial.println(avg_acs);
    Serial.print("Scale up=");
    Serial.println(avg_acs * (5.0/1024.0));
  #endif
  // this is for a 30 AMP sensor (this comment may not be true any more)
  acs_value_f=((avg_acs * (5.0 / 1024.0)) - 2.5)/acs_factor;
  last_acs_value_f = acs_value_f;
  #ifdef IS_CHATTY
    Serial.println(AcsValueF);
    delay(10);
  #endif
  // if AcsValueF is near 0 then the power is off/limit switch was triggered
  if ( acs_value_f >= (acs_off * -1.0) and acs_value_f <= acs_off ) {
    is_at_limit=true;
  } else {
    is_at_limit=false;
  }
}

// print our status
void ACS712LimitSwitch::status() {
  if (! do_print) {
    return;
  }
  Serial.print(F("ACS712LimitSwitch:: switch_name="));
  Serial.print(switch_name);
  Serial.print(F(", limit_position: "));
  Serial.print(limit_position);
  Serial.print(F(", last_acs_value_f "));
  Serial.print(last_acs_value_f, 5);
  Serial.print(F(", acs_value_f "));
  Serial.print(acs_value_f, 5);
  Serial.print(F(", is_at_limit: "));
  Serial.println(bool_tostr(is_at_limit));
}


///////////////////////////////////////////////////////////
//
// Class to manage a collection of limit switches
//  there are three: left, right, center
//  Center switch may be a real limit switch  
//  just a pointer to left or right limit switch
//
///////////////////////////////////////////////////////////

class Limits {
   
   public:
      void update( char motor_direction );
      boolean is_max_left( void );
      boolean is_max_right( void );
      boolean is_centered( void );
      char center_is(void);         // what postion is the center switch l,r,c
      void status();
      char    home_is;            // home position l,r,c (the position of the center_switch)
      int     sum_limits( void ); // number of limit swtiches active (test for errors)

      // p_limits_name        The name of this limits combo
      // p_left_limit         The left limit switch structure
      // p_right_limit        The right limit switch structure
      // p_center_limit       The center limit swtich struct (AKA the home switch) can be a pointer to left_limit or right_limit if the represent home
      Limits(String p_limits_name,
             GenericLimitSwitch* p_left_limit, 
             GenericLimitSwitch* p_right_limit,
             GenericLimitSwitch* p_center_limit) :
        limits_name(p_limits_name),
        left_limit(p_left_limit), 
        right_limit(p_right_limit), 
        center_limit(p_center_limit) 
        { home_is =  center_limit->get_limit_position(); }  
 
   private:
      String limits_name;
      GenericLimitSwitch* left_limit;     //left limit switch 
      GenericLimitSwitch* right_limit;    //left limit switch 
      GenericLimitSwitch* center_limit;   //left limit switch 
      boolean left_limit_active;
      boolean right_limit_active;
      boolean center_limit_active;
};

void Limits::update(char motor_direction) {

  left_limit->update();
  right_limit->update();
  center_limit->update();

  left_limit_active = left_limit->get_is_at_limit();
  right_limit_active = right_limit->get_is_at_limit();
  center_limit_active = center_limit->get_is_at_limit();

  // if we are using an acs for the motor left and right, power will be off when we reach either
  // we must used the current motor direction to work this out.
  if (right_limit->get_limit_type() == "acs712" && left_limit->get_limit_type() == "acs712") {
      if (motor_direction == 'l') {  // was going left
        right_limit_active = false;      // we are not right
      } else { // was going right, must be right
        left_limit_active = false;       // we are not left      
      }
  }

}

int  Limits::sum_limits() {
    int limits_active=0;
    if (left_limit_active) { limits_active++; }
    if (right_limit_active) { limits_active++; }
    // handle case where we re-use the left or right limit for home/center
    if (center_limit_active && center_limit->get_limit_position() == 'c') { limits_active++; }
    return limits_active;
}

boolean Limits::is_max_left() {
    return left_limit_active;
}

boolean Limits::is_max_right() {
    return right_limit_active;
}

boolean Limits::is_centered() {
  return center_limit_active;
}

char Limits::center_is() {
  return center_limit->get_limit_position();
}


void Limits::status() {
  if (! do_print) {
    return;
  }
  // dump the limit switch info
  Serial.print(F("L_LimitSwitch: "));
  left_limit->status();
  Serial.print(F("R_LimitSwitch: "));
  right_limit->status();
  Serial.print(F("C_LimitSwitch: "));
  center_limit->status();

  // dump the Limits info (group info)
  Serial.print(F("Limits:: limits_name="));
  Serial.print(limits_name);
  Serial.print(F(", is_max_left="));
  Serial.print(bool_tostr(is_max_left()));
  Serial.print(F(" is_centered="));
  Serial.print(bool_tostr(is_centered()));
  Serial.print(F(", is_max_right="));
  Serial.println(bool_tostr(is_max_right()));
}


////////////////////// end Limits //////////////////////////

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
    void go_left();
    void go_right();
    boolean is_stopped();
    boolean is_disabled();
    char current_direction();
    void enable();
    void disable();
    void status();
    String get_motor_name();
    void new_speed(unsigned int new_target_speed);

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
    HBridgeMotor(String p_motor_name, int m_speed_left,int  m_speed_right, int p_speed, int p_standby, int p_left, int p_right,
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
          digitalWrite(p_left, 0);
          pinMode(p_right, OUTPUT);
          digitalWrite(p_right, 0);
          pinMode(p_speed, OUTPUT);
          digitalWrite(p_speed, 0);
          pinMode(p_standby, OUTPUT);
          digitalWrite(p_standby, 0);

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
    String motor_name;                  // name of this motor for status messages
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

String HBridgeMotor::get_motor_name() {return motor_name; }

// run the motor by just chaning the speed
void HBridgeMotor::new_speed(unsigned int new_target_speed)
{
  Serial.print(F("HBridgeMotor::new_speed motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", Speed="));
  Serial.print(speed);
  Serial.print(F(", new_speed="));
  Serial.println(new_target_speed);

  speed = new_target_speed;

  // make sure our direction pins are both enabled
  digitalWrite(pin_left, move_left);
  digitalWrite(pin_right, move_right);
}

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
      Serial.print(F("speed INCREASED to: "));
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

// continues left if already going left, else stops and restart left
void HBridgeMotor::go_left() {
  int orig_speed = speed;
  if (current_direction() != 'l') {
    Serial.print(F("HBridgeMotor::go_left: motor_name="));
    Serial.print(motor_name);
    Serial.println(F(" reversing direction"));
    
    stop();
    move_left = 1;
    move_right = 0;
    max_speed = max_speed_left;
    if (orig_speed != 0) {
      delay(reverse_delay_millis);
      new_speed(target_speed);
    }
  }
}

// contines right if already going right, else stops and restarts rights
void HBridgeMotor::go_right() {
  int orig_speed = speed;
  if (current_direction() != 'r') {
    Serial.print(F("HBridgeMotor::go_right: motor_name="));
    Serial.print(motor_name);
    Serial.println(F(" reversing direction"));
    stop();
    move_left = 0;
    move_right = 1;
    max_speed = max_speed_right;
    if (orig_speed != 0) {
      delay(reverse_delay_millis);
      new_speed(target_speed);
    }  
  }
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
  digitalWrite(pin_standby, 1);
  motor_is_disabled = false;
}

void HBridgeMotor::disable() {
//  if (motor_is_disabled == false) {
//    Serial.println("disabeling motor");
//    stop();
//    digitalWrite(pin_standby, 0);
//    motor_is_disabled = true;
//  }
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
    void go_left();
    void go_right();
    boolean is_stopped();
    boolean is_disabled();
    char current_direction();
    void enable();
    void disable();
    void status();
    int speed = 0;
    String get_motor_name();

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
    IBT2Motor(String p_motor_name, 
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
          disable(); 
          go_left(); 
       }
    
  private:
    String motor_name;                    // name of this motor printed during status messages
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

String IBT2Motor::get_motor_name() {return motor_name; }

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

  // trim down the motor speed; if speed > 0
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
    
    if (do_print) {
      Serial.print(F("IBT2Motor::run() speed="));
      Serial.print(speed);
      Serial.print(F(", max_speed="));
      Serial.println(run_max_speed);
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

////////////////////// end IBT2Motor //////////////////////////


///////////////////////////////////////////////////////////
//
// Class to manage remote dancer stack input 
//
/////////////////////////////////////////////////////////// 
class Dancer {

  public:

    void update();                      // update the pin state
    void dance();                       // check the limits and motor setting and decide how to direct the motor
    void status();                      // dump the Dancer status

    // p_dancer_name  name of this dancer, for status messages
    // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    // p_motor        pointer to our motor structure
    // p_left_limit   left limit switch
    // p_right_limit  right limit switch
    // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    // p_dancer_delay_seconds   Number of seconds to delay motor start after remote starts dancing
    // p_dancer_extend_seconds  Number of seconds to run motor after center resting position has been reached
    Dancer( String p_dancer_name,
            int p_dancer_pin, 
            GenericMotor *p_motor, 
            GenericLimitSwitch *p_left_limit,
            GenericLimitSwitch *p_right_limit,
            GenericLimitSwitch *p_center_limit,
            int p_dancer_delay_seconds,
            int p_dancer_extend_seconds) : 
      dancer_name(p_dancer_name),
      dancer_pin(p_dancer_pin), 
      motor(p_motor)
      { 
        pinMode(dancer_pin, INPUT);

        is_home = false;
        is_centered = false;
        remote_is_dancing = false;
        center_passed = false;
        
        delay_dance_millis = 1000L * p_dancer_delay_seconds;
        start_delay_complete = false;
        extend_dance_millis = 1000L * p_dancer_extend_seconds;
        is_extended = false;

        current_limits = new Limits(
            String("L_chair_limits"), // p_limits_name        The name of this limits combo
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
        Serial.print(motor->get_motor_name());
        Serial.print(F(", left_limit_name="));
        Serial.print(p_left_limit->get_switch_name());
        Serial.print(F(", right_limit_name="));
        Serial.print(p_right_limit->get_switch_name());
        Serial.print(F(", center_limit_name="));
        Serial.println(p_center_limit->get_switch_name());
        Serial.print(F("        delay_dance_millis="));
        Serial.print(delay_dance_millis);
        Serial.print(F(", extend_dance_millis="));
        Serial.print(extend_dance_millis);
        Serial.print(F(", center_is="));
        Serial.println(center_is);       

        update();       // lets set the pins for the first time
      }
      

  private:
    String dancer_name;                 // dancer name
    char home_position;                 // home is what position l,r,c (left, right, center)  
    Limits *current_limits;             // left right center limits for this dancer
    GenericMotor *motor;                // the motor attached to this dancer,  A dancer can only control one motor so we have two dancers
    int     dancer_pin;                 // pin to check for remote dancer
    boolean remote_is_dancing;          // is the remote asking for us to dance?
    char    center_is;                  // what positoin is the center position l,r,c
    boolean is_centered;                // have we reached the center switch after remote stopped dancing
    boolean center_passed;              // have we passed the center
    boolean is_home;                    // did we reach home while remote_is_dancing=false and extend our dance?
    long    current_millis;             // number of milliseconds at last update call
    boolean dance_just_started;         // signals a new dance was started (remote_is_dancing switched to state on);
    boolean dance_just_ended;            // a dance just completed (remote_is_dancing switched to state off)
    
    unsigned long delay_dance_millis;    // number of millis to delay after remote_is_dancing is set;
    unsigned long delay_start_millis;    // time that the remote started dancing
    boolean start_delay_complete;       // have enough milliseconds passed to complete the start delay

    unsigned long extend_dance_millis;   // number of seconds to extend the dance remote stops dancing
    unsigned long extend_start_millis;   // millisonds when we are finally home (extend beyond home position)
    boolean is_extended;                // run a bit longer after we are home (to strech out fabric of instance)

};


void Dancer::update() {

  int current_remote_status;
  
  // check our remote status
  current_remote_status = (digitalRead(dancer_pin) == 1);

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
    } else {
      dance_just_ended=true;              // signal the dance() routine we had a status change
      Serial.print(F("Dancer::update "));
      Serial.print(dancer_name);
      Serial.println(F(" remote dancer STOPPED dancing"));
    }
  }

  remote_is_dancing = current_remote_status;
  current_millis = millis();
}

void Dancer::dance() {
  
  boolean reverse_motor=false;  // assume we are going the correct way before we check

  if (current_limits->sum_limits() > 1) {
    if (!motor->is_stopped()) {
      motor->stop();    
    }
    if (do_print) {
          Serial.print(F("ERROR: Dancer::dance() dancer_name="));
          Serial.print(dancer_name);
          Serial.print(F(" too_many_limits_active stopping motors"));
    }
    return;
  }

  //////////////////////////////////////////////
  // remote is dancing, first we delay then we really start to dance
  //////////////////////////////////////////////
  if (remote_is_dancing == true) {
    
    // remote started dancing, but we are home, so we need clear
    // the wind up starts: delay dance if necessary
    if (dance_just_started) {
      dance_just_started=false;
      is_home=false;
      is_extended=false;
      start_delay_complete=false;
      delay_start_millis = current_millis;
      Serial.print(F("Dancer::dance() dancer_name="));
      Serial.print(dancer_name);
      Serial.println(F(" was was HOME, starting a whole new dance now"));
      Serial.print(F("Dancer::dance() dancer_name="));
      Serial.print(dancer_name);
      Serial.print(F(" was was START_DELAY BEGINNING delay_dance_seconds="));
      Serial.println(delay_dance_millis/1000);
    }

    // check if the extend delay has passed
    if (current_millis - delay_start_millis >= delay_dance_millis) {
      if (!start_delay_complete) {
        start_delay_complete = true;
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name);
        Serial.println(F(" START_DELAY COMPLETE!"));
      }
    }
  }

  //////////////////////////////////////////////
  // wind it down, 
  // first get to center, then extend dance
  //////////////////////////////////////////////
  if (remote_is_dancing == false) {

    start_delay_complete = true;  // we don't delay when remote stops dancing

    if (dance_just_ended) {
      dance_just_ended = false;
      is_centered = false;
      is_extended = false; 
      Serial.print(F("Dancer::dance() dancer_name="));
      Serial.print(dancer_name); 
      Serial.println(F(" CENTERING!")); 
    }
    
    // are we centered yet? 
    if (!is_centered) {
      // have we made to centered postion?
      if (current_limits->is_centered()) {
        is_centered=true;
        extend_start_millis = current_millis; 
        Serial.print(F("Dancer::dance "));
        Serial.print(dancer_name);
        Serial.println(" is now centered!");     
        if (center_is != 'c') { // setup the motor for the extend operation
          reverse_motor= true;      
        }
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name); 
        Serial.println(F(" CENTERING COMPLETE!")); 
        Serial.print(F("Dancer::dance() dancer_name="));
        Serial.print(dancer_name); 
        Serial.print(F(" EXTENDING SECONDS=")); 
        Serial.println(extend_dance_millis/1000);
      }
    }
    
    // still not centered, are we moving in the correct direction?
    if (!is_centered) {
      // are we going the wrong direction?
      if (center_is == 'c' && center_passed) {
        reverse_motor=true;
      } else if (motor->current_direction() != center_is) { // center is l/r and we are not going in the direction, then go back
        reverse_motor=true;
      }
    }

    // we are centered, now check if we are extended
    if (is_centered && !is_extended) {
      if (current_millis - extend_start_millis >= extend_dance_millis) {
        is_extended = true;
        is_home = true;   // this stops the motor
        Serial.print(F("Dacner::dance ")); 
        Serial.print(dancer_name);
        Serial.println(F(" EXTEND COMPLETE, IS_HOME=true"));
      }
    }
  }
    
  //////////////////////////////////////////////
  // run the motor unless have successfull come home
  //////////////////////////////////////////////
  if (is_home) {
    if (!motor->is_stopped()) {
      Serial.println("Dancer::dance is_home=true, stopping motor...");
      motor->stop();
    }
  } else { // not home so keep running the motor
    if (start_delay_complete) {
      
      if (current_limits->is_centered()) {
        center_passed = true;
      }

      // if revsrse called for from above do it.
      if (reverse_motor == true) {
        center_passed = !center_passed; // reversing, if we have not passsed center, now we have else it is still ahead
        if (motor->current_direction() == 'l') {
          motor->go_right();
        } else {
          motor->go_left();
        }
      }
      
      if (current_limits->is_max_left()) {
        motor->go_right();
        center_passed = false;
      }
      
      if(current_limits->is_max_right()) {
        motor->go_left();
        center_passed = false;
      }
  
      if (motor->is_stopped()) {
        motor->start();
      }

      motor->run();   // this takes care of things like variable speeds etc
            
    } // start_delay == true
  }  // is_home == false
}

void Dancer::status() {
  if (! do_print) {
    return;
  }
  Serial.println("----- Dancer status() Group Start ---");
  motor->status();
  current_limits->status();
  Serial.print(F("Dancer: name="));
  Serial.print(dancer_name);
  Serial.print(F(", remote_is_dancing="));
  Serial.print(bool_tostr(remote_is_dancing));
  Serial.print(F(", is_home="));
  Serial.print(bool_tostr(is_home));
  Serial.print(F(", is_centered="));
  Serial.print(bool_tostr(is_centered));
  Serial.print(F(", center_passed="));
  Serial.print(bool_tostr(center_passed)); 
  Serial.print(F(", is_extended="));
  Serial.println(bool_tostr(is_extended));
  
  Serial.print(F("        current_millis="));
  Serial.print(current_millis);  
  Serial.print(F(", start_delay_complete="));
  Serial.print(bool_tostr(start_delay_complete));  
  Serial.print(F(", delay_dance_millis="));
  Serial.print(delay_dance_millis); 
  Serial.print(F(", delay_start_millis-current_millis="));
  Serial.println(delay_start_millis-current_millis);
  
  Serial.print(F("        is_extended="));
  Serial.print(bool_tostr(is_extended)); 
  Serial.print(F(", extend_start_millis="));
  Serial.print(extend_start_millis);  
  Serial.print(F(", extend_start_millis-current_millis="));
  Serial.print(extend_start_millis-current_millis);
  Serial.print(F(", dance_just_started="));
  Serial.print(bool_tostr(dance_just_started));
  Serial.print(F(", dance_just_ended="));
  Serial.println(bool_tostr(dance_just_ended));

}

////////////////////// end Dancer //////////////////////////

// Suggested pin layout, but some older scupltures will have different layouts, check the older code for the exact layout in github
#define M1_LEFT_LIMIT_PIN 2      // input from left limit switch, expects a NO (normally open) switch
#define M1_RIGHT_LIMIT_PIN 3     // input from right limit switch, expects a NO (normally open) switch
#define M1_CENTER_LIMIT_PIN A0   // for digital or analog devices
#define M1_ACS712_LIMIT_PIN A1   // using with ACS271

// Sparkfun TB6612FNG (class HBridgeMotor) Motor1
#define M1_HGRIDGE_LEFT_PIN 4    // output to move left pin on h-bridge shield
#define M1_HGRIDGE_RIGHT_PIN 5   // output to move right pin on h-bridge shield
#define M1_HGRIDGE_PWM_PIN 6     // output speed/on-off must be a PWM pin

// pins to control the motor IBT_2  (class IBT2Motor) Motor1
#define M1_IBT2_PWMR  5   // PWM pin from 0 - 1024 when > 0 moves montor to Right (left must be 0)
#define M1_IBT2_PWML  6   // PWM pin from 0 - 2014 when > 0 moves motor to Left (right must be 0)


/////// MOTOR 2 PINS FOR
#define M2_LEFT_LIMIT_PIN 7      // input from left limit switch, expects a NO (normally open) switch
#define M2_RIGHT_LIMIT_PIN 8     // input from right limit switch, expects a NO (normally open) switch
#define M2_CENTER_LIMIT_PIN A2   // for digital or analog devices
#define M2_ACS712_LIMIT_PIN A3   // using with ACS271

// Sparkfun TB6612FNG (class HBridgeMotor) Motor1
#define M2_HGRIDGE_LEFT_PIN 9    // output to move left pin on h-bridge shield
#define M2_HGRIDGE_RIGHT_PIN 10   // output to move right pin on h-bridge shield
#define M2_HGRIDGE_PWM_PIN 11     // output speed/on-off must be a PWM pin

// pins to control the motor IBT_2  (class IBT2Motor) Motor1
#define M2_IBT2_PWMR  9   // PWM pin from 0 - 1024 when > 0 moves montor to Right (left must be 0)
#define M2_IBT2_PWML  10   // PWM pin from 0 - 2014 when > 0 moves motor to Left (right must be 0)

#define DANCER_INPUT_PIN A4     // pin to read dancer input
#define HBRIDGE_MOTOR_STDBY 12  // enable the motor card HBRIDGE only use for both MOTOR1 and MOTOR2

GenericMotor *my_motor1;
Dancer *my_dancer1;
GenericLimitSwitch *m1_left_limit_switch;
GenericLimitSwitch *m1_right_limit_switch;
GenericLimitSwitch *m1_center_limit_switch;

GenericMotor *my_motor2;
Dancer *my_dancer2;
GenericLimitSwitch *m2_left_limit_switch;
GenericLimitSwitch *m2_right_limit_switch;
GenericLimitSwitch *m2_center_limit_switch;

#define NUM_DANCERS 1
// Dancer* dancers[NUM_DANCERS] = {my_dancer1, my_dancer2};
Dancer* dancers[NUM_DANCERS];

void setup() {

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("\nStart setup"));
  Serial.println(F("PingPongMotorCentered.ino"));
  
  pinMode(HBRIDGE_MOTOR_STDBY, OUTPUT);
  digitalWrite(HBRIDGE_MOTOR_STDBY, LOW);

  pinMode(DANCER_INPUT_PIN, INPUT);

  // SITX2 piece
  my_motor1 = new HBridgeMotor(    
    String("L_chair"),            // p_motor_name               name of this motor (for status messages)
    255,                   // m_speed_left               max speed when going left
    255,                   // m_speed_right              max_speed when going right
    M1_HGRIDGE_PWM_PIN,    // p_speed                    pwm speed pin
    HBRIDGE_MOTOR_STDBY,   // p_standby                  stand by pin (high=enabled, low=disabled)
    M1_HGRIDGE_LEFT_PIN,   // p_left                     pin for left direction 
    M1_HGRIDGE_RIGHT_PIN,  // p_right                    pin for right direction
    100,                   // p_motor_start_speed        start() uses this as the first pmw speed when starting, must be *LESS* then m_speed_left, m_speed_right
    25,                     // p_speed_increment          increment the pmw by this ammount when starting the motor
    50,                    // p_speed_up_increment_delay   delay to add between increments to make speedup smoother
    50,                     // p_slow_down_increment_delay  delay to add between increments to make slowdown smoother
    500                    // p_reverse_delay_millis     wait this many milliseconds when reversing direction 
    );

  // SITX2 piece
  m1_left_limit_switch = new DigitalLimitSwitch(
    String("L_chair_left_limit"), // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    String("NC")  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
  m1_right_limit_switch = new DigitalLimitSwitch(
    String("L_chair_right_limit"), // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_RIGHT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'r',  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    String("NC")  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
  m1_center_limit_switch = m1_left_limit_switch; // the left limit is the center/home

  
  // SITX2 piece
  my_dancer1 = new Dancer(
    String("L_chair"),       // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,       // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor1,              // p_motor        pointer to our motor structure
    m1_left_limit_switch,   // p_left_limit   left limit switch
    m1_right_limit_switch,  // p_right_limit  right limit switch
    m1_center_limit_switch, // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    0,                      // p_dancer_delay_seconds   Number of seconds to delay motor start after remote starts dancing
    0                       // p_dancer_extend_seconds  Number of seconds to run motor after center resting position has been reached
    );

    dancers[0] = my_dancer1;

    do_print = true;      // make sure the calls to status prints 
    my_dancer1->update();
    my_dancer1->status();
    
    Serial.println(F("End Setup"));

}

// only print every so often, becaus it is really to much every loop...
void slow_down_prints() {
  num_loops += 1;
  if ((num_loops % PRINT_EVER_NTH_ITTER) == 0) {
    do_print = true;
    num_loops = 1; // stop roll over
  } else {
    do_print = false;
  }
}

// reset the arduino
//void(* resetFunc) (void) = 0;

// watch for limits and request to enable/disable the motor
void loop() {
  
  slow_down_prints();
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->update();
      dancers[i]->status();
  }
  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->dance();
    }
}

