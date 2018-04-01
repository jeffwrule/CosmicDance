#include <Arduino.h>
#include "config.h"

String bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}



