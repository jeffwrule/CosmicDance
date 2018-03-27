#ifndef CONFIG_H
#define CONFIG_H

// forward declerations for helpers
char *bool_tostr(boolean input_bool);
unsigned short digitalReads(unsigned int pin, unsigned int reads);

#define SERIAL_SPEED 250000
#define PRINT_AFTER_SECONDS 3    // print every n seconds 

#define IS_BRIEF True
// #define IS_CHATTY     // define this for extra debug information 

// debug tool, do not leave set for normal user
// #define ALWAYS_BE_DANCING     // uncomment for debug, normally commented out, set remote_is_dancing_ 1 (helps with floating line)
//#define NEVER_BE_DANCING        // uncomment for debug, normally commented out, sets remote_is_dancing_ 0 (helps with floating line)

#define DELAY_TYPE_DANCE 'd'      // delay amount is from start of dance
#define DELAY_TYPE_MOVE  'm'      // delay amount is from start of move

#endif
