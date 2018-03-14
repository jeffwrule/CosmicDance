#ifndef DANCER_H
#define DANCER_H

class Dancer {

  public:
    String  dancer_name;                // The current dancer name
    status(PrintTimer *pt);             // print the current dancer status
    init_values();                      // print the init values for this object
    update(unsigned long current_ms);   // update the dancer pin read

    // dancer_name      name this dancer
    // dance_pin        pin to read if remote is dancing
    // dimmer_list      list of dimmer strings to control
    // num_dummers      number of dimmers in the list
    Dancer(String dancer_name, int dancer_pin, Dimmer *dimmer_list, int num_dimmers) :
      dancer_name(dancer_name),   
      dancer_pin(dancer_pin), 
      dimmer_list(dimmer_list),
      num_dimmers(num_dimmers)
      {
        pinMode(DANCER_PIN, INPUT);
        remote_is_dancing = digitalRead(dancer_pin) == 1; // peek at the remote pin
        is_dancing = !remote_is_dancing;      // ensure a mis-match on first update
      } 

  private:
    int dancer_pin;             // pin to read for remote dancer status
    boolean is_dancing;         // are we dancing or not
    boolean remote_is_dancing;  // is the remote dancng 
    Dimmer *dimmer_list;        // list of dimmers to work with
    int num_dimmers;            // number of dimmers

};

Dancer::init_values() {
  Serial.print(F("DANCER_INIT: dancer_name="));
  Serial.print(dancer_name);
  Serial.print(F(", dancer_pin="));
  Serial.print(dancer_pin); 
  Serial.print(F(", num_dimmers="));
  Serial.println(num_dimmers); 
  for (int i=1; i<num_dimmers; i++) {
    Serial.print(F("Dimmer# "));
    Serial.print(i);
    Serial.print(F(", name="));
    Serial.println(dimmer_list[i].dimmer_name);
    dimmer_list[i].init_values();
  }
}

Dancer::status(PrintTimer *pt) {

  if (!pt->get_do_print()) {
    return;
  }
  
  Serial.print(F("Dancer::status() dancer_name="));
  Serial.print(dancer_name);
  Serial.print(F(", current_ms="));
  Serial.print(pt->current_millis);
  Serial.print(F(", is_dancing="));
  Serial.print(bool_tostr(is_dancing));
  Serial.print(F(", remote_is_dancing="));
  Serial.println(bool_tostr(remote_is_dancing));
  for (int i=0; i<num_dimmers; i++) {
    dimmer_list[i].status(pt);
  }
}

Dancer::update(unsigned long current_millis) {

  int  pin_reads;               // sum of multiple reads of the pin
  int  num_loops=5;             // number of reads to execute on the pin

  // get an average read from the remote pin (helps w/ bouncing etc)
  for (int i=0; i<num_loops; i++) {
    pin_reads += digitalRead(dancer_pin);
  }

  // if number of reads > num_loops/2 pin is on
  if (pin_reads >= num_loops/2.0) {
    remote_is_dancing = true;
  } else {
    remote_is_dancing = false;
  }

  #ifdef IS_CHATTY
    Serial.print(F("Dancer::update() pin status, pin_reads="));
    Serial.print(pin_reads);
    Serial.print(F(", num_loops="));
    Serial.print(num_loops);
    Serial.print(F(", remote_is_dancing="));
    Serial.print(bool_tostr(remote_is_dancing));
    Serial.print(F(", is_dancing="));
    Serial.println(bool_tostr(is_dancing));
  #endif
  if (is_dancing && remote_is_dancing == 0) {
    // stop dancing
    #if defined IS_BRIEF
      Serial.print(F("DANCE_STOPPED: current_millis: "));
      Serial.println(current_millis);
    #endif
    for (int i=0; i<num_dimmers; i++) {
      dimmer_list[i].reset(current_millis);
    }
    is_dancing = false;
  } else if (!is_dancing && remote_is_dancing == 1 ) {
    // start dancing
    #if defined IS_BRIEF
      Serial.print(F("DANCE_STARTED: current_ms: "));
      Serial.println(current_millis);
    #endif
    for (int i=0; i<num_dimmers; i++) {
      dimmer_list[i].start(current_millis);
    }
    is_dancing = true;
  }

  // update the power numbers for internal dimmers
  for (int i=0; i<num_dimmers; i++) {
    dimmer_list[i].update(current_millis);
  }
}


#endif
