
using namespace std;

// PingPong montor that returns to a center switch when switched off

#define DEBUG = true
//#define PRINT_EVER_NTH_ITTER 50
//#define PRINT_EVER_NTH_ITTER 30000

#define PRINT_EVER_NTH_ITTER 1

//#define IS_CHATTY     // define this for extra debug information 


// because magnets are impprecise, add specific delays for different directions.
// final delays for cloud gate build
//#define CENTER_MOVING_LEFT_DELAY 9000
//#define CENTER_MOVING_RIGHT_DELAY 1

// expected delays for normal analog line (white switch) switch Sparkfun QRE1113 (Analog)
#define CENTER_MOVING_LEFT_DELAY 1
#define CENTER_MOVING_RIGHT_DELAY 1

#define CENTER_READ_CUTOFF 250    // known to work with LIFE piece. (magnetic sensor?)

#define MONTOR_START_DELAY 30     // seconds to delay after start dancing is set but before we actually start the motor
boolean start_delay_complete=false;
unsigned long start_delay_begin;
unsigned long start_delay_current;


//// IBT_2 only setting this block

// Heaven and Earth
#define NEEDS_JUMP_START_RIGHT true         // jumpstart when going right
#define NEEDS_JUMP_START_LEFT  false         // jumpstart when going left
#define MOTOR_JUMP_START_SPEED1 255       // IBT_2 needs more power to get going (up) then the normal run speed (heaven and earth)
#define JUMP_START_MILLIS_DURATION1 2000  // IBT_2 motor will run left (up) speed for this many seconds when starting left was 7000
#define MOTOR_JUMP_START_SPEED2 170       // IBT_2 needs more power to get going (up) then the normal run speed (heaven and earth)
#define JUMP_START_MILLIS_DURATION2 4000  // IBT_2 motor will run left (up) speed for this many seconds when starting left was 1300
#define MOTOR_SPEED_LEFT  120        // GO DOWN: values between 0 (off) and 255 (fully on) This is for heaven and earth
#define MOTOR_SPEED_RIGHT 130
// GO UP: values between 0 (off) and 255 (fully on) This is for heaven and earth
#define MAX_SECONDS_TO_LIMIT_SWITCH 150 // max time we should ever expect to reach either limit switch

#define MOTOR_START_SPEED  100       // low values don't produce movement must be lower then MOTOR_SPEED_(LEFT_RIGHT),, REALLY!! at least 1 < MOTOR_SPEED|MOTOR_SPEED_LEFT|MOTOR_SPEED_RIGHT
#define SPEED_INCREMENT 50          // amount to increment speed when starting....

//
//// IBT_2 DRIPDRIP
//#define MOTOR_JUMP_START_SPEED1 80       // IBT_2 needs more power to get going (up) then the normal run speed (drip drop)
//#define JUMP_START_MILLIS_DURATION1 1  // IBT_2 motor will run left (up) speed for this many seconds when starting left
//#define MOTOR_JUMP_START_SPEED2 80       // IBT_2 needs more power to get going (up) then the normal run speed (drip drip)
//#define JUMP_START_MILLIS_DURATION2 2  // IBT_2 motor will run left (up) speed for this many seconds when starting left

// IBT_2 DRIPDRIP
//#define MOTOR_JUMP_START_SPEED1 255       // IBT_2 needs more power to get going (up) then the normal run speed (drip drop)
//#define JUMP_START_MILLIS_DURATION1 1  // IBT_2 motor will run left (up) speed for this many seconds when starting left
//#define MOTOR_JUMP_START_SPEED2 255       // IBT_2 needs more power to get going (up) then the normal run speed (drip drip)
//#define JUMP_START_MILLIS_DURATION2 2  // IBT_2 motor will run left (up) speed for this many seconds when starting left

// IBT_2 longing cloud
//#define NEEDS_JUMP_START_RIGHT false         // jumpstart when going right
//#define NEEDS_JUMP_START_LEFT  false         // jumpstart when going left
//#define MOTOR_JUMP_START_SPEED1 255       // IBT_2 needs more power to get going (up) then the normal run speed (drip drop)
//#define JUMP_START_MILLIS_DURATION1 1  // IBT_2 motor will run left (up) speed for this many seconds when starting left
//#define MOTOR_JUMP_START_SPEED2 255       // IBT_2 needs more power to get going (up) then the normal run speed (drip drip)
//#define JUMP_START_MILLIS_DURATION2 2  // IBT_2 motor will run left (up) speed for this many seconds when starting left

// #define MOTOR_SPEED 72        // values between 0 (off) and 255 (fully on) (normal production speed)
//#define MOTOR_SPEED 130        // values between 0 (off) and 255 (fully on) a little fast for LIFE
//#define MOTOR_SPEED 200        // values between 0 (off) and 255 (fully on)  (testing speed)



//#define MOTOR_SPEED_LEFT  90        // GO DOWN: values between 0 (off) and 255 (fully on) This is for gravity circle up
//#define MOTOR_SPEED_RIGHT 70        // GO UP: values between 0 (off) and 255 (fully on) This is for heaven and earth
//#define MOTOR_SPEED_LEFT  80        // GO UP: values between 0 (off) and 255 (fully on) This is for DRIPDRIP
//#define MOTOR_SPEED_RIGHT 80        // GO DOWN: values between 0 (off) and 255 (fully on) This is for DRIPDRIP
//#define MAX_SECONDS_TO_LIMIT_SWITCH 150 // max time we should ever expect to reach either limit switch
//#define MOTOR_SPEED_LEFT  198        // GO UP: values between 0 (off) and 255 (fully on) This is for Longing Cloud
//#define MOTOR_SPEED_RIGHT 198        // GO DOWN: values between 0 (off) and 255 (fully on) This is for Longing Cloud
//#define MAX_SECONDS_TO_LIMIT_SWITCH 350 // max time we should ever expect to reach either limit switch

// make sure max speed (is always larger then this value)

//
//// used with dripdrip
//#define STOP_DELAY         30    // milliseconds to delay between decrements in speed when stopping, typical setting
//#define STOP_DELAY_EVERY_N_L 50    // delay every Nth decrement when stopping, typical setting
//#define STOP_DELAY_EVERY_N_R 50    // delay every Nth decrement when stopping, typical setting

// used with longing cloud
//#define STOP_DELAY         0    // milliseconds to delay between decrements in speed when stopping, typical setting
//#define STOP_DELAY_EVERY_N_L 1000    // basically no delay, it stops automatically: delay every Nth decrement when stopping, typical setting
//#define STOP_DELAY_EVERY_N_R 1000    // basically no delay, it stops automatically: delay every Nth decrement when stopping, typical setting


#define STOP_DELAY         100    // milliseconds to delay between decrements in speed when stopping, typical setting, heaven n earty
#define STOP_DELAY_EVERY_N_R 50    // delay every Nth decrement when stopping, typical setting going right (up), heaven n earth
#define STOP_DELAY_EVERY_N_L 25    // delay every Nth decrement when stopping, typical setting going left (down), heaven n earth

#define REVERSE_DELAY  1000      // milleseconds to delay when reversing... reduced from 1000 for longing cloud

//ACS712 AMPS
#define ACS712_30A_FACTOR 0.066  // 30 AMP sensing board (least sensative) use with 30AMP version of board
#define ACS712_20A_FACTOR 0.100  // 20 AMP sensing board (more sensative) use with 20AMP version of board
#define ACS712_5A_FACTOR  0.185  // 5 AMP sensing board (most sensative) use with 5AMP version board
#define ACS712_FACTOR ACS712_5A_FACTOR

#define ACS_OFF 0.10              // when the motor is off the board returns values between ACS_OFF*-1 and ACS_OFF

// limit switch related information...
#define LEFT_LIMIT_PIN 2        // input from left limit switch
#define RIGHT_LIMIT_PIN 3       // input from right limit switch
//#define ANALOG_RIGHT_LIMIT_PIN A1 // using with ACS271    // most pieces
#define ANALOG_RIGHT_LIMIT_PIN A0 // using with ACS271

// what kind of limit switches are we using, physical switches or analog censors
#define RIGHT_LIMIT_TYPE 'd'   // d for digital switch 'a' for analog ACS712 module, b (both) single sensor for both right and left 
                               // b longing cloud  ACS712 checks power being cut both directions
                               // d heaven and earth  (digital pins both directions)
#define CENTER_IS 'l'    // use one of the following values right='r', left='l', center='c'
                         // original gravity 'l' 
                         // round_gravity is 'r'
                         // others are 'c'
                         // dripdrop is 'l'
                         // longing cloud is 'r'
                         // heaven and earth l
                         

#define D_CENTER_PIN 4          // digital center pin
#define A_CENTER_PIN A3         // analog center pin
// IMPORTANT: must set this correctly for any scuplpture the has a center other then LEFT or RIGHT.
#define CENTER_IS_ANALOG false  // are we reading a digital or analog pin for center, digital works best when center same left or right
                                 
#define PULLUP_ON 0             // reverse the logic for pullup pins
#define PULLUP_OFF 1            // reverse the logic for pullup pins


// pins to control the motor TB6612FNG (class HBridgeMotor)
#define MOTOR_PMW_PIN 5     // output speed/on-off must be a PMW pin
#define MOTOR_LEFT_PIN 6     // output to move left pin on h-bridge shield
#define MOTOR_RIGHT_PIN 7    // output to move right pin on h-bridge shield
#define MOTOR_STDBY_PIN 8   // 0 after we stop the motor, 1 when we are ready to start the motor...

// pins to control the motor IBT_2  (class IBT2Motor)
#define MOTOR_IBT2_PMWR  5   // PMW pin from 0 - 1024 when > 0 moves montor to Right (left must be 0)
#define MOTOR_IBT2_PMWL  6   // PMW pin from 0 - 2014 when > 0 moves motor to Left (right must be 0)

// Set this depending on what kind of ping/pong motor controler you have hooked up.
// IBT2Motor http://www.hessmer.org/blog/2013/12/28/ibt-2-h-bridge-with-arduino/  (for larger amp motors)
// HBridgeMotor: https://www.sparkfun.com/products/9457  (for smaller amp motors)
#define MOTOR_CLASS "IBT2Motor"  // A string that is the class name for this break out board. IBT2Motor or HBridgeMotor
                            
// communication pin from the dancer
#define DANCER_INPUT_PIN 9  // this goes high when the dancer is dancing

// extend the center/home, this allows the dancer to keep going for a few more seconds, normally 0 for most piecies
// we added this to strech out the fabric on drip drp.
//#define DANCER_EXTEND_SECONDS 6    
// longing cloud     
//#define DANCER_EXTEND_SECONDS 3  
//Heaven and Earth
#define DANCER_EXTEND_SECONDS 0    

// debug tool, do not leave set for normal user
//#define ALWAYS_BE_DANCING     // uncomment for debu, normally commented out

long num_loops = 1;          // used to limit prints 
boolean do_print = false;   // used to limit prints

String bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

// read the ACS712 HAL censor to see if we have power 
// map the result into PULLUP_OFF the motor is running, PULLUP_ON the motor is off
float lastAcsValueF=0.0;
int readACS712() {
  // put your main code here, to run repeatedly:

  unsigned int x=0;

  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  // sample this value a bunch of times to get an average
  for (int x=0; x < 150; x++) {
    AcsValue = analogRead(ANALOG_RIGHT_LIMIT_PIN);
    // Serial.print("individual read=");
    // Serial.println(AcsValue);
    Samples = Samples + AcsValue;
    delay(1);
  }
  AvgAcs=Samples/150.0;
  //  Serial.print("Samples=");
  //  Serial.println(Samples);
  //  Serial.print("AvgAcs=");
  //  Serial.println(AvgAcs);
  //  Serial.print("Scale up=");
  //  Serial.println(AvgAcs * (5.0/1024.0));
  // this is for a 30 AMP sensor
  AcsValueF=((AvgAcs * (5.0 / 1024.0)) - 2.5)/ACS712_FACTOR;
  lastAcsValueF = AcsValueF;
  //  Serial.println(AcsValueF);
  //  delay(10);
  // if AcsValueF is near 0 then the power is off/limit switch was triggered
  // also we use PULL_ON logic, switch active=0 switch off=1
  if ( AcsValueF >= (ACS_OFF * -1.0) and AcsValueF <= ACS_OFF ) {
    return PULLUP_ON;   // we have reached our limit
  } else {
    return PULLUP_OFF;  // we are not at limit/end
  }
 
}


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
};


class GenericDancer {

  public:
    virtual void update() =0;
    virtual void stop_dancing() =0;
    virtual void extend_dance() =0;
    virtual void dance() =0;
    virtual void print() =0;
    virtual boolean remote_is_dancing() =0;
    boolean dance_extended = false;
};


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
      boolean dance_extended;     // was the dance extended....
      Limits(int l_pin, int r_pin, int d_c_pin, int a_c_pin, GenericMotor* my_motor, GenericDancer* my_dancer) : 
        left_pin(l_pin), right_pin(r_pin), d_center_pin(d_c_pin), a_center_pin(a_c_pin), center_passed(false), _my_motor(my_motor), _my_dancer(my_dancer)
        { update(); }  
 
   private:
      int left_pin;              // the pin number for the left pin
      int right_pin;             // the pin number for the right pin
      int d_center_pin;          // digital center pin
      int a_center_pin;          // analog center pin
      int last_left_read;        // the value the last time we peaked at the left pin
      int last_right_read;       // the value the last time we peaked at the right pin
      int last_a_center_read;    // the value the last time we peaked at the analong center pin 
      int last_d_center_read;    // the value the last time we peaked at the digital center pin 
      int last_center_read;      // normalize the value for the analog and digital center pin here
      int last_acs_read;         // read from the acs sensor
      boolean left_limit_active;
      boolean right_limit_active;
      boolean center_limit_active;
      GenericMotor* _my_motor;
      GenericDancer* _my_dancer;
};

void Limits::update() {

  last_left_read = digitalRead(left_pin);   // set the left pin
  
  // set the right pin
  if (RIGHT_LIMIT_TYPE == 'a') {      // acs712 device is active, but right side only
    // when the left limit is on we turn off power, so this could create a false read
    // just say it is not set when we have the left pin on.
    if (last_left_read == PULLUP_ON) { // if left limit switch active, then believe it
      last_right_read = PULLUP_OFF;
    } else {
      last_right_read = readACS712();
    }
  } else if (RIGHT_LIMIT_TYPE == 'b') {  // no limit switch either side, use current direction and ACS712
    last_acs_read = readACS712();
    #ifdef IS_CHATTY
      Serial.println("Limits before we set 'b' type...");
      _my_dancer->print();
      print();
      _my_motor->print();
    #endif
 _my_dancer->print();
Serial.print(F("dance_extended: "));
Serial.println(bool_tostr(_my_dancer->dance_extended));
    if (_my_dancer->dance_extended == true ) { // we are neither left or right, just near left or right 
      #ifdef IS_CHATTY
        Serial.println("using 'b', dance_extended == true section...");
      #endif
      last_right_read = PULLUP_OFF;   // right is set by ACS read
      last_left_read = PULLUP_OFF;       // we are not left            
    } else {
      // dance is not extended...
      #ifdef IS_CHATTY
        Serial.println("using 'b', dance_extended == false section...");
      #endif        
      if (_my_motor->current_direction() == 'l') {
        last_left_read = last_acs_read;   // left is set by ACS read
        last_right_read = PULLUP_OFF;     // we are not right
      } else {
        last_right_read = last_acs_read;   // right is set by ACS read
        last_left_read = PULLUP_OFF;       // we are not left      
      }
    }
    #ifdef IS_CHATTY
      Serial.println("Limits after right after we set 'b' type...");
      _my_dancer->print();
      print();
      _my_motor->print();
    #endif 
  } else { // not analog, just read the switch
    last_right_read = digitalRead(right_pin);
  }

  // set the center pin read
  if (CENTER_IS == 'r' || CENTER_IS == 'l') {
      // center is set using right/left pin setting setting from above
      last_a_center_read = 2000;  // we use digital pin read for l/r communication
      last_d_center_read = PULLUP_OFF;
      if (RIGHT_LIMIT_TYPE == 'b') {  
        // hard to detect if we are centered, just know we made it left or right and then extended, we are centered...
        last_center_read = PULLUP_OFF;
        if (_my_dancer->dance_extended == true ) { last_center_read = PULLUP_ON; }
      } else { // center just follows the digital reads for left and right
        if (CENTER_IS == 'r' && _my_motor->current_direction() == 'r') { last_center_read = last_right_read; }
        if (CENTER_IS == 'l' && _my_motor->current_direction() == 'l') { last_center_read = last_left_read; }
      }
  } else  if (CENTER_IS_ANALOG) {
    // center is computed off analog pin
    last_d_center_read = PULLUP_OFF;
    last_a_center_read = analogRead(a_center_pin);
    last_center_read = PULLUP_OFF;
    if (last_a_center_read < CENTER_READ_CUTOFF) { last_center_read = PULLUP_ON; center_passed = true; }
  } else {
    // center is computed off digital pin
    last_a_center_read = 2000;   // set this to a value out of range for normal analog devices 0-1023
    last_d_center_read = digitalRead(d_center_pin); 
    if (last_d_center_read == PULLUP_ON) { center_passed = true;}
  }
  
  last_center_read = PULLUP_OFF;  // this will get set later in the routine. Default to not set.
  
  left_limit_active = false; 
  right_limit_active = false;
  center_limit_active = false;
  if (last_left_read == PULLUP_ON) {left_limit_active = true; center_passed = false;}
  if (last_right_read == PULLUP_ON) { right_limit_active = true; center_passed = false;}
  if (last_center_read == PULLUP_ON && _my_dancer->dance_extended == true) { center_limit_active = true; }
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
    if (CENTER_IS == 'l') {
      return left_limit_active;
    } else if (CENTER_IS == 'r') {
      return right_limit_active;
    } else {
      return center_limit_active;
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
  Serial.print(right_pin);
  Serial.print(F(", lastAcsValueF="));
  Serial.println(lastAcsValueF);
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
    
    HBridgeMotor(int m_speed_left,int  m_speed_right, int p_speed, int p_standby, int p_left, int p_right) :
      max_speed_left(m_speed_left), 
      max_speed_right(m_speed_right),
      pin_speed(p_speed),
      pin_left(p_left),
      pin_right(p_right),
      pin_standby(p_standby),
      _is_disabled(false)
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

// continues left if already going left, else stops and restart left
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
    void stop();                // stop the motor by bring pmw down to 0
    void start();               // if current speed is less then start speed, make it start speed
    void run();                 // increment motor speed to max speed in SPEED_INCREMENT jumps over multiple calls, does nothing if motor is disabled
    void go_left();
    void go_right();
    boolean is_stopped();
    boolean is_disabled();
    char current_direction();
    void enable();
    void disable();
    void print();
    int speed = 0;
    
    IBT2Motor(int m_speed_left, int m_speed_right,  int p_pmwr, int p_pmwl) :
      max_speed_left(m_speed_left),
      max_speed_right(m_speed_right),
      pin_pmwr(p_pmwr),
      pin_pmwl(p_pmwl),
      _is_disabled(false)
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
    int pin_pmwr;                         // right pin
    int pin_pmwl;                         // left pin
    int current_pmw_pin;                  // current_pin switches between the two above
    int alt_pmw_pin;                      // the other pin, this pin will be set to 0 while we use the current to drive the motor
    int max_speed;                        // max speed for the current direction, set in go_left and go_right from max_speed_left/right
    int max_speed_left;                   // max speed to go when going right, set with MOTOR_SPEED_RIGHT
    int max_speed_right;                  // max speed to go when going left, set with MOTOR_SPEED_LEFT
    bool needs_jump_start;                // mapped from NEEDS_JUMP_START_LEFT and NEEDS_JUMP_START_RIGHT in go_left and go_right
    unsigned long jump_enabled_millis=0;  // set in run() to the current milliseconds value millis()
    unsigned long jump_diff_millis=0;     // how long has the current jump window been active.
    unsigned long jump_millis_duration1=JUMP_START_MILLIS_DURATION1;          // number of milliseconds to run jump window 1
    int jump_start_speed1=MOTOR_JUMP_START_SPEED1;                            // jump window 1 max speed, mapped into max_speed during this window
    unsigned long jump_millis_duration2=JUMP_START_MILLIS_DURATION2;          // number of milliseconds to run jump window 1, must be large then window 1 to run. 
    int jump_start_speed2=MOTOR_JUMP_START_SPEED2;                            // jump window 1 max speed, mapped into max_speed during this window, 
                                                                              //     jump start speed 2 (should be less then or equal to speed1)
    boolean _is_disabled = 0;             // has disable() been called, cleared by enable()
    int stop_delay_every_n = 10;          // when we have decrement the speed this many points, delay for STOP_DELAY milliseconds
    unsigned long max_time_to_limit_switch = 1000L * MAX_SECONDS_TO_LIMIT_SWITCH;   // number of seconds to have passed before we reset motor
                                                                                    // add b/c the time piece would get stuck sometimes, this was just
                                                                                    // a fix re-execute the jump start values.
    unsigned long millis_motor_start=0;   // millis when motor was last started
    unsigned long millis_since_start=0;   // number of millis since motor was last started

    int move_left = 0;                    // we are moving left,  move_right should be 0, set in go_left, cleared in go_right, returned by current_direction
    int move_right = 0;                   // we are moving right, move_left should be 0,  set in go_right, cleared go_left, returned by current_direction
};

// bring the motor down in a controlled manor
void IBT2Motor::stop() {
  
  boolean motor_was_running = false;

  Serial.println("IBT2Motor::stop");
  #ifdef IS_CHATTY
    Serial.print("debug before stop: current_pmw_pin=");
    Serial.print(current_pmw_pin);
    Serial.print(", alt_pmw_pin=");
    Serial.print(alt_pmw_pin);
    Serial.print(", Speed=");
    Serial.println(speed);
  #endif

  // trim down the motor speed; if speed > 0
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

  #ifdef IS_CHATTY
    Serial.print("debug after stop: current_pmw_pin=");
    Serial.print(current_pmw_pin);
    Serial.print(", alt_pmw_pin=");
    Serial.print(alt_pmw_pin);
    Serial.print(", Speed=");
    Serial.println(speed);
  #endif
  
}

void IBT2Motor::start() { 
    
    Serial.println("IBT2Motor::start");
    Serial.print("before start Speed=");
    Serial.println(speed); 
    
    if (speed < MOTOR_START_SPEED) {
      speed = MOTOR_START_SPEED;
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
    if (needs_jump_start == true) { 
      // this motor is just getting started, let's jump start it!
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
        #ifdef IS_CHATTY
          Serial.print(F("In jump start1"));
        #endif 
      } else if (jump_diff_millis < jump_millis_duration2) {
         run_max_speed = jump_start_speed2;
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
  #ifdef IS_CHATTH
    Serial.println(F("IBT2Motor::run() end"));
  #endif
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
    needs_jump_start=NEEDS_JUMP_START_LEFT;
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
    needs_jump_start=NEEDS_JUMP_START_RIGHT;
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
  } else if (move_right == 1) {
    return 'r';
  } else {
    return '?';
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
  Serial.print(F(", move_left="));
  Serial.print(move_left);
  Serial.print(F(", move_right="));
  Serial.print(move_right);
  Serial.print(F(", direction="));
  Serial.print(current_direction());
  Serial.print(F(", max_speed="));
  Serial.print(max_speed);
  Serial.print(F(", needs_jumpstart="));
  Serial.print(bool_tostr(needs_jump_start));
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
class Dancer: public GenericDancer {

  public:
    Dancer( int p_dancer, GenericMotor *motor) : pin_dancer(p_dancer), _motor(motor) {update(); stop_dancing();}
    void update();
    boolean i_am_dancing;         // the motor should be moving....
    void stop_dancing();
    void extend_dance();
    void dance();
    void print();
    boolean remote_is_dancing();

  private:
    int     pin_dancer;
    boolean remote_is_dancing_;
    GenericMotor  *_motor;
    int     extend_seconds = DANCER_EXTEND_SECONDS;
};

boolean Dancer::remote_is_dancing() {
  return remote_is_dancing_;
}

void Dancer::update() {
  remote_is_dancing_ = (digitalRead(pin_dancer) == 1);
  #ifdef ALWAYS_BE_DANCING
    remote_is_dancing_ = true;
  #endif
  
  if (remote_is_dancing_ || dance_extended == false) {
    dance_extended = false;
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

void Dancer::stop_dancing() {
  if (_motor->is_disabled() == false) {
    Serial.println("Stopping dance");
    _motor->disable();
  }
}

void Dancer::extend_dance() {
  int i = extend_seconds;
  
  Serial.println(F("Extending dance, if necessary..."));

  while ( i > 0 ) {
    Serial.print(F("Extended dance seconds remaining: "));
    Serial.println(i);
    do_print = true;
    print_status();
    dance();
    delay(1000);
    i = i - 1;
  }
  
  dance_extended = true;
  print_status();
  
  Serial.println(F("Extending dance, complete..."));

}

void Dancer::print() {
  Serial.print(F("Dancer: i_am_dancing="));
  Serial.print(bool_tostr(i_am_dancing));
  Serial.print(F(", remote_is_dancing="));
  Serial.print(bool_tostr(remote_is_dancing_));
  Serial.print(F(", extend_seconds="));
  Serial.print(extend_seconds);
  Serial.print(F(", dance_extended="));
  Serial.println(bool_tostr(dance_extended));
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
  pinMode(D_CENTER_PIN, INPUT_PULLUP);
  
  pinMode(DANCER_INPUT_PIN, INPUT);
  
  Serial.print("MOTOR_CLASS=");
  Serial.println(MOTOR_CLASS);
  if (MOTOR_CLASS == "HBridgeMotor") {
    my_motor = new HBridgeMotor(MOTOR_SPEED_LEFT, MOTOR_SPEED_RIGHT, MOTOR_PMW_PIN, MOTOR_STDBY_PIN, MOTOR_LEFT_PIN,  MOTOR_RIGHT_PIN);
  } else if (MOTOR_CLASS == "IBT2Motor") {
    my_motor = new IBT2Motor(MOTOR_SPEED_LEFT, MOTOR_SPEED_RIGHT, MOTOR_IBT2_PMWR, MOTOR_IBT2_PMWL);
  } else {
    Serial.println("ERROR: unknown motor class");
  }

  my_dancer = new Dancer(DANCER_INPUT_PIN, my_motor);  

  current_limits = new Limits(LEFT_LIMIT_PIN, RIGHT_LIMIT_PIN, D_CENTER_PIN, A_CENTER_PIN, my_motor, my_dancer);

  Serial.print("Center is=");
  Serial.print(CENTER_IS);
  Serial.print(" Center_IS_ANALOG=");
  Serial.println(CENTER_IS_ANALOG);
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

// reset the arduino
//void(* resetFunc) (void) = 0;

// watch for limits and request to enable/disable the motor
void loop() {
    
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
  if (my_dancer->remote_is_dancing() == false && my_dancer->dance_extended == false) {
    if (do_print) {
      Serial.println("Not dancing, going home...");
    }
    // if we have passed the center on this run, then home is behind us
    if (CENTER_IS == 'l') {
      if (my_motor->current_direction() == 'r') {
        my_motor->go_left();
      }
    } else if (CENTER_IS == 'r') {
      if (my_motor->current_direction() == 'l') {
        my_motor->go_right();
      }
    } else {
      if (current_limits->center_passed) {
        if (my_motor->current_direction() == 'l') {
          my_motor->go_right();
        } else {
          my_motor->go_left();
        }
      }
    }
  }

  // if remote is stopped and we are back at start position and not extended... extend!
  if (my_dancer->remote_is_dancing() == false && current_limits->isCentered() && my_dancer->dance_extended == false) {
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
    if (current_limits->isMaxLeft()) {
      my_motor->go_right();
    } else if (current_limits->isMaxRight()) {
      my_motor->go_left();
    }
    my_dancer->extend_dance();  // will only move if DANCER_EXTEND_SECONDS > 0
    my_dancer->stop_dancing();
    my_dancer->update();
    current_limits->update();
    print_status();
    Serial.println("Dancer is centered!");
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

