#include <Arduino.h>
#include "config.h"

char* bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

// average the read values and return 1 or 0
unsigned short digitalReads(unsigned int pin, unsigned int reads) {
  int result=0;
  
  for (int i=0; i<reads; i++) {
    result += digitalRead(pin);
  }
  return round(int((result * 1.0) / (reads * 1.0)));
}


  
