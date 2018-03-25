#ifndef CONFIG_H
#define CONFIG_H

char* bool_tostr(boolean input_bool);  /// forward declare bool_tostr;

#define SERIAL_SPEED  250000
#define PRINT_AFTER_SECONDS 3

#define IS_BRIEF True
//#define IS_CHATTY True 

#define ALWAYS_BE_DANCING false

// initialize the different dimmers 
#define DIMMER_UP_SECONDS 6
#define DIMMER_DOWN_SECONDS 4

#define DIMMER_OFF 0
#define DIMMER_ON  100

#define DIMMER_MIN 0
#define DIMMER_MAX 100

// these are our dimmer pins on an UNO or Nano
#define PMW1 3
#define PMW2 5
#define PMW3 6
#define PMW4 9
#define PMW5 10
#define PMW6 11

#define DANCER_PIN A0

#endif
