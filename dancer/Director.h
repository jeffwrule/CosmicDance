#ifndef DIRECTOR_H
#define DIRECTOR_H

class Director {
  public:
    void status(boolean do_print);          // print a status message
    void update();    // alias for check_for_direction
    boolean has_new_direction();            // do we have some new direction from the director
    unsigned char get_latest_direction();      // return the last thing the director sent
    void ignore_pending();            // read all the queued direction and throw it aways
    void send(unsigned char msg);     // send something back to the director              

    Director(GenericXbee *xbee_device, Fob *fob_device) : xbee_device(xbee_device), fob_device(fob_device) { new_direction = empty_request; }

  private:

    const unsigned char empty_request = '\0';  // not a request

    GenericXbee *xbee_device;         
    Fob *fob_device;                      
    unsigned char new_direction;            // the latest direction from the director
  
};

void Director::status(boolean do_print) {
  if (! do_print) { return; }
  Serial.print(F("Director::status() new_direction="));
  Serial.println((char)new_direction);
  if (!fob_device->fob_is_dancing && new_direction != empty_request && (xbee_device->xbee_available() > 0)) {
    Serial.println(F("    WARNING: commands waiting to be consumed"));
  }
}

/*
 * check the network to see if we any new directions 
 * from the director
 */
void Director::update() {
  // read until we get what we want or noting left to read
  #if defined IS_CHATTY
    Serial.println(F("Director::checking for direction..."));
  #endif

  // director does not get updates when the FOB is active
  if (fob_device->fob_is_dancing) { return; }
  
  // only update new_direction if the current new_direction has been consumed
  if (xbee_device->xbee_available() > 0) {
    if (new_direction == empty_request) {
      new_direction = xbee_device->xbee_read();
      #if defined IS_BRIEF
        Serial.print(F("check_for_direction:Got something: "));
        Serial.println((char)new_direction);
      #endif
    }
  }
}

// do we have new direction from the director
boolean Director::has_new_direction() {
  return new_direction != empty_request;
}
// return the latest direction we recived and clear the direction
unsigned char Director::get_latest_direction() {
    unsigned char last_direction;
    last_direction=new_direction;
    new_direction = empty_request;
    return last_direction;
}

// send a message back to the director
void Director::send(unsigned char msg) {
  #ifdef IS_CHATTY
    Serial.print(F("Director::send() msg="));
    Serial.println((char)msg);
  #endif
  xbee_device->xbee_write(msg);
}

// drain all pending input and clear any current message
void Director::ignore_pending() {
    while (xbee_device->xbee_available() > 0) {
      update();  // we def want to drain the input so pass false for fob_is_dancing
      new_direction = empty_request;
    }
}


#endif
