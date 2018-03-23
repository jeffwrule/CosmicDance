#ifndef CONFIGS_H
#define CONFIGS_H

// forward declerations for helpers
char *bool_tostr(boolean input_bool);
unsigned short digitalReads(unsigned int pin, unsigned int reads);

#define SERIAL_SPEED 250000
#define PRINT_AFTER_SECONDS 3    // print every n seconds 

#define IS_BRIEF True
// #define IS_CHATTY     // define this for extra debug information 

//#define PIN_READ_DELAY_MS  400

// debug tool, do not leave set for normal user
#define ALWAYS_BE_DANCING     // uncomment for debug, normally commented out, set remote_is_dancing_ 1 (helps with floating line)
//#define NEVER_BE_DANCING        // uncomment for debug, normally commented out, sets remote_is_dancing_ 0 (helps with floating line)


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
#define THUNDER_SPEED_START 25   
#define THUNDER_SPEED_SLOW 70   
#define THUNDER_SPEED_JUMPSTART 100
#define THUNDER_SPEED_FAST 110
#define THUNDER_SPEED_MAX 200

#endif
