#ifndef PRINTTIMER_H
#define PRINTETIMER_H

// class to control print speed so we don't overwhelm the serial port
class PrintTimer {
  
  public:
    void    update();           // update if it is time to print
    void    print_now();        // override the counter and print now
    boolean get_do_print();     // can we print
    void    init_values();      // inital values for the print timer
    void    status();           // dump the current status of the print timer
    unsigned long current_millis;   // milliseconds at last update
    
    PrintTimer(unsigned long print_after_seconds) : 
      print_after_millis(print_after_seconds*1000) { 
      print_diff_millis=0;
      update();
      }

  private:
    boolean do_print;                   // the the print flag
    unsigned long print_after_millis;   // amount of time to wait between prints
    unsigned long last_print_millis;     // time since our last update;
    unsigned long print_diff_millis;   // difference between last print and this print;
};

// Compute if enough loops have happened to print again
void PrintTimer::update() {
  unsigned long new_millis = millis();
  if (new_millis < current_millis) { last_print_millis=0; }  // check for wrap around on current_millis  
  current_millis = new_millis;
  do_print=false;
  if ((current_millis - last_print_millis) >= print_after_millis) {
    do_print = true;
    print_diff_millis=current_millis - last_print_millis;
    last_print_millis = current_millis;
  }
}

// allow the sketch to override from time to time and call for a print
void PrintTimer::print_now() {
  do_print = true;
}

// retun the value of the do_print status
boolean PrintTimer::get_do_print() {
  return do_print;
}

void PrintTimer::init_values() {
  Serial.print(F("PRINTTIMER_INIT: print_after_millis="));
  Serial.println(print_after_millis);
}

void PrintTimer::status() {
  if (!do_print) {
    return;
  }
  Serial.print(F("PrintTimer::status() do_print="));
  Serial.print(bool_tostr(do_print));
  Serial.print(F(", current_millis="));
  Serial.print(current_millis);
  Serial.print(F(", last_print_millis="));
  Serial.print(last_print_millis);
  Serial.print(F(", print_diff_millis="));
  Serial.println(print_diff_millis);
}

#endif
