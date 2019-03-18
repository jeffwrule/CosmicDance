#include <arduino.h>
#include "config.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// HERE IS WHERE WE INCLUDE THE AAB_devince name include to give this script it personality //
//////////////////////////////////////////////////////////////////////////////////////////////
//#include "AAB_pastpresentfuture.h"
//#include "AAB_nostay.h"
// #include "AAB_walloflifecenter.h"
#include "AAC_mountain.h"

void loop()
{
  print_timer->update();

  // is the remote dancing or not? (start/stop local timers)
  my_dancer->update(print_timer->current_millis);

  // numbers after check and update
  print_timer->status();
  my_dancer->status(print_timer);
}


