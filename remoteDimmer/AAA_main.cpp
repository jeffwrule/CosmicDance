#include <arduino.h>
#include </Users/jrule/Documents/Arduino/remoteDimmer/config.h>

//////////////////////////////////////////////////////////////////////////////////////////////
// HERE IS WHERE WE INCLUDE THE AAB_devince name include to give this script it personality //
//////////////////////////////////////////////////////////////////////////////////////////////
#include </Users/jrule/Documents/Arduino/remoteDimmer/AAB_pastpresentfuture.h>

void loop()
{
  print_timer->update();

  // is the remote dancing or not? (start/stop local timers)
  my_dancer->update(print_timer->current_millis);

  // numbers after check and update
  print_timer->status();
  my_dancer->status(print_timer);
}


