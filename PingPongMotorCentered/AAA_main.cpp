#include <Arduino.h>
#include "config.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// HERE IS WHERE WE INCLUDE THE BBB_devince name include to give this script it personality //
//////////////////////////////////////////////////////////////////////////////////////////////
//#include "AAB_sitx2.h"
//#include "AAB_nostay.h"
#include "AAD_water.h"

// watch for limits and request to enable/disable the motor
void loop() {
  
  print_timer->update();
  print_timer->status();
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->update();
      dancers[i]->status();
  }
  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->dance();
    }
}
