#ifndef AAD_WATER_CONFIG_H
#define AAD_WATER_CONFIG_H


#define WATER_LONG_MOVE_DURATION 300    // seconds

#define WATER_MOVE_UP 'r'
#define WATER_MOVE_DOWN 'l'

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////

  // Suggested pin layout, but some older scupltures will have different layouts, check the older code for the exact layout in github
  #define M1_LEFT_PMW 10      // input from left limit switch, expects a NO (normally open) switch
  #define M1_RIGHT_PMW 11     // input from right limit switch, expects a NO (normally open) switch

  #define M1_START_PWM 1
  #define M1_INCREMENT 5

  #define M1_DELAY_MS 10

  #define M1_NEEDS_JUMP_LEFT true
  #define M1_NEEDS_JUMP_RIGHT true
  #define M1_JUMP_DURATION 300

  #define WATER_SPEED_DOWN 40      // right
  #define WATER_SPEED_UP 40       // left

  #define M1_MAX_SPEED_RIGHT 40    // up
  #define M1_MAX_SPEED_LEFT 40    // down

  #define M1_JUMP1_MILLS 500
  #define M1_JUMP2_MILLS 500
  #define M1_JUMP_SPEED1 50
  #define M1_JUMP_SPEED2 40

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////

#define M1_LEFT_LIMIT_PIN 2
#define M1_RIGHT_LIMIT_PIN 3

// remote dancer pine 
#define DANCER_INPUT_PIN    A0     

#endif
