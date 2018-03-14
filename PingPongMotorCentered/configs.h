#ifndef CONFIGS_H
#define CONFIGS_H

// forward declerations for helpers
String bool_tostr(bool input_bool);
unsigned short digitalReads(unsigned int pin, unsigned int reads);

#define SERIAL_SPEED 250000
#define PRINT_AFTER_SECONDS 3    // print every n seconds 

#define IS_BRIEF True
// #define IS_CHATTY     // define this for extra debug information 

#define PIN_READ_DELAY_MS  400

// debug tool, do not leave set for normal user
//#define ALWAYS_BE_DANCING     // uncomment for debug, normally commented out, set remote_is_dancing_ 1 (helps with floating line)
//#define NEVER_BE_DANCING        // uncomment for debug, normally commented out, sets remote_is_dancing_ 0 (helps with floating line)

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////
  
  // Suggested pin layout, but some older scupltures will have different layouts, check the older code for the exact layout in github
  #define M1_LEFT_LIMIT_PIN 7      // input from left limit switch, expects a NO (normally open) switch
  #define M1_RIGHT_LIMIT_PIN 8     // input from right limit switch, expects a NO (normally open) switch
  
  
  // Sparkfun TB6612FNG (class HBridgeMotor) Motor1
  #define M1_HGRIDGE_LEFT_PIN 9    // output to move left pin on h-bridge shield
  #define M1_HGRIDGE_RIGHT_PIN 10   // output to move right pin on h-bridge shield
  #define M1_HGRIDGE_PWM_PIN 11     // output speed/on-off must be a PWM pin
 
////////////////////////////////////////
/// MOTOR 2 PINS
////////////////////////////////////////

  // fob switches 
  #define M2_LEFT_LIMIT_PIN 2      // input from left limit switch, expects a NO (normally open) switch
  #define M2_RIGHT_LIMIT_PIN 3     // input from right limit switch, expects a NO (normally open) switch

  // Sparkfun TB6612FNG (class HBridgeMotor) Motor1
  #define M2_HGRIDGE_LEFT_PIN 4    // output to move left pin on h-bridge shield
  #define M2_HGRIDGE_RIGHT_PIN 5   // output to move right pin on h-bridge shield
  #define M2_HGRIDGE_PWM_PIN 6     // output speed/on-off must be a PWM pin
  
////////////////////////////////////////
/// TB6612FNG common pin
////////////////////////////////////////

  #define HBRIDGE_MOTOR_STDBY 12  // enable the motor card HBRIDGE only use for both MOTOR1 and MOTOR2

// remote dancer pine 
#define DANCER_INPUT_PIN    A4     // remote dancer input pin

#endif
