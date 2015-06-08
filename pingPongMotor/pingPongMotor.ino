
using namespace std;

#define DEBUG = true
#define PRINT_EVER_NTH_ITTER 15000

#define MOTOR_SPEED 72        // values between 0 (off) and 255 (fully on)
#define MOTOR_START_SPEED 30  // low values don't produce movement must be lower then MOTOR_SPEED

#define REVERSE_DELAY  1000      // milleseconds to delay when reversing...

// limit switch related information...
#define LEFT_LIMIT_PIN 2        // input from left limit switch
#define RIGHT_LIMIT_PIN 3       // input from right limit switch
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
      void print();
      Limits(int l_pin, int r_pin) : 
        left_pin(l_pin), right_pin(r_pin), last_left_read(1), last_right_read(1)
        { update(); }  
 
   private:
      int left_pin;      // the pin number for the left pin
      int right_pin;     // the pin number for the right pin
      int last_left_read;  // the value the last time we peaked at the left pin
      int last_right_read;  // the value the last time we peaked at the right pin
      boolean left_limit_active;
      boolean right_limit_active;
};

void Limits::update() {
  last_left_read = digitalRead(left_pin);
  last_right_read = digitalRead(right_pin);
  left_limit_active = false; 
  right_limit_active = false;
  if (last_left_read == PULLUP_ON) {left_limit_active = true;}
  if (last_right_read == PULLUP_ON) { right_limit_active = true;}
}

boolean Limits::isMaxLeft() {
    return left_limit_active;
}

boolean Limits::isMaxRight() {
    return right_limit_active;
}

void Limits::print() {
  Serial.print(F("Limits: atLeftMax="));
  Serial.print(bool_tostr(isMaxLeft()));
  Serial.print(F(", atRigtMax="));
  Serial.print(bool_tostr(isMaxRight()));
  Serial.print(F(", last_left_read="));
  Serial.print(last_left_read);
  Serial.print(F(", last_right_read="));
  Serial.print(last_right_read);
  Serial.print(F(", left_pin="));
  Serial.print(right_pin);
  Serial.print(F(", right_pin="));
  Serial.println(left_pin);
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
  
  if (speed < max_speed) {
    
    Serial.println("HBridgeMotor::start");
    Serial.print("Speed=");
    Serial.println(speed); 
  
    if (speed == 0) {
      digitalWrite(pin_left, move_left);
      digitalWrite(pin_right, move_right);
      digitalWrite(pin_speed, max_speed);
    }
    
    int start_speed = speed;
    if (speed < MOTOR_START_SPEED) {
      speed = MOTOR_START_SPEED;
    }
    
    speed += 1;
    analogWrite(pin_speed, speed);
    if ((speed % 1) == 0) {
      delay(50);
    }
  } 
}

// contines left if alrady going left, else stops and restart left
void HBridgeMotor::go_left() {
  int orig_speed = speed;
  if (current_direction() != 'l') {
    stop();
    move_left = 1;
    move_right = 0;
    if (orig_speed > 0) {
      #ifdef DEBUG
        Serial.print("Delaying...");
        Serial.println(REVERSE_DELAY);
      #endif
      delay(REVERSE_DELAY);
//      start();   
    }
  }
}

// contines right if already going right, else stops and restarts rights
void HBridgeMotor::go_right() {
  int orig_speed = speed;
  if (current_direction() != 'r') {
    stop();
    move_left = 0;
    move_right = 1;
    if (orig_speed > 0) {
      #ifdef DEBUG
        Serial.print("Delaying...");
        Serial.println(REVERSE_DELAY);
      #endif
      delay(REVERSE_DELAY);      
//      start();   
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
    Dancer( int p_dancer, HBridgeMotor *motor ) : pin_dancer(p_dancer), i_am_dancing(true), _motor(motor) {update(); stop_dancing();}
    void update( void ) { remote_is_dancing = (digitalRead(pin_dancer) == 1); }
    boolean remote_is_dancing;
    boolean i_am_dancing;  // pretend we are dancing, so the init will definately shut the motor off
    void stop_dancing();
    void start_dancing();
    void print();

  private:
    int     pin_dancer;
    HBridgeMotor  *_motor;
};

void Dancer::start_dancing() {
  if (i_am_dancing == false) {
    Serial.println("starting to dance");
    _motor->enable();
    i_am_dancing = true;
  }
  _motor->start();  // keep doing this, it will just do nothing when we reach max speed
}

void Dancer::stop_dancing() {
  if (i_am_dancing == true) {
    Serial.println("Stopping dance");
    _motor->disable();
    i_am_dancing = false;
  }
}

void Dancer::print() {
  Serial.print(F("Dancer: remote_is_dancing="));
  Serial.print(bool_tostr(remote_is_dancing));
  Serial.print(F(", i_am_dancing="));
  Serial.println(bool_tostr(i_am_dancing));
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
  
  pinMode(MOTOR_LEFT_PIN, OUTPUT);
  digitalWrite(MOTOR_LEFT_PIN, 0);
  pinMode(MOTOR_RIGHT_PIN, OUTPUT);
  digitalWrite(MOTOR_RIGHT_PIN, 0);
  pinMode(MOTOR_PMW_PIN, OUTPUT);
  digitalWrite(MOTOR_PMW_PIN, 0);
  pinMode(MOTOR_STDBY_PIN, OUTPUT);
  digitalWrite(MOTOR_STDBY_PIN, 0);
  
  pinMode(DANCER_INPUT_PIN, INPUT);
  
  current_limits = new Limits(LEFT_LIMIT_PIN, RIGHT_LIMIT_PIN);
  
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
  
  slow_down_prints();
  current_limits->update();
  my_dancer->update();
//  my_dancer->remote_is_dancing = true; // always on if you uncomment this
  print_status();
  
  // if remote is stopped and we are not at the top, return to the top
  if (my_dancer->remote_is_dancing == false && current_limits->isMaxRight() != true) {
    if (do_print) {
      Serial.println("Not dancing, returning to top...");
    }
    my_dancer->remote_is_dancing = true;
    my_motor->go_right();  // return to top/right position
  }

  // stop dancing at the top and stay there, until remote is dancing again
  if (my_dancer->remote_is_dancing == false && current_limits->isMaxRight()) {
    if (do_print) { 
      Serial.println("Not dancing and at top, stop...");
    }
    my_dancer->stop_dancing();
    do_print=true;
  }  
  
  // react to status of limit switches, reverse motor if necessary
  if (current_limits->isMaxLeft() && current_limits->isMaxRight()) {
    // something is very wrong stop the motor....
    my_dancer->stop_dancing();
    if (do_print) {
      Serial.println("ERROR: both limts are active at the same time, motor is stopped...");
    }
    return;
  } else if (current_limits->isMaxLeft()) {
    my_motor->go_right();
  } else if (current_limits->isMaxRight()) {
    my_motor->go_left();
  }
  
  // react to input from Dancer and start and stop the motor
  if (my_dancer->remote_is_dancing) {
    my_dancer->start_dancing();
  } else {
    my_dancer->stop_dancing();
  }
}

