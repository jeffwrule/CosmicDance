#ifndef AAD_WATER_H
#define AAD_WATER_H

#include "PrintTimer.h"
#include "Limits.h"
#include "DanceMove.h"
#include "Dancer.h"

#include "IBT2Motor.h"
#include "DigitalLimitSwitch.h"

#include "AAD_water_config.h"

//  COPY CONFIGS AFTER THIS LINE

// control the print speed
PrintTimer *print_timer;

// how many dancers do we have?
#define NUM_DANCERS 1
Dancer* dancers[NUM_DANCERS];

//// motor 1 related instances and variables
IBT2Motor *my_motor1;
Dancer *my_dancer1;
GenericLimitSwitch *m1_top_limit_switch;
GenericLimitSwitch *m1_bottom_limit_switch;
GenericLimitSwitch *m1_center_limit_switch;
#define D1_NUM_DANCE_MOVES 2
DanceMove *d1_dance_moves[D1_NUM_DANCE_MOVES];


void setup() {

  int i;

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("\n------------ Start setup ------------"));
  Serial.println(F("PingPongMotorCentered.ino"));
  Serial.println(F("WATER Config"));


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
    
  my_motor1 = new IBT2Motor(
        "motor1", 
        M1_MAX_SPEED_LEFT, M1_MAX_SPEED_RIGHT,  
        M1_LEFT_PMW, M1_RIGHT_PMW, 
        M1_START_PWM, M1_INCREMENT,
        M1_DELAY_MS, M1_DELAY_MS, M1_DELAY_MS, 
        M1_DELAY_MS,
        M1_NEEDS_JUMP_LEFT, M1_NEEDS_JUMP_RIGHT,
        M1_JUMP1_MILLS, M1_JUMP_DURATION,
        M1_JUMP2_MILLS, M1_JUMP_DURATION,
        50 );
        
        
  print_timer = new PrintTimer(PRINT_AFTER_SECONDS);
  print_timer->init_values();
  print_timer->status();

  /////////////////////////////////////////////////////////////////
  // ------- LEFT CHAIR SITX2 motor/dancer 1------------
  /////////////////////////////////////////////////////////////////

  // L BOTTOM LIMIT SWITCH
  m1_bottom_limit_switch = new DigitalLimitSwitch(
    "m1_bottom_limit",    // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',                  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NO"                  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
    
  // R TOP LIMIT SWITCH
  m1_top_limit_switch = new DigitalLimitSwitch(
    "m1_top_limit",           // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_RIGHT_LIMIT_PIN,       // p_limit_name             name for this limit switch
    'r',                      // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NO"                      // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
    
  // L CENTER LIMIT SWITCH
  m1_center_limit_switch = m1_top_limit_switch; // the bottom limit is the center/home

  ////// center up/down motor movement
  i=0;
  d1_dance_moves[i++] =
    new DanceMove(
          "m_up",
          0,                        // move_delay_seconds     seconds to delay before we start moving the motor
          WATER_MOVE_UP,            // move_direction         motor direction l,r (l=down, r=up typically)
          WATER_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          WATER_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
  d1_dance_moves[i++] =
    new DanceMove(
          "m_down",
          0,                        // move_delay_seconds     seconds to delay before we start moving the motor
          WATER_MOVE_DOWN,            // move_direction         motor direction l,r (l=down, r=up typically)
          WATER_SPEED_UP,           // move_speed             pwm speed to run the motor at, 0-255
          WATER_LONG_MOVE_DURATION, // move_duration_seconds  5 minutes, we just want to hit the limit to stop
          DELAY_TYPE_DANCE          // delay start DANCE 'd' or MOVE 'm'
          );
 

  if (i != D1_NUM_DANCE_MOVES) {
    Serial.println(F("ERROR: NUM_D1_DANCE_MOVES != num of initialized steps, NUM_D1_DANCE_MOVES="));
  }
  
  my_dancer1 = new Dancer(
    "dancer1",                  // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor1,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    "m1_limits",                // p_limits_name  The name of the Limits objects that will be created using the LimitSwitch objects
    m1_bottom_limit_switch,     // p_left_limit   left limit switch
    m1_top_limit_switch,        // p_right_limit  right limit switch
    m1_center_limit_switch,     // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    d1_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D1_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0,                          // p_delay_dance_seconds  one time delay at beginning of dance
    0,                          // p_extend_dance_seconds number of seconds to extend the dance when done
    WATER_SPEED_DOWN,            // p_extend_speed speed to run the PMW at when extending
    WATER_SPEED_DOWN             // p_center_speed   speed to run motor at when centering
    );

  i=0;
  dancers[i++] = my_dancer1;

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
