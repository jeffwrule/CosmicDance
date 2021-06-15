#ifndef AAD_VTBL_CONFIG_H
#define AAD_VTBL_CONFIG_H


#define VTBL_LONG_MOVE_DURATION 300    // seconds

#define VTBL_MOVE_UP 'r'
#define VTBL_MOVE_DOWN 'l'

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////

  // Suggested pin layout, but some older scupltures will have different layouts, check the older code for the exact layout in github
  #define M1_LEFT_PMW 6      // input from left limit switch, expects a NO (normally open) switch
  #define M1_RIGHT_PMW 5     // input from right limit switch, expects a NO (normally open) switch

  #define M1_START_PWM 1
  #define M1_INCREMENT 5

  #define M1_DELAY_MS 10

  #define M1_NEEDS_JUMP_LEFT true
  #define M1_NEEDS_JUMP_RIGHT true
  #define M1_JUMP_DURATION 300

  #define VTBL_SPEED_DOWN 255      // right
  #define VTBL_SPEED_UP 255       // left

  #define M1_MAX_SPEED_RIGHT 255    // up
  #define M1_MAX_SPEED_LEFT  255    // down

  #define M1_JUMP1_MILLS 500
  #define M1_JUMP2_MILLS 500
  #define M1_JUMP_SPEED1 50
  #define M1_JUMP_SPEED2 40

  #define MAX_SECONDS_TO_LIMIT_SWITCH 150

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////

#define M1_LEFT_LIMIT_PIN 2
#define M1_RIGHT_LIMIT_PIN 3

// remote dancer pine 
#define DANCER_INPUT_PIN    4    

#endif
