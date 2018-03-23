#ifndef CONFIG_H
#define CONFIG_H

char* bool_tostr(boolean input_bool);  /// forward declare bool_tostr;

#define SERIAL_SPEED  250000
#define PRINT_AFTER_SECONDS 2

#define IS_BRIEF True
//#define IS_CHATTY True 

// these are our dimmer pins on an UNO or Nano
#define PMW1 3
#define PMW2 5
#define PMW3 6
#define PMW4 9
#define PMW5 10
#define PMW6 11

#define DANCER_PIN A0

#endif
