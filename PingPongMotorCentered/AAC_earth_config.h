#ifndef AAC_EARTH_CONFIG_H
#define AAC_EARTH_CONFIG_H


#define EARTH_LONG_MOVE_DURATION 300    // seconds

#define EARTH_MOVE_UP 'r'
#define EARTH_MOVE_DOWN 'l'

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////

  // Suggested pin layout, but some older scupltures will have different layouts, check the older code for the exact layout in github
  #define M1_LEFT_PMW 10      // input from left limit switch, expects a NO (normally open) switch
  #define M1_RIGHT_PMW 11     // input from right limit switch, expects a NO (normally open) switch

  #define M1_START_PWM 10
  #define M1_INCREMENT 5

  #define M1_DELAY_MS 10

  #define M1_NO_JUMP_START false
  #define M1_JUMP_DURATION 0

  #define EARTH_SPEED_DOWN 45      // right
  #define EARTH_SPEED_UP 45        // left

  #define M1_MAX_SPEED_RIGHT 45    // up
  #define M1_MAX_SPEED_LEFT 45     // down

  #define M1_JUMP1_MILLS 10
  #define M1_JUMP2_MILLS 10
  #define M1_JUMP_SPEED1 45
  #define M1_JUMP_SPEED2 45

//////////////////////////////////////////
///// MOTOR 1 PINS
//////////////////////////////////////////

#define M1_LEFT_LIMIT_PIN 2
#define M1_RIGHT_LIMIT_PIN 3

// remote dancer pine 
#define DANCER_INPUT_PIN    A0     

#endif
