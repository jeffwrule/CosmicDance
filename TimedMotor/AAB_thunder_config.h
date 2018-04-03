#ifndef BBB_THUNDER_CONFIG_H
#define BBB_THUNDER_CONFIG_H

////////////////////////////////////////
// MOTOR 1 pin(s) PWMMotor
///////////////////////////////////////
#define M1_PWM_PIN  9                              // pwm speed pin

////////////////////////////////////////
// MOTOR2 pin(s) PWMMotorMonitored (bass)
///////////////////////////////////////
#define M2_PWM_PIN  10                              // pwm speed pin
#define M2_LIMIT_INTERRUPT_PIN 2                    // interrupt attached pin for this motor

////////////////////////////////////////
// MOTOR3 pin(s) PWMMotorMonitored (treble)
///////////////////////////////////////
#define M3_PWM_PIN  11
#define M3_LIMIT_INTERRUPT_PIN 3

// remote dancer pine 
#define DANCER_INPUT_PIN    A4     // remote dancer input pin

// speeds to run the different motors at 0-255
#define THUNDER_ROLLERS_START 99  
#define THUNDER_ROLLERS_INCREMENT 25
#define THUNDER_ROLLERS_INCREMENT_DELAY 0
#define THUNDER_ROLLERS_SLOW 100
#define THUNDER_ROLLERS_FAST 255

#define THUNDER_HAMMER_START 254
#define THUNDER_HAMMER_INCREMENT 25
#define THUNDER_HAMMER_INCREMENT_DELAY 0
#define THUNDER_HAMMER_SPEED_HAMMER 255

#define THUNDER_HAMMER_DURATION_BASS 28
#define THUNDER_HAMMER_DURATION_TREBLE 14

#endif
