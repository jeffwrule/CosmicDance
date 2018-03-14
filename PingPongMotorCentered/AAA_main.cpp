#include <Arduino.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/configs.h>

#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/GenericLimitSwitch.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/GenericMotor.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/Limits.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/DanceMove.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/Dancer.h>

//  COPY CONFIGS AFTER THIS LINE

// include the limit switch and motor type for this build
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/HBridgeMotor.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/DigitalLimitSwitch.h>

// control the print speed
PrintTimer *print_timer;

// how many dancers do we have?
#define NUM_DANCERS 2
Dancer* dancers[NUM_DANCERS];

//// motor 1 related instances and variables
GenericMotor *my_motor1;
Dancer *my_dancer1;
GenericLimitSwitch *m1_top_limit_switch;
GenericLimitSwitch *m1_bottom_limit_switch;
GenericLimitSwitch *m1_center_limit_switch;
#define D1_NUM_DANCE_MOVES 4
DanceMove *d1_dance_moves[D1_NUM_DANCE_MOVES];

// motor 2 related instances and variables
GenericMotor *my_motor2;
Dancer *my_dancer2;
GenericLimitSwitch *m2_top_limit_switch;
GenericLimitSwitch *m2_bottom_limit_switch;
GenericLimitSwitch *m2_center_limit_switch;
#define D2_NUM_DANCE_MOVES 2
DanceMove *d2_dance_moves[D2_NUM_DANCE_MOVES];

// down = r, up=l
#define SITX2_SPEED_DOWN 255    
#define SITX2_SPEED_UP 255

void setup() {

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("\n------------ Start setup ------------"));
  Serial.println(F("PingPongMotorCentered.ino"));
  Serial.println(F("SITX2 Config"));

  pinMode(HBRIDGE_MOTOR_STDBY, OUTPUT);
  digitalWrite(HBRIDGE_MOTOR_STDBY, LOW);

  print_timer = new PrintTimer(PRINT_AFTER_SECONDS);
  print_timer->init_values();
  print_timer->status();


  // ------- LEFT CHAIR SITX2 motor/dancer 1------------

  my_motor1 = new HBridgeMotor(    
    String("L_sitx2"),    // p_motor_name               name of this motor (for status messages)
    255,                  // m_speed_left               max speed when going left
    255,                  // m_speed_right              max_speed when going right
    M1_HGRIDGE_PWM_PIN,   // p_speed                    pwm speed pin
    HBRIDGE_MOTOR_STDBY,  // p_standby                  stand by pin (high=enabled, low=disabled)
    M1_HGRIDGE_LEFT_PIN,  // p_left                     pin for left direction 
    M1_HGRIDGE_RIGHT_PIN, // p_right                    pin for right direction
    25,                   // p_motor_start_speed        start() uses this as the first pmw speed when starting, must be *LESS* then m_speed_left, m_speed_right
    25,                   // p_speed_increment          increment the pmw by this ammount when starting the motor
    50,                   // p_speed_up_increment_delay   delay to add between increments to make speedup smoother
    50,                   // p_slow_down_increment_delay  delay to add between increments to make slowdown smoother
    200                   // p_reverse_delay_millis     wait this many milliseconds when reversing direction 
    );

  // SITX2 (tipping chairs)
  m1_bottom_limit_switch = new DigitalLimitSwitch(
    String("LC_bottom_limit"), // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',                  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    String("NC")  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
  m1_top_limit_switch = new DigitalLimitSwitch(
    String("LC_top_limit"), // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_RIGHT_LIMIT_PIN,       // p_limit_name             name for this limit switch
    'r',                      // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    String("NC")              // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
  m1_center_limit_switch = m1_bottom_limit_switch; // the bottom limit is the center/home

  // SITX2 (tipping chairs), down first
  d1_dance_moves[0] = new DanceMove(
        String("LC_move_down_1"),
        0,                  // move_delay_seconds     seconds to delay before we start moving the motor
        'r',                // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,  // move_speed             pwm speed to run the motor at, 0-255
        5*60                   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        ); 

  // SITX2 (tipping chairs), up second
  d1_dance_moves[1] = new DanceMove(
      String("LC_move_up_2"),
      10,                // move_delay_seconds     seconds to delay before we start moving the motor
      'l',              // move_direction         motor direction l,r (l=down, r=up typically)
      SITX2_SPEED_UP,  // move_speed             pwm speed to run the motor at, 0-255
      5*60              // move_duration_seconds  5 minutes, we just want to hit the limit switch
      ); 

//  // SITX2 (tipping chairs), down first
//  d1_dance_moves[2] = new DanceMove(
//        String("LC_move_down_3"),
//        4,                  // move_delay_seconds     seconds to delay before we start moving the motor
//        'r',                // move_direction         motor direction l,r (l=down, r=up typically)
//        SITX2_SPEED_DOWN,  // move_speed             pwm speed to run the motor at, 0-255
//        10                   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
//        ); 
//
//  // SITX2 (tipping chairs), up second
//  d1_dance_moves[3] = new DanceMove(
//      String("LC_move_up_4"),
//      4,                // move_delay_seconds     seconds to delay before we start moving the motor
//      'l',              // move_direction         motor direction l,r (l=down, r=up typically)
//      SITX2_SPEED_UP,  // move_speed             pwm speed to run the motor at, 0-255
//      10              // move_duration_seconds  5 minutes, we just want to hit the limit switch
//      ); 

  // SITX2 (tipping chairs)
  my_dancer1 = new Dancer(
    String("LC_sitx2"),    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor1,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    m1_bottom_limit_switch,     // p_left_limit   left limit switch
    m1_top_limit_switch,        // p_right_limit  right limit switch
    m1_center_limit_switch,     // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    d1_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D1_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0,                          // p_delay_dance_seconds  one time delay at beginning of dance
    0,                          // p_extend_dance_seconds number of seconds to extend the dance when done
    SITX2_SPEED_DOWN,            // p_extend_speed speed to run the PMW at when extending
    SITX2_SPEED_DOWN             // p_center_speed   speed to run motor at when centering
    );

  // ------- RIGHT CHAIR SITX2 motor/dancer 2------------

  my_motor2 = new HBridgeMotor(    
    String("R_sitx2"),    // p_motor_name               name of this motor (for status messages)
    255,                  // m_speed_left               max speed when going left
    255,                  // m_speed_right              max_speed when going right
    M2_HGRIDGE_PWM_PIN,   // p_speed                    pwm speed pin
    HBRIDGE_MOTOR_STDBY,  // p_standby                  stand by pin (high=enabled, low=disabled)
    M2_HGRIDGE_LEFT_PIN,  // p_left                     pin for left direction 
    M2_HGRIDGE_RIGHT_PIN, // p_right                    pin for right direction
    25,                   // p_motor_start_speed        start() uses this as the first pmw speed when starting, must be *LESS* then m_speed_left, m_speed_right
    25,                   // p_speed_increment          increment the pmw by this ammount when starting the motor
    50,                   // p_speed_up_increment_delay   delay to add between increments to make speedup smoother
    50,                   // p_slow_down_increment_delay  delay to add between increments to make slowdown smoother
    200                   // p_reverse_delay_millis     wait this many milliseconds when reversing direction 
    );

  // SITX2 (tipping chairs)
  m2_bottom_limit_switch = new DigitalLimitSwitch(
    String("RC_bottom_limit"), // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M2_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',                  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    String("NC")  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
  m2_top_limit_switch = new DigitalLimitSwitch(
    String("RC_top_limit"), // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M2_RIGHT_LIMIT_PIN,       // p_limit_name             name for this limit switch
    'r',                      // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    String("NC")              // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
  m2_center_limit_switch = m2_bottom_limit_switch; // the bottom limit is the center/home

  // SITX2 (tipping chairs), down first
  d2_dance_moves[0] = new DanceMove(
        String("RC_move_down_1"),
        0,                  // move_delay_seconds     seconds to delay before we start moving the motor
        'r',                // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,  // move_speed             pwm speed to run the motor at, 0-255
        5*60                   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        ); 

  // SITX2 (tipping chairs), up second
  d2_dance_moves[1] = new DanceMove(
      String("RC_move_up_2"),
      10,                // move_delay_seconds     seconds to delay before we start moving the motor
      'l',              // move_direction         motor direction l,r (l=down, r=up typically)
      SITX2_SPEED_UP,  // move_speed             pwm speed to run the motor at, 0-255
      5*60              // move_duration_seconds  5 minutes, we just want to hit the limit switch
      ); 

//  // SITX2 (tipping chairs), down first
//  d2_dance_moves[2] = new DanceMove(
//        String("RC_move_down_3"),
//        4,                  // move_delay_seconds     seconds to delay before we start moving the motor
//        'r',                // move_direction         motor direction l,r (l=down, r=up typically)
//        SITX2_SPEED_DOWN,  // move_speed             pwm speed to run the motor at, 0-255
//        10                   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
//        ); 
//
//  // SITX2 (tipping chairs), up second
//  d2_dance_moves[3] = new DanceMove(
//      String("RC_move_up_4"),
//      4,                // move_delay_seconds     seconds to delay before we start moving the motor
//      'l',              // move_direction         motor direction l,r (l=down, r=up typically)
//      SITX2_SPEED_UP,  // move_speed             pwm speed to run the motor at, 0-255
//      10              // move_duration_seconds  5 minutes, we just want to hit the limit switch
//      ); 

  // SITX2 (tipping chairs)
  my_dancer2 = new Dancer(
    String("RC_sitx2"),    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor2,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    m2_bottom_limit_switch,     // p_left_limit   left limit switch
    m2_top_limit_switch,        // p_right_limit  right limit switch
    m2_center_limit_switch,     // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    d2_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D2_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0,                          // p_delay_dance_seconds  one time delay at beginning of dance
    0,                          // p_extend_dance_seconds number of seconds to extend the dance when done
    SITX2_SPEED_DOWN,            // p_extend_speed speed to run the PMW at when extending
    SITX2_SPEED_DOWN             // p_center_speed   speed to run motor at when centering
    );

  // ---------- END CHAIR CONFIG ------------
  dancers[0] = my_dancer1;
  dancers[1] = my_dancer2;

  print_timer->print_now();  // make sure this call prints a status
  for (int i=0; i<NUM_DANCERS; i++) {
    dancers[i]->update();
    dancers[i]->status();
  }
    
    Serial.println(F("------------ End Setup ------------"));

}

// watch for limits and request to enable/disable the motor
void loop() {
  
  print_timer->update();
  print_timer->status();
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->update();
      dancers[i]->status();
  }
  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->dance();
    }
}

