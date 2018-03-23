#include <Arduino.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/configs.h>

// include the limit switch and motor type for this build
#include </Users/jrule/Documents/Arduino/TimedMotor/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/PWMMotor.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/DanceMove.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/Dancer.h>

// control the print speed
PrintTimer *print_timer;

// how many dancers do we have?
#define NUM_DANCERS 3
Dancer* dancers[NUM_DANCERS];

//// motor 1 related instances and variables
GenericMotor *my_motor1;
Dancer *my_dancer1;
#define D1_NUM_DANCE_MOVES 4
DanceMove *d1_dance_moves[D1_NUM_DANCE_MOVES];

// motor 2 related instances and variables
GenericMotor *my_motor2;
Dancer *my_dancer2;
#define D2_NUM_DANCE_MOVES 3
DanceMove *d2_dance_moves[D2_NUM_DANCE_MOVES];

// motor 3 related instances and variables
GenericMotor *my_motor3;
Dancer *my_dancer3;
#define D3_NUM_DANCE_MOVES 3
DanceMove *d3_dance_moves[D3_NUM_DANCE_MOVES];

void setup() {
  int i=0;

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("\n------------ Start setup ------------"));
  Serial.println(F("PingPongMotorCentered.ino"));
  Serial.println(F("THUNDER Config"));

  print_timer = new PrintTimer(PRINT_AFTER_SECONDS);
  print_timer->init_values();
  print_timer->status();

/////////////////// CENTER ROLLERS ///////////////////////

  my_motor1 = new PWMMotor (
        "CENTER ROLLERS",       // const char *motor_name           // the name of this motor
        M1_PWM_PIN,             // unsigned short pwm_pin           // the pin to control this motor
        THUNDER_SPEED_START,     // unsigned int  motor_start_speed  // what speed to start the motor at
        20,                     // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
        30                      // increment_delay_millis           // how long to wait between speed changes
        );

  ////////////////////////////////////////////////////////
  /////// DANCE MOVES M1 /////////////////////////////////
  ////////////////////////////////////////////////////////

  i=0;
  d1_dance_moves[i++] = new DanceMove(
        "rollers1_jump_start",
        5,                    // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_START,                  // motor start speed      starting pwm speed
        THUNDER_SPEED_JUMPSTART,   // move_speed             pwm speed to run the motor at, 0-255
        1                   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        ); 

  d1_dance_moves[i++] = new DanceMove(
        "rollers1_start",
        0,                    // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_START,  // motor start speed      starting pwm speed
        THUNDER_SPEED_SLOW,   // move_speed             pwm speed to run the motor at, 0-255
        20                   // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        ); 
  
  d1_dance_moves[i++] = new DanceMove(
        "rollers2_fast",
        0,                    // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_START,   // motor start speed      starting pwm speed
        THUNDER_SPEED_FAST,   // move_speed             pwm speed to run the motor at, 0-255
        20                    // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        ); 
  
  d1_dance_moves[i++] = new DanceMove(
        "rollers3_slow",
        0,                      // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_START,    // motor start speed      starting pwm speed
        THUNDER_SPEED_SLOW,    // move_speed             pwm speed to run the motor at, 0-255
        60*60                  // move_duration_seconds  60 minutes, we just want to hit the limit to stop
        ); 

  if (i != D1_NUM_DANCE_MOVES) { Serial.println(F("ERRROR: D1_NUM_DANCE_MOVES != i++")); }

  // DANCER Thunder ROLLERS
  
  my_dancer1 = new Dancer(
    "THUNDER_ROLLERS",    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor1,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    d1_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D1_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0                           // p_delay_dance_seconds  one time delay at beginning of dance
    );

  ////////////////  BASE HAMMER M2 ///////////

  my_motor2 = new PWMMotor (
        "BASE HAMMER",          // const char *motor_name           // the name of this motor
        M2_PWM_PIN,             // unsigned short pwm_pin           // the pin to control this motor
        THUNDER_SPEED_SLOW,     // unsigned int  motor_start_speed  // what speed to start the motor at
        20,                     // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
        0                       // increment_delay_millis           // how long to wait between speed changes
        );

  ////////////////////////////////////////////////////////
  // DANCE MOVES M2
  ////////////////////////////////////////////////////////

  i=0;
  d2_dance_moves[i++] = new DanceMove(
        "BASS_hammer1",
        5,                   // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_MAX,    // motor start speed      starting pwm speed
        THUNDER_SPEED_MAX,    // move_speed             pwm speed to run the motor at, 0-255
        100,                  // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        M2_LIMIT_INTERRUPT_PIN,                    // which interrupt enabled pin is our rotation swith attached to
        2                     // number of rotations to execute before we stop
        ); 

  d2_dance_moves[i++] = new DanceMove(
        "BASS_hammer2",
        5,                    // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_MAX,   // motor start speed      starting pwm speed
        THUNDER_SPEED_MAX,   // move_speed             pwm speed to run the motor at, 0-255
        100,                  // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        M2_LIMIT_INTERRUPT_PIN,                    //  which interrupt enabled pin is our rotation swith attached to
        2                     // number of rotations to execute before we stop
        ); 
        
  d2_dance_moves[i++] = new DanceMove(
        "BASS_wiatforever",
        60*60,                // move_delay_seconds     seconds to delay before we start moving the motor
        0,                    // motor start speed      starting pwm speed
        0,                    // move_speed             pwm speed to run the motor at, 0-255
        0                     // move_duration_seconds  60 minutes, we just want to hit the limit to stop
        ); 

  if (i != D2_NUM_DANCE_MOVES) { Serial.println(F("ERRROR: D2_NUM_DANCE_MOVES != i++")); }

  my_dancer2 = new Dancer(
    "THUNDER_BASS",    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor2,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    d2_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D2_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0                          // p_delay_dance_seconds  one time delay at beginning of dance
    );


  ////////////////  TREBBIL HAMMER M3 ////////

  my_motor3 = new PWMMotor (
        "TREBLE HAMMER",        // const char *motor_name           // the name of this motor
        M3_PWM_PIN,             // unsigned short pwm_pin           // the pin to control this motor
        THUNDER_SPEED_SLOW,     // unsigned int  motor_start_speed  // what speed to start the motor at
        20,                     // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
        0                       // increment_delay_millis           // how long to wait between speed changes
        );

  ////////////////////////////////////////////////////////
  // DANCE MOVES M3
  ////////////////////////////////////////////////////////

  i=0;
  d3_dance_moves[i++] = new DanceMove(
        "TREBLE_hammer1",
        0,                   // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_MAX,    // motor start speed      starting pwm speed
        THUNDER_SPEED_MAX,    // move_speed             pwm speed to run the motor at, 0-255
        100,                  // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        M3_LIMIT_INTERRUPT_PIN,                    // which interrupt enabled pin is our rotation swith attached to
        2                     // number of rotations to execute before we stop
        ); 

  d3_dance_moves[i++] = new DanceMove(
        "TREBLE_hammer2",
        0,                    // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_SPEED_MAX,   // motor start speed      starting pwm speed
        THUNDER_SPEED_MAX,   // move_speed             pwm speed to run the motor at, 0-255
        100,                  // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        M3_LIMIT_INTERRUPT_PIN,                    //  which interrupt enabled pin is our rotation swith attached to
        2                     // number of rotations to execute before we stop
        ); 
        
  d3_dance_moves[i++] = new DanceMove(
        "TREBLE_wiatforever",
        60*60,                // move_delay_seconds     seconds to delay before we start moving the motor
        0,                    // motor start speed      starting pwm speed
        0,                    // move_speed             pwm speed to run the motor at, 0-255
        0                     // move_duration_seconds  60 minutes, we just want to hit the limit to stop
        ); 

  if (i != D3_NUM_DANCE_MOVES) { Serial.println(F("ERRROR: D3_NUM_DANCE_MOVES != i++")); }

  my_dancer3 = new Dancer(
    "THUNDER_TREBLE",    // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor3,                  // p_motor        pointer to our motor structure
    print_timer,                // print_timer    the current print timer and current_millis second tracker
    d3_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D3_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0                          // p_delay_dance_seconds  one time delay at beginning of dance
    );

  ////////////////   END HAMMERS ////////////

  i=0;
  dancers[i++] = my_dancer1;
  dancers[i++] = my_dancer2;
  dancers[i++] = my_dancer3;

  if (i != NUM_DANCERS) { Serial.println(F("ERRROR: NUM_DANCERS != i++")); }
    
  Serial.println(F("------------ End Setup ------------"));

}

// watch for limits and request to enable/disable the motor
void loop() {

  /////////////////////////////////////////
  // update things
  ////////////////////////////////////////
  print_timer->update();
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->update();
  }

  /////////////////////////////////////////
  // update things
  ////////////////////////////////////////  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->status();
  }
  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->dance();
    }
}

