#ifndef CONFIG_H
#define CONFIG_H

char* bool_tostr(boolean input_bool);  /// forward declare bool_tostr;

#define SERIAL_SPEED  250000
#define PRINT_AFTER_SECONDS 5

#define IS_BRIEF true
#define IS_CHATTY false 
#define IS_VERBOSE false

//#define ALWAYS_BE_DANCING false

#endif
