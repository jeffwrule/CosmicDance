
using namespace std;

// PingPong montor continues until it reaches any limit switch to center.

#define DEBUG = true
//#define PRINT_EVER_NTH_ITTER 15000
// #define PRINT_EVER_NTH_ITTER 30000

#define PRINT_EVER_NTH_ITTER 1

// because magnets are impprecise, add specific delays for different directions.
// final delays for cloud gate build
//#define CENTER_MOVING_LEFT_DELAY 9000
//#define CENTER_MOVING_RIGHT_DELAY 1

// expected delays for normal analog line (white switch) switch Sparkfun QRE1113 (Analog)
//#define CENTER_MOVING_LEFT_DELAY 1
//#define CENTER_MOVING_RIGHT_DELAY 1

#define CENTER_READ_CUTOFF 250    // known to work with LIFE piece.

// #define MOTOR_SPEED 72        // values between 0 (off) and 255 (fully on) (normal production speed)
//#define MOTOR_SPEED 130        // values between 0 (off) and 255 (fully on) a little fast for LIFE
//#define MOTOR_SPEED 200        // values between 0 (off) and 255 (fully on)  (testing speed)

// heaven and earth
#define MOTOR_SPEED_LEFT  60        // GO DOWN: values between 0 (off) and 255 (fully on) This is for heaven and earth
#define MOTOR_SPEED_RIGHT 60        // GO UP: values between 0 (off) and 255 (fully on) This is for heaven and earth
#define MAX_SECONDS_TO_LIMIT_SWITCH 150 // max time we should ever expect to reach either limit switch

#define MOTOR_JUMP_START_SPEED1 70       // IBT_2 needs more power to get going (up) then the normal run speed (heaven and earth)
#define JUMP_START_MILLIS_DURATION1 100  // IBT_2 motor will run left (up) speed for this many seconds when starting left
#define MOTOR_JUMP_START_SPEED2 60       // IBT_2 needs more power to get going (up) then the normal run speed (heaven and earth)
#define JUMP_START_MILLIS_DURATION2 100  // IBT_2 motor will run left (up) speed for this many seconds when starting left

#define MOTOR_START_SPEED  20       // low values don't produce movement but values *MUST BE*  MOTOR_SPEED
#define SPEED_INCREMENT 5            // amount to increment speed when starting....

//#define STOP_DELAY         30    // milliseconds to delay between decrements in speed when stopping, typical setting
//#define STOP_DELAY_EVERY_N_L 50    // delay every Nth decrement when stopping, typical setting
//#define STOP_DELAY_EVERY_N_R 50    // delay every Nth decrement when stopping, typical setting

#define STOP_DELAY           0    // milliseconds to delay between decrements in speed when stopping, typical setting, heaven n earth
#define STOP_DELAY_EVERY_N_R 0    // delay every Nth decrement when stopping, typical setting going right (up), heaven n earth
#define STOP_DELAY_EVERY_N_L 0    // delay every Nth decrement when stopping, typical setting going left (down), heaven n earth

//#define REVERSE_DELAY  1000      // milleseconds to delay when reversing...
#define REVERSE_DELAY  500      // milleseconds to delay when reversing... new setting for heaven and earth

// limit switch related information...
#define LEFT_LIMIT_PIN 2        // input from left limit switch
#define RIGHT_LIMIT_PIN 3       // input from right limit switch

// #define LEFT_LIMIT_IS_CENTER true    // use the left limit as home rather then center, converts us back into a simple pingPong

//#define D_CENTER_PIN 4          // digital center pin
//#define A_CENTER_PIN A3         // analog center pin
//#define CENTER_IS_ANALOG true   // are we reading a digital or analog pin for center


//#define CENTER_LIMIT_PIN 4      // input from center limit switch
//#define CENTER_LIMIT_PIN A0      // input from center limit switch
#define PULLUP_ON 0             // reverse the logic for pullup pins
#define PULLUP_OFF 1            // reverse the logic for pullup pins



// pins to control the motor TB6612FNG (class HBridgeMotor)
#define MOTOR_PMW_PIN 5     // output speed/on-off must be a PMW pin
#define MOTOR_LEFT_PIN 6     // output to move left pin on h-bridge shield
#define MOTOR_RIGHT_PIN 7    // output to move right pin on h-bridge shield
#define MOTOR_STDBY_PIN 8   // 0 after we stop the motor, 1 when we are ready to start the motor...

// output pins to control the motor IBT_2  (class IBT2Motor)
#define MOTOR_IBT2_PMWR  5   // PMW pin from 0 - 1024 when > 0 moves montor to Right (left must be 0)
#define MOTOR_IBT2_PMWL  6   // PMW pin from 0 - 2014 when > 0 moves motor to Left (right must be 0)

#define MOTOR_CLASS "IBT2Motor"  // A string that is the class name for this break out board.
                            
                              

// communicatin pin from the dancer
#define DANCER_INPUT_PIN 9  // this goes high when the dancer is dancing

int num_loops = 1;          // used to limit prints 
boolean do_print = false;   // used to limit prints

String bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

///////////////////////////////////////////////////////////
//
// Class to manage limit switches
//
///////////////////////////////////////////////////////////
class Limits {
   
   public:
      void update( void );
      boolean isMaxLeft( void );
      boolean isMaxRight( void );
      boolean isCentered( void );
      int sumLimits( void ); 
      void print();
      // boolean center_passed;      // have we passed the center since the last reverse
      Limits(int l_pin, int r_pin) : 
        left_pin(l_pin), right_pin(r_pin)
        { update(); }  
 
   private:
      int left_pin;              // the pin number for the left pin
      int right_pin;             // the pin number for the right pin
      // int d_center_pin;          // digital center pin
      // int a_center_pin;          // analog center pin
      int last_left_read;        // the value the last time we peaked at the left pin
      int last_right_read;       // the value the last time we peaked at the right pin
      //int last_a_center_read;    // the value the last time we peaked at the analong center pin 
      //int last_d_center_read;    // the value the last time we peaked at the digital center pin 
      //int last_center_read;      // normalize the value for the analog and digital center pin here
      boolean left_limit_active;
      boolean right_limit_active;
      // boolean center_limit_active;
};

void Limits::update() {
  last_left_read = digitalRead(left_pin);
  last_right_read = digitalRead(right_pin);
  
  left_limit_active = false; 
  right_limit_active = false;
  if (last_left_read == PULLUP_ON) {left_limit_active = true; }
  if (last_right_read == PULLUP_ON) { right_limit_active = true;}
  
}

int  Limits::sumLimits() {
    return last_left_read + last_right_read;
}

boolean Limits::isMaxLeft() {
    return left_limit_active;
}

boolean Limits::isMaxRight() {
    return right_limit_active;
}

boolean Limits::isCentered() {
    if (left_limit_active || right_limit_active) {
      return true;
    } else {
      return false;
    }
}

void Limits::print() {
  Serial.print(F("Limits: atLeftMax="));
  Serial.print(bool_tostr(isMaxLeft()));
  Serial.print(F(" isCentered="));
  Serial.print(bool_tostr(isCentered()));
  Serial.print(F(", atRigtMax="));
  Serial.println(bool_tostr(isMaxRight()));
  Serial.print(F("        last_left_read="));
  Serial.print(last_left_read);
  Serial.print(F(", last_right_read="));
  Serial.println(last_right_read);
  Serial.print(F("        left_pin="));
  Serial.print(left_pin);
  Serial.print(F(", right_pin="));
  Serial.println(right_pin);
}
////////////////////// end Limits //////////////////////////

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
    virtual void print() =0;
    int speed = 0;
};

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
    void print();
    int speed = 0;
    
    HBridgeMotor(int m_speed_left,int  m_speed_right, int p_speed, int p_standby, int p_left, int p_right, Limits *limits ) :
      max_speed_left(m_speed_left), 
      max_speed_right(m_speed_right),
      pin_speed(p_speed),
      pin_left(p_left),
      pin_right(p_right),
      pin_standby(p_standby),
      _is_disabled(false),
      _limits(limits)
      { 
          pinMode(p_left, OUTPUT);
          digitalWrite(p_left, 0);
          pinMode(p_right, OUTPUT);
          digitalWrite(p_right, 0);
          pinMode(p_speed, OUTPUT);
          digitalWrite(p_speed, 0);
          pinMode(p_standby, OUTPUT);
          digitalWrite(p_standby, 0);
          disable(); go_left(); 
       }
    
  private:
    int pin_speed;
    int pin_left;
    int pin_right;
    int pin_standby;
    int max_speed;
    int max_speed_left;
    int max_speed_right;
    boolean _is_disabled = 0;
    int stop_delay_every_n = 50;
    Limits *_limits;

    int move_left = 0;
    int move_right = 0;
};

// bring the motor down in a controlled manor
void HBridgeMotor::stop() {
  
  boolean motor_was_running = false;

  Serial.println("HBridgeMotor::stop");
  Serial.print("Speed=");
  Serial.println(speed);
  
  for (int i=speed; i!=0; i--) {
    motor_was_running = true;
    analogWrite(pin_speed, i);
    if ((i % stop_delay_every_n) == 0) {
      Serial.println(i);
      delay(STOP_DELAY);
    }
    if (speed < MOTOR_START_SPEED) { break; }
  }
  // only do this final dealy when really stoppin the motor
  analogWrite(pin_speed, 0);
  if (motor_was_running) {
    delay(30);
  }
  speed = 0;
}

void HBridgeMotor::start() { 
    
    Serial.println("HBridgeMotor::start");
    Serial.print("Speed=");
    Serial.println(speed); 
  
    if (speed == 0) {
      digitalWrite(pin_left, move_left);
      digitalWrite(pin_right, move_right);
      digitalWrite(pin_speed, max_speed);
    }
    
    if (speed < MOTOR_START_SPEED) {
      speed = MOTOR_START_SPEED;
    }
}

// keep increasing the motor speed until max speed
void HBridgeMotor::run() {
  if ( ! is_disabled() ) {
    if (speed < max_speed) {
      speed += SPEED_INCREMENT;
      if (speed > max_speed) { speed = max_speed; } 
      analogWrite(pin_speed, speed);
      if ((speed % 1) == 0) {
        delay(50);
      }
    }
  }
}

// continues left if alrady going left, else stops and restart left
void HBridgeMotor::go_left() {
  int orig_speed = speed;
  if (current_direction() != 'l') {
    Serial.println("HBridgeMotor::go_left: reversing direction");
    stop();
    move_left = 1;
    move_right = 0;
    max_speed = max_speed_left;
    stop_delay_every_n = STOP_DELAY_EVERY_N_L;
    if (orig_speed != 0) {
      delay(REVERSE_DELAY);
      start();
    }
  }
}

// contines right if already going right, else stops and restarts rights
void HBridgeMotor::go_right() {
  int orig_speed = speed;
  if (current_direction() != 'r') {
    Serial.println("HBridgeMotor::go_right: reversing direction");
    stop();
    move_left = 0;
    move_right = 1;
    max_speed = max_speed_right;
    stop_delay_every_n = STOP_DELAY_EVERY_N_R;
    if (orig_speed != 0) {
      delay(REVERSE_DELAY);
      start();
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
  if (_is_disabled == true) {
    Serial.println("enabeling motor");
    digitalWrite(pin_standby, 1);
    start();
    _is_disabled = false;
  }
}

void HBridgeMotor::disable() {
  if (_is_disabled == false) {
    Serial.println("disabeling motor");
    stop();
    digitalWrite(pin_standby, 0);
    _is_disabled = true;
  }
}

boolean HBridgeMotor::is_disabled() {
  return _is_disabled;
}

boolean HBridgeMotor::is_stopped() {
  return (speed == 0);
}

void HBridgeMotor::print() {
  Serial.print(F("HBridgeMotor: is_disabled="));
  Serial.print(bool_tostr(is_disabled()));
  Serial.print(F(", is_stopped="));
  Serial.print(bool_tostr(is_stopped()));
  Serial.print(F(", direction="));
  Serial.print(current_direction());
  Serial.print(F(", max_speed="));
  Serial.print(max_speed);
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
    void print();
    int speed = 0;
    
    IBT2Motor(int m_speed_left, int m_speed_right,  int p_pmwr, int p_pmwl, Limits *limits ) :
      max_speed_left(m_speed_left),
      max_speed_right(m_speed_right),
      pin_pmwr(p_pmwr),
      pin_pmwl(p_pmwl),
      _is_disabled(false),
      _limits(limits)
      { 
          pinMode(pin_pmwr, OUTPUT);
          digitalWrite(pin_pmwr, 0);
          pinMode(pin_pmwl, OUTPUT);
          digitalWrite(pin_pmwl, 0);
          current_pmw_pin = pin_pmwl;
          alt_pmw_pin = pin_pmwr;
          max_speed = 0;
          disable(); go_left(); 
       }
    
  private:
    int pin_pmwr;
    int pin_pmwl;
    int current_pmw_pin;
    int alt_pmw_pin;
    int max_speed;
    int max_speed_left;
    int max_speed_right;
    int jump_start_speed;
    bool needs_jump_start=false;
    unsigned long jump_enabled_millis=0;      // set each time we go from disabled to enabled.
    unsigned long jump_millis_duration1=JUMP_START_MILLIS_DURATION1;   // run jump start speed for this many milliseconds at startup
    int jump_start_speed1=MOTOR_JUMP_START_SPEED1;                           // jumpstart speed 1
    unsigned long jump_millis_duration2=JUMP_START_MILLIS_DURATION2;   // run jump start speed if seconds is longer then jump_millis_duration1 and less then this value 
    int jump_start_speed2=MOTOR_JUMP_START_SPEED2;                           // jump start speed 2 (should be less then speed1)
    unsigned long jump_diff_millis=0;
    boolean _is_disabled = 0;
    int stop_delay_every_n = 10;
    unsigned long max_time_to_limit_switch = 1000L * MAX_SECONDS_TO_LIMIT_SWITCH;   // number of seconds to have passed before we reset motor
    unsigned long millis_motor_start=0;                         // millis when motor was last started
    unsigned long millis_since_start=0;                         // number of millis since motor was last started
    Limits *_limits;

    int move_left = 0;
    int move_right = 0;
};

// bring the motor down in a controlled manor
void IBT2Motor::stop() {
  
  boolean motor_was_running = false;

  Serial.println("IBT2Motor::stop");
  Serial.print("Speed=");
  Serial.println(speed);
  
  for (int i=speed; i!=0; i--) {
    motor_was_running = true;
    analogWrite(current_pmw_pin, i);
    digitalWrite(alt_pmw_pin, 0);
    if ((i % stop_delay_every_n) == 0) {
      Serial.println(i);
      delay(STOP_DELAY);
    }
    if (i < MOTOR_START_SPEED) { break; }
  }
  digitalWrite(current_pmw_pin, 0);
  digitalWrite(alt_pmw_pin, 0);
  // only do this final dealy when really stoppin the motor
  if (motor_was_running) {
    delay(100);
  }
  speed = 0;
}

void IBT2Motor::start() { 
    
    Serial.println("IBT2Motor::start");
    Serial.print("Speed=");
    Serial.println(speed); 
    
    if (speed < MOTOR_START_SPEED) {
      speed = MOTOR_START_SPEED;
    }
}

// keep increasing the motor speed until max speed
void IBT2Motor::run() {
  int run_max_speed=0;
  unsigned long current_millis=0;
  
  if ( ! is_disabled() ) {
    
    run_max_speed = max_speed; // default the current normal max speed

//    Serial.print(F("speed="));
//    Serial.print(speed);
//    Serial.print(F(", needs_jump_start="));
//    Serial.print(needs_jump_start);
    
    // check if we need to override max speed for a jump start 
    if (needs_jump_start == true) { 
      // this motor is just getting strted, list jump start it!
      if (speed <= MOTOR_START_SPEED ) { 
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
        run_max_speed = jump_start_speed1;
      } else if (jump_diff_millis < jump_millis_duration2) {
         run_max_speed = jump_start_speed2;
      } else {
        run_max_speed = max_speed;
      }
      
//      Serial.print(F(", jump_enabled_millis="));
//      Serial.print(jump_enabled_millis);
//      Serial.print(F(", jump_diff_millis="));
//      Serial.print(jump_diff_millis);
      
    }

    if (speed <= MOTOR_START_SPEED) {
      millis_motor_start=millis();
    }
    current_millis=millis();
    // guard against millis wrap around
    if (current_millis < millis_motor_start){
      millis_motor_start=current_millis;
    }
    millis_since_start = current_millis -  millis_motor_start; 
    if (millis_since_start > max_time_to_limit_switch) {
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
      speed += SPEED_INCREMENT;
      if (speed > run_max_speed) { speed = run_max_speed; } 
      digitalWrite(alt_pmw_pin, 0);
      analogWrite(current_pmw_pin, speed);
      if ((speed % 1) == 0) {
        delay(50);
      }
    } else if (speed > run_max_speed) {
      speed -= SPEED_INCREMENT;
      if (speed < run_max_speed) { speed = run_max_speed; } 
      digitalWrite(alt_pmw_pin, 0);
      analogWrite(current_pmw_pin, speed);
      if ((speed % 1) == 0) {
        delay(50);
      }
    }
  }
}

// continues left if alrady going left, else stops and restart left
void IBT2Motor::go_left() {
  int orig_speed = speed;
  if (current_direction() != 'l') {
    Serial.println("IBT2Motor::go_left: reversing direction");
    stop();
    move_left = 1;
    move_right = 0;
    current_pmw_pin = pin_pmwl;
    alt_pmw_pin = pin_pmwr;
    max_speed = max_speed_left;
    needs_jump_start=true;
    stop_delay_every_n = STOP_DELAY_EVERY_N_L;
    if (orig_speed != 0) {
      delay(REVERSE_DELAY);
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
    current_pmw_pin = pin_pmwr;
    alt_pmw_pin = pin_pmwl;
    max_speed = max_speed_right;
    needs_jump_start=true;
    stop_delay_every_n = STOP_DELAY_EVERY_N_R;
    if (orig_speed != 0) {
      delay(REVERSE_DELAY);
      start();
    }  
  }
}

// the current direction of the motor 
char IBT2Motor::current_direction() {
  if (move_left == 1) {
    return 'l';
  } else {
    return 'r';
  }
}

void IBT2Motor::enable() {
  if (_is_disabled == true) {
    Serial.println("enabeling motor");
    start();
    _is_disabled = false;
  }
}

void IBT2Motor::disable() {
  if (_is_disabled == false) {
    Serial.println("disabeling motor");
    stop();
    _is_disabled = true;
  }
}

boolean IBT2Motor::is_disabled() {
  return _is_disabled;
}

boolean IBT2Motor::is_stopped() {
  return (speed == 0);
}

void IBT2Motor::print() {
  Serial.print(F("IBT2Motor: is_disabled="));
  Serial.print(bool_tostr(is_disabled()));
  Serial.print(F(", is_stopped="));
  Serial.print(bool_tostr(is_stopped()));
  Serial.print(F(", direction="));
  Serial.print(current_direction());
  Serial.print(F(", max_speed="));
  Serial.print(max_speed);
  Serial.print(F(", needs_jumpstart="));
  Serial.print(needs_jump_start);
  Serial.print(F(", jump_diff_millis="));
  Serial.print(jump_diff_millis);
  Serial.print(F(", max_time_to_limit_switch="));
  Serial.print((unsigned long) max_time_to_limit_switch);
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
    Dancer( int p_dancer, GenericMotor *motor ) : pin_dancer(p_dancer), start_again(false), _motor(motor) {update(); stop_dancing();}
    void update();
    boolean i_am_dancing;         // the motor should be moving....
    boolean start_again;          // requeue to our first position
    void stop_dancing();
    void start_dancing();
    void dance();
    void print();

  private:
    int     pin_dancer;
    boolean remote_is_dancing;
    GenericMotor  *_motor;
};

void Dancer::update() {
  remote_is_dancing = (digitalRead(pin_dancer) == PULLUP_ON);
  
  if (remote_is_dancing) {
    start_again = false;    // always clear this when remote starts dancing
  }
  
  if (remote_is_dancing || start_again ) {
      i_am_dancing = true;
  } else {
    i_am_dancing = false;
  }
}

void Dancer::dance() {
  
  if (i_am_dancing && _motor->is_disabled()) {
    _motor->enable();
    _motor->start();
  }
  _motor->run();
}

void Dancer::start_dancing() {
  if (i_am_dancing == false) {
    Serial.println("starting to dance");
    // don't restart motor if already started...
    _motor->enable();
  }
  _motor->start();  // keep doing this, it will just do nothing when we reach max speed
}

void Dancer::stop_dancing() {
  if (_motor->is_disabled() == false) {
    Serial.println("Stopping dance");
    _motor->disable();
  }
}

void Dancer::print() {
  Serial.print(F("Dancer: i_am_dancing="));
  Serial.print(bool_tostr(i_am_dancing));
  Serial.print(F(", remote_is_dancing="));
  Serial.print(bool_tostr(remote_is_dancing));
  Serial.print(F(", start_again="));
  Serial.println(bool_tostr(start_again));
}

////////////////////// end Dancer //////////////////////////

Limits *current_limits;

GenericMotor * my_motor;

Dancer *my_dancer;

void setup() {

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  
  pinMode(LEFT_LIMIT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_LIMIT_PIN, INPUT_PULLUP);
  
  // pinMode(DANCER_INPUT_PIN, INPUT);
  pinMode(DANCER_INPUT_PIN, INPUT_PULLUP);
  
  current_limits = new Limits(LEFT_LIMIT_PIN, RIGHT_LIMIT_PIN);

  Serial.print("MOTOR_CLASS=");
  Serial.println(MOTOR_CLASS);
  if (MOTOR_CLASS == "HBridgeMotor") {
    my_motor = new HBridgeMotor(MOTOR_SPEED_LEFT, MOTOR_SPEED_RIGHT, MOTOR_PMW_PIN, MOTOR_STDBY_PIN, MOTOR_LEFT_PIN,  MOTOR_RIGHT_PIN, current_limits);
  } else if (MOTOR_CLASS == "IBT2Motor") {
    my_motor = new IBT2Motor(MOTOR_SPEED_LEFT, MOTOR_SPEED_RIGHT, MOTOR_IBT2_PMWR, MOTOR_IBT2_PMWL, current_limits);
  } else {
    Serial.println("ERROR: unknown motor class");
  }


  my_dancer = new Dancer(DANCER_INPUT_PIN, my_motor);  

}


// dump the status of the limit pins
void print_status() {
  
  if (! do_print) {
    return;
  }
  my_dancer->print();
  current_limits->print();
  my_motor->print();
  Serial.println(' ');
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

// watch for limits and request to enable/disable the motor
void loop() {
  
  boolean i_was_dancing = false;
  
  slow_down_prints();
  current_limits->update();
  my_dancer->update();
  print_status();

  // 0 switches actives = 3, 1 switches active = 2, 2 switches active 1, 3 switches active = 0 
  if (current_limits->sumLimits() < 1) {
    // something is very wrong stop the motor....
    if (do_print) {
      Serial.println("ERROR: 2 or more limts are active at the same time, motor is stopped...");
    }    
    my_dancer->stop_dancing();
    return;
  } 
  
  // if remote is stopped and we are not at start, go back to start
  if (my_dancer->i_am_dancing == false && current_limits->isCentered() != true) {
    if (do_print) {
      Serial.println("Not dancing, restarting dance...");
    }
    my_dancer->start_again = true;
    my_dancer->update();
    // if we have passed the center on this run, then home is behind us
  }

  // if remote is stopped and we are back at start position, stop
  if (my_dancer->start_again && current_limits->isCentered()) {
    if (do_print) { 
      Serial.println("restart complete, stopping...");
    }

    
    my_dancer->stop_dancing();
    my_dancer->start_again = false;
    my_dancer->update();
  }  
  
  if (my_dancer->i_am_dancing) {
    if (current_limits->isMaxLeft()) {
      my_motor->go_right();
    } else if (current_limits->isMaxRight()) {
      my_motor->go_left();
    }
    
    my_dancer->dance();
  } else {
    my_dancer->stop_dancing();
  }
  
}
