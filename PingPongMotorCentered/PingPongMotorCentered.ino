
using namespace std;

// PingPong montor that returns to a center switch when switched off

#define DEBUG = true
//#define PRINT_EVER_NTH_ITTER 15000
#define PRINT_EVER_NTH_ITTER 30000

//#define PRINT_EVER_NTH_ITTER 1

// because magnets are impprecise, add specific delays for different directions.
// final delays for cloud gate build
//#define CENTER_MOVING_LEFT_DELAY 9000
//#define CENTER_MOVING_RIGHT_DELAY 1

// expected delays for normal analog line (white switch) switch Sparkfun QRE1113 (Analog)
#define CENTER_MOVING_LEFT_DELAY 1
#define CENTER_MOVING_RIGHT_DELAY 1

#define CENTER_READ_CUTOFF 90    // know to work with LIFE piece.

// #define MOTOR_SPEED 72        // values between 0 (off) and 255 (fully on) (normal production speed)
#define MOTOR_SPEED 130        // values between 0 (off) and 255 (fully on) a little fast for LIFE
//#define MOTOR_SPEED 200        // values between 0 (off) and 255 (fully on)  (testing speed)
#define MOTOR_START_SPEED 30   // low values don't produce movement must be lower then MOTOR_SPEED
#define SPEED_INCREMENT 10     //

#define REVERSE_DELAY  1000      // milleseconds to delay when reversing...

// limit switch related information...
#define LEFT_LIMIT_PIN 2        // input from left limit switch
#define RIGHT_LIMIT_PIN 3       // input from right limit switch

#define D_CENTER_PIN 4          // digital center pine
#define A_CENTER_PIN A3         // analog center pin
#define CENTER_IS_ANALOG true   // are we reading a digital or analog pin for center

//#define CENTER_LIMIT_PIN 4      // input from center limit switch
//#define CENTER_LIMIT_PIN A0      // input from center limit switch
#define PULLUP_ON 0             // reverse the logic for pullup pins
#define PULLUP_OFF 1            // reverse the logic for pullup pins



// pins to control the motor
#define MOTOR_PMW_PIN 5     // output speed/on-off must be a PMW pin
#define MOTOR_LEFT_PIN 6     // output to move left pin on h-bridge shield
#define MOTOR_RIGHT_PIN 7    // output to move right pin on h-bridge shield
#define MOTOR_STDBY_PIN 8   // 0 after we stop the motor, 1 when we are ready to start the motor...

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
      boolean center_passed;      // have we passed the center since the last reverse
      Limits(int l_pin, int r_pin, int d_c_pin, int a_c_pin) : 
        left_pin(l_pin), right_pin(r_pin), d_center_pin(d_c_pin), a_center_pin(a_c_pin), center_passed(false)
        { update(); }  
 
   private:
      int left_pin;              // the pin number for the left pin
      int right_pin;             // the pin number for the right pin
      int d_center_pin;          // digital center pin
      int a_center_pin;          // analog center pin
      int last_left_read;        // the value the last time we peaked at the left pin
      int last_right_read;       // the value the last time we peaked at the right pin
      int last_a_center_read;    // the value the last time we peaked at the analong center pin 
      int last_d_center_read;    // the value the last time we peaked at the analong center pin 
      int last_center_read;      // normalize the value for the analog and digital center pin here
      boolean left_limit_active;
      boolean right_limit_active;
      boolean center_limit_active;
};

void Limits::update() {
  last_left_read = digitalRead(left_pin);
  last_right_read = digitalRead(right_pin);
  if (CENTER_IS_ANALOG) {
    last_a_center_read = analogRead(a_center_pin);
    last_d_center_read = PULLUP_OFF;
  } else {
    last_a_center_read = 2000;   // set this to a value out of range for normal analog devices 0-1023
    last_d_center_read = digitalRead(d_center_pin);    
  }
  last_center_read = PULLUP_OFF;  // this will get set later in the routine. Default to not set.
  
  left_limit_active = false; 
  right_limit_active = false;
  center_limit_active = false;
  if (last_left_read == PULLUP_ON) {left_limit_active = true; center_passed = false;}
  if (last_right_read == PULLUP_ON) { right_limit_active = true; center_passed = false;}
  if (CENTER_IS_ANALOG) {
    if (last_a_center_read < CENTER_READ_CUTOFF) { last_center_read = PULLUP_ON; center_limit_active = true; center_passed = true;} 
  } else {
    if (last_d_center_read == PULLUP_ON) { last_center_read = PULLUP_ON; center_limit_active = true; center_passed = true;}  
  }
}

int  Limits::sumLimits() {
    return last_left_read + last_right_read + last_center_read;
}

boolean Limits::isMaxLeft() {
    return left_limit_active;
}

boolean Limits::isMaxRight() {
    return right_limit_active;
}

boolean Limits::isCentered() {
    return center_limit_active;
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
  Serial.print(F(", last_a_center_read="));
  Serial.print(last_a_center_read);
  Serial.print(F(", last_d_center_read="));
  Serial.print(last_d_center_read);
  Serial.print(F(", last_center_read(psuedo)="));
  Serial.print(last_center_read);
  Serial.print(F(", last_right_read="));
  Serial.println(last_right_read);
  Serial.print(F("        left_pin="));
  Serial.print(left_pin);
  Serial.print(F(", d_center_pin="));
  Serial.print(d_center_pin);
  Serial.print(F(", a_center_pin="));
  Serial.print(a_center_pin);
  Serial.print(F(", right_pin="));
  Serial.println(right_pin);
  Serial.print(F("        center_passed="));
  Serial.println(bool_tostr(center_passed));
}
////////////////////// end Limits //////////////////////////

///////////////////////////////////////////////////////////
//
// Class to manage motor H-BRIDGE break out board 
// SparkFun Part: TB6612FNG (1A)
//
/////////////////////////////////////////////////////////// 
class HBridgeMotor {
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
    
    HBridgeMotor(int m_speed, int p_speed, int p_standby, int p_left, int p_right, Limits *limits ) :
      max_speed(m_speed), 
      pin_speed(p_speed),
      pin_left(p_left),
      pin_right(p_right),
      pin_standby(p_standby),
      _is_disabled(false),
      _limits(limits)
      { disable(); go_left(); }
    
  private:
    int pin_speed;
    int pin_left;
    int pin_right;
    int pin_standby;
    int max_speed;
    boolean _is_disabled;
    Limits *_limits;

    int move_left = 1;
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
    if ((i % 50) == 0) {
      Serial.println(i);
      delay(30);
    }
  }
  // only do this final dealy when really stoppin the motor
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
      analogWrite(pin_speed, speed);
      if ((speed % 1) == 0) {
        delay(50);
      }
    }
  }
}

// contines left if alrady going left, else stops and restart left
void HBridgeMotor::go_left() {
  int orig_speed = speed;
  if (current_direction() != 'l') {
    Serial.println("HBridgeMotor::go_left: reversing direction");
    stop();
    move_left = 1;
    move_right = 0;
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
  Serial.print(F(", speed="));
  Serial.println(speed);  
}

////////////////////// end HBridgeMotor //////////////////////////

///////////////////////////////////////////////////////////
//
// Class to manage remote dancer stack input 
//
/////////////////////////////////////////////////////////// 
class Dancer {

  public:
    Dancer( int p_dancer, HBridgeMotor *motor ) : pin_dancer(p_dancer), start_again(false), _motor(motor) {update(); stop_dancing();}
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
    HBridgeMotor  *_motor;
};

void Dancer::update() {
  remote_is_dancing = (digitalRead(pin_dancer) == 1);
  
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

HBridgeMotor *my_motor;

Dancer *my_dancer;

void setup() {

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  
  pinMode(LEFT_LIMIT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_LIMIT_PIN, INPUT_PULLUP);
  pinMode(D_CENTER_PIN, INPUT_PULLUP);
  
  pinMode(MOTOR_LEFT_PIN, OUTPUT);
  digitalWrite(MOTOR_LEFT_PIN, 0);
  pinMode(MOTOR_RIGHT_PIN, OUTPUT);
  digitalWrite(MOTOR_RIGHT_PIN, 0);
  pinMode(MOTOR_PMW_PIN, OUTPUT);
  digitalWrite(MOTOR_PMW_PIN, 0);
  pinMode(MOTOR_STDBY_PIN, OUTPUT);
  digitalWrite(MOTOR_STDBY_PIN, 0);
  
  pinMode(DANCER_INPUT_PIN, INPUT);
  
  current_limits = new Limits(LEFT_LIMIT_PIN, RIGHT_LIMIT_PIN, D_CENTER_PIN, A_CENTER_PIN);
  
  my_motor = new HBridgeMotor(MOTOR_SPEED, MOTOR_PMW_PIN, MOTOR_STDBY_PIN, MOTOR_LEFT_PIN,  MOTOR_RIGHT_PIN, current_limits);

  my_dancer = new Dancer(DANCER_INPUT_PIN, my_motor);  
  
  Serial.println("End setup");
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
  if (current_limits->sumLimits() < 2) {
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
    if (current_limits->center_passed) {
      if (my_motor->current_direction() == 'l') {
        my_motor->go_right();
      } else {
        my_motor->go_left();
      }
    }
  }

  // if remote is stopped and we are back at start position, stop
  if (my_dancer->start_again && current_limits->isCentered()) {
    if (do_print) { 
      Serial.println("restart complete, stopping...");
    }
    if (my_motor->current_direction() == 'l') {
      Serial.print("Moving Left delaying: ");
      Serial.println(CENTER_MOVING_LEFT_DELAY);
      delay(CENTER_MOVING_LEFT_DELAY);
    } else { 
      Serial.print("Moving Right delaying: ");
      Serial.println(CENTER_MOVING_RIGHT_DELAY);
      delay(CENTER_MOVING_RIGHT_DELAY);
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

