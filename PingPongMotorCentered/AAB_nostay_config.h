#ifndef AAB_NOSTAY_CONFIG_H
#define AAB_NOSTAY_CONFIG_H

#define SITX2_SPEED_DOWN 255
#define SITX2_SPEED_UP 255

#define SITX2_LONG_MOVE_DURATION 3600    // seconds

#define SITX2_MOVE_UP 'r'
#define SITX2_MOVE_DOWN 'l'

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
