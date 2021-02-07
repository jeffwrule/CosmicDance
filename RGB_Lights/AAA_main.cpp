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

#ifdef HAS_FOB
  fob->update();
  if (!fob->fob_is_dancing) {
    do_dimm_up_down(print_timer);
  } else {
#endif
  // update the lights
  my_nucleon->update(print_timer);
#ifdef HAS_FOB
  }
#endif

}
