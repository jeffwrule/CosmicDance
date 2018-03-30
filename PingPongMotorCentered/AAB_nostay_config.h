#ifndef AAB_NOSTAY_CONFIG_H
#define AAB_NOSTAY_CONFIG_H
// pin layout in software for HBRIDGE
// This pin out will only allow for 1 motor (we used pmw for non-standrd use, but we only need one
#define M1_LEFT_LIMIT_PIN 2      // input from left limit switch, expects a NO (normally open) switch
#define M1_RIGHT_LIMIT_PIN 3     // input from right limit switch, expects a NO (normally open) switch
#define M1_CENTER_LIMIT_PIN A0   // for digital or analog devices
#define M1_ACS712_LIMIT_PIN A1   // using with ACS271

// Sparkfun TB6612FNG (class HBridgeMotor) Motor1
#define M1_HGRIDGE_PWM_PIN 5     // output speed/on-off must be a PWM pin
#define M1_HGRIDGE_RIGHT_PIN 6   // output to move right pin on h-bridge shield AIN1
#define M1_HGRIDGE_LEFT_PIN 7    // output to move left pin on h-bridge shield AIN2

// common pins for HBRIDGE and IBT2
#define DANCER_INPUT_PIN    9     // pin to read dancer input
#define HBRIDGE_MOTOR_STDBY 8  // enable the motor card HBRIDGE only use for both MOTOR1 and MOTOR2

#endif
