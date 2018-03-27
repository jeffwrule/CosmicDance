#ifndef BB_THUNDER_H
#define BB_THUNDER_H
// include the limit switch and motor type for this build
#include </Users/jrule/Documents/Arduino/TimedMotor/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/PWMMotor.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/DanceMove.h>
#include </Users/jrule/Documents/Arduino/TimedMotor/Dancer.h>

#include </Users/jrule/Documents/Arduino/TimedMotor/AAB_thunder_config.h>

// control the print speed
PrintTimer *print_timer;

// how many dancers do we have?
#define NUM_DANCERS 3
Dancer* dancers[NUM_DANCERS];

//// motor 1 related instances and variables
GenericMotor *my_motor1;
Dancer *my_dancer1;
#define D1_NUM_DANCE_MOVES 2
DanceMove *d1_dance_moves[D1_NUM_DANCE_MOVES];

// motor 2 related instances and variables
GenericMotor *my_motor2;
Dancer *my_dancer2;
#define D2_NUM_DANCE_MOVES 2
DanceMove *d2_dance_moves[D2_NUM_DANCE_MOVES];

// motor 3 related instances and variables
GenericMotor *my_motor3;
Dancer *my_dancer3;
#define D3_NUM_DANCE_MOVES 2
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
        "CENTER ROLLERS",                 // const char *motor_name           // the name of this motor
        M1_PWM_PIN,                       // unsigned short pwm_pin           // the pin to control this motor
        THUNDER_ROLLERS_START,            // unsigned int  motor_start_speed  // what speed to start the motor at
        THUNDER_ROLLERS_INCREMENT,        // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
        THUNDER_ROLLERS_INCREMENT_DELAY   // increment_delay_millis           // how long to wait between speed changes
        );

  ////////////////////////////////////////////////////////
  /////// DANCE MOVES M1 /////////////////////////////////
  ////////////////////////////////////////////////////////

  i=0;
  d1_dance_moves[i++] = new DanceMove(
        "rollers1",
        99,                     // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_ROLLERS_START,  // motor start speed      starting pwm speed
        THUNDER_ROLLERS_SLOW,   // move_speed             pwm speed to run the motor at, 0-255
        82,                     // move_duration_seconds 
        DELAY_TYPE_DANCE        // delay this move on the start of the move 'm'
        ); 
  
  d1_dance_moves[i++] = new DanceMove(
        "rollers2",       // move_name              name of this move for debug
        60*60,            // move_delay_seconds     seconds to delay before we start moving the motor
        0,                // motor start speed      starting pwm speed
        0,                // move_speed             pwm speed to run the motor at, 0-255
        0,                // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_MOVE   // delay this move on the start of the move 'm'
        ); 
  
  if (i != D1_NUM_DANCE_MOVES) { 
    Serial.print(F("ERRROR: D1_NUM_DANCE_MOVES != i++, D1_NUM_DANCE_MOVES=")); 
    Serial.print(D1_NUM_DANCE_MOVES);
    Serial.print(F(", i="));
    Serial.println(i);
  }

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
        THUNDER_HAMMER_START,     // unsigned int  motor_start_speed  // what speed to start the motor at
        THUNDER_HAMMER_INCREMENT,                     // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
        THUNDER_HAMMER_INCREMENT_DELAY                       // increment_delay_millis           // how long to wait between speed changes
        );

  ////////////////////////////////////////////////////////
  // DANCE MOVES M2
  ////////////////////////////////////////////////////////

  i=0;
  d2_dance_moves[i++] = new DanceMove(
        "bass1",
        223,                             // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_HAMMER_START,           // motor start speed      starting pwm speed
        THUNDER_HAMMER_SPEED_HAMMER,           // move_speed             pwm speed to run the motor at, 0-255
        THUNDER_HAMMER_DURATION_BASS,
        DELAY_TYPE_DANCE     // delay this move on the start of the move 'm'
        ); 

  d2_dance_moves[i++] = new DanceMove(
        "bass2",
        60*60,                // move_delay_seconds     seconds to delay before we start moving the motor
        0,                    // motor start speed      starting pwm speed
        0,                    // move_speed             pwm speed to run the motor at, 0-255
        0,                     // move_duration_seconds  60 minutes, we just want to hit the limit to stop
        DELAY_TYPE_MOVE     // delay this move on the start of the move 'm'
        ); 

  if (i != D2_NUM_DANCE_MOVES) { 
    Serial.print(F("ERRROR: D2_NUM_DANCE_MOVES != i++, D2_NUM_DANCE_MOVES=")); 
    Serial.print(D2_NUM_DANCE_MOVES);
    Serial.print(F(", i="));
    Serial.println(i);
  }

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
        THUNDER_HAMMER_START,     // unsigned int  motor_start_speed  // what speed to start the motor at
        THUNDER_HAMMER_INCREMENT,                     // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
        THUNDER_HAMMER_INCREMENT_DELAY                       // increment_delay_millis           // how long to wait between speed changes
        );

  ////////////////////////////////////////////////////////
  // DANCE MOVES M3
  ////////////////////////////////////////////////////////

  i=0;
  d3_dance_moves[i++] = new DanceMove(
        "treble1",
        189,                           // move_delay_seconds     seconds to delay before we start moving the motor
        THUNDER_HAMMER_START,   // motor start speed      starting pwm speed
        THUNDER_HAMMER_SPEED_HAMMER,         // move_speed             pwm speed to run the motor at, 0-255
        THUNDER_HAMMER_DURATION_TREBLE,
        DELAY_TYPE_DANCE     // delay this move on the start of the move 'm'
        ); 

  d3_dance_moves[i++] = new DanceMove(
        "treble2",
        60*60,                // move_delay_seconds     seconds to delay before we start moving the motor
        0,                    // motor start speed      starting pwm speed
        0,                    // move_speed             pwm speed to run the motor at, 0-255
        0,                     // move_duration_seconds  60 minutes, we just want to hit the limit to stop
        DELAY_TYPE_MOVE     // delay this move on the start of the move 'm'
        ); 

  if (i != D3_NUM_DANCE_MOVES) { 
    Serial.print(F("ERRROR: D3_NUM_DANCE_MOVES != i++, D3_NUM_DANCE_MOVES=")); 
    Serial.print(D3_NUM_DANCE_MOVES);
    Serial.print(F(", i="));
    Serial.println(i);
  }

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

  if (i != NUM_DANCERS) { 
    Serial.print(F("ERRROR: NUM_DANCERS != i++")); 
    Serial.print(NUM_DANCERS);
    Serial.print(F(", i="));
    Serial.println(i);
  }
    
  Serial.println(F("------------ End Setup ------------"));

}

#endif
