#include <Arduino.h>
#include "config.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// HERE IS WHERE WE INCLUDE THE BBB_devince name include to give this script it personality //
//////////////////////////////////////////////////////////////////////////////////////////////
#include "AAB_thunder.h"

// watch for limits and request to enable/disable the motor
void loop() {

  /////////////////////////////////////////
  // update things
  ////////////////////////////////////////
  print_timer->update();
  
  if (print_timer->get_do_print()) {
    Serial.println(F("------------------------- LOOP ------------------------------"));
  }

  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->update();
  }

  /////////////////////////////////////////
  // update things
  ////////////////////////////////////////  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->status();
  }
  
  for (int i = 0; i < NUM_DANCERS; i++) {
      dancers[i]->dance();
    }
}

