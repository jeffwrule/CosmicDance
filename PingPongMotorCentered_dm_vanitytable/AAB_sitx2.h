#ifndef AAB_SITX2_H
#define AAB_SITX2_H

#include "PrintTimer.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/Limits.h"
#include "DanceMove.h"
#include "Dancer.h"

#include "AAB_sitx2_config.h"

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
#define D1_NUM_DANCE_MOVES 9
DanceMove *d1_dance_moves[D1_NUM_DANCE_MOVES];

// motor 2 related instances and variables
GenericMotor *my_motor2;
Dancer *my_dancer2;
GenericLimitSwitch *m2_top_limit_switch;
GenericLimitSwitch *m2_bottom_limit_switch;
GenericLimitSwitch *m2_center_limit_switch;
#define D2_NUM_DANCE_MOVES 9
DanceMove *d2_dance_moves[D2_NUM_DANCE_MOVES];


void setup() {

  int i;

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("\n------------ Start setup ------------"));
  Serial.println(F("PingPongMotorCentered.ino"));
  Serial.println(F("SITX2 Config"));

  pinMode(HBRIDGE_MOTOR_STDBY, OUTPUT);
  digitalWrite(HBRIDGE_MOTOR_STDBY, LOW);

  print_timer = new PrintTimer(PRINT_AFTER_SECONDS);
  print_timer->init_values();
  print_timer->status();

  /////////////////////////////////////////////////////////////////
  // ------- LEFT CHAIR SITX2 motor/dancer 1------------
  /////////////////////////////////////////////////////////////////

  ////// LEFT CHIAR MOTOR
  my_motor1 = new HBridgeMotor(    
    "L_sitx2",    // p_motor_name               name of this motor (for status messages)
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

  ////// LEFT LIMIT SWITCHES SITX2 (tipping chairs)
  // L BOTTOM LIMIT SWITCH
  m1_bottom_limit_switch = new DigitalLimitSwitch(
    "LC_bottom_limit", // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',                  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NC"  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
  // R TOP LIMIT SWITCH
  m1_top_limit_switch = new DigitalLimitSwitch(
    "LC_top_limit", // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_RIGHT_LIMIT_PIN,       // p_limit_name             name for this limit switch
    'r',                      // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NC"              // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
  // L CENTER LIMIT SWITCH
  m1_center_limit_switch = m1_bottom_limit_switch; // the bottom limit is the center/home

  ////// LEFT CHIAR MOVEMENTS SITX2 (tipping chairs)
  i=0;
  d1_dance_moves[i++] =
    new DanceMove(
          "LCU1",
          9,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCD1",
          28,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_DOWN,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCU2",
          85,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCD2",
          112,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_DOWN,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCU3",
          154,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCD3",
          191,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_DOWN,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCU4",
          215,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LCD4",
          258,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_DOWN,            // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "LC_final",
          60*60,                        // move_delay_seconds     seconds to delay before we start moving the motor
          SITX2_MOVE_DOWN,              // move_direction         motor direction l,r (l=down, r=up typically)
          SITX2_SPEED_UP,               // move_speed             pwm speed to run the motor at, 0-255
          SITX2_LONG_MOVE_DURATION,     // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE              // delay start DANCE 'd' or MOVE 'm'
          );

  if (i != D1_NUM_DANCE_MOVES) {
    Serial.println(F("ERROR: NUM_D1_DANCE_MOVES != num of initialized steps, NUM_D1_DANCE_MOVES="));
  }

  ///// LEFT CHAIR DANCER SITX2 (tipping chairs)
  my_dancer1 = new Dancer(
    "LC_sitx2",    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor1,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    "LC_limit_switch",          // p_limits_name  The name of the Limits objects that will be created using the LimitSwitch objects
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

  /////////////////////////////////////////////////////////////////
  //  // ------- RIGHT CHAIR SITX2 motor/dancer 2------------
  /////////////////////////////////////////////////////////////////
  
  ////// RIGHT CHIAR MOTOR   
  my_motor2 = new HBridgeMotor(    
    "R_sitx2",    // p_motor_name               name of this motor (for status messages)
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

  ////// RIGHT CHAIR LIMIT SWITCHES 
  // R BOTTOM LIMIT switch
  m2_bottom_limit_switch = new DigitalLimitSwitch(
    "RC_bottom_limit", // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M2_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',                  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NC"  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
  // R TOP LIMIT switch
  m2_top_limit_switch = new DigitalLimitSwitch(
    "RC_top_limit", // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M2_RIGHT_LIMIT_PIN,       // p_limit_name             name for this limit switch
    'r',                      // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NC"              // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
  // R CENTER LIMIT switch
  m2_center_limit_switch = m2_bottom_limit_switch; // the bottom limit is the center/home

  ////// RIGHT CHAIR MOVES SITX2 (tipping chairs), down first
  i=0;
  d2_dance_moves[i++] = new DanceMove(
        "RCU1",
        55,                       // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCD1",
        73,                       // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_DOWN,          // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,         // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCU2",
        127,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCD2",
        139,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_DOWN,          // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,         // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCU3",
        154,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCD3",
        191,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_DOWN,          // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,         // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCU4",
        239,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RCD4",
        268,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_DOWN,          // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,         // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm' 
        ); 
  d2_dance_moves[i++] = new DanceMove(
        "RC_final",
        60*60,                      // move_delay_seconds     seconds to delay before we start moving the motor
        SITX2_MOVE_DOWN,            // move_direction         motor direction l,r (l=down, r=up typically)
        SITX2_SPEED_DOWN,           // move_speed             pwm speed to run the motor at, 0-255
        SITX2_LONG_MOVE_DURATION,   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_DANCE            // delay start DANCE 'd' or MOVE 'm' 
        ); 
  if (i != D2_NUM_DANCE_MOVES) {
    Serial.print(F("ERROR: D2_NUM_DANCE_MOVES != num of initialized steps, D2_NUM_DANCE_MOVES="));
    Serial.print(D2_NUM_DANCE_MOVES);
    Serial.print(F(", i="));
    Serial.println(i);
  }


  ////// RIGHT CHAIR DANCER SITX2 (tipping chairs)
  my_dancer2 = new Dancer(
    "RC_sitx2",    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor2,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    "RC_limit_switch",          // p_limits_name  The name of the Limits objects that will be created using the LimitSwitch objects
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
  i=0;
  dancers[i++] = my_dancer1;
  dancers[i++] = my_dancer2;

  if (i != NUM_DANCERS) {
    Serial.print(F("ERROR: NUM_DANCERS != i NUM_DANCERS="));
    Serial.print(NUM_DANCERS);
    Serial.print(F(", i="));
    Serial.println(i);
  }

  print_timer->print_now();  // make sure this call prints a status
  for (int i=0; i<NUM_DANCERS; i++) {
    dancers[i]->update();
    dancers[i]->status();
  }
    
    Serial.println(F("------------ End Setup ------------"));

}

#endif
