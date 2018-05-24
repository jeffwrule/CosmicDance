#include <arduino.h>
#include "config.h"

char* bool_tostr(boolean input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

