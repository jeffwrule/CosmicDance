#include <arduino.h>
#include "config.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// HERE IS WHERE WE INCLUDE THE AAB_devince name include to give this script it personality //
//////////////////////////////////////////////////////////////////////////////////////////////
//#include "AAA_matter.h"
//#include "AAC_water.h"
#include "AAB_antimatter.h"


void loop()
{
  
  print_timer->update();
  // comment this out of you don't have a fob

  g_toggle_switch->update(print_timer->current_millis);
  
  /* off */
  if (!g_toggle_switch->state()) {
    /* make sure to immediately write the current low dimm state */
    if (g_toggle_switch->state() != g_toggle_last_state) {
      dimm_all(print_timer);
      g_toggle_last_state=g_toggle_switch->state();
    }
    do_dimm_up_down(print_timer);
  } else { /* on */
    if (g_toggle_switch->state() != g_toggle_last_state) {
      g_toggle_last_state=g_toggle_switch->state();
    }    // update the lights
    my_nucleon->update(print_timer);
  }

}
