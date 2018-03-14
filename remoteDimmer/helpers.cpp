#include <arduino.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/config.h>

String bool_tostr(boolean input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}


