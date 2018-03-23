#ifndef FOB_H
#define FOB_H
#
class Fob {

  public:
    void update();                                // update the fob status
    void status(boolean do_print);                // dump the fob status
    void fake_fob_is_dancing(boolean is_on);      // called to turn on fake fob (when pin is floating);
    boolean fob_is_dancing;                       // has the fob been pushed so we are dancing
    boolean always_be_dancing;                    // fake it as if the fob was pushed
    
    Fob(unsigned short fob_pin) : 
      fob_pin(fob_pin),
      always_be_dancing(false), 
      // fob_was_pushed(false),
      fob_is_dancing(false) {
        pinMode(fob_pin, INPUT_PULLUP);
        delay(40);
  
        fob_open = digitalRead(fob_pin);
        Serial.print(F("FOB::CONSTRUCTOR fob_pin="));
        Serial.print(fob_pin);
        Serial.print(F(", Fob is detected as "));
        if (fob_open == 1 ) {
          Serial.print("NO (normally open)");  // has a pull-up resistor so open = 1
        } else {
          Serial.print("NC (normally closed)");  // pin tied to ground so closed = 0
        }
        Serial.print(F(", fob_open="));
        Serial.println(fob_open);
    }

  private:
    unsigned short  fob_pin;         // the fob pin
    unsigned short  fob_open;        // how pin reads when it is not pushed
    boolean fob_was_pushed;          // was the fob just pushed

};

// check the fob status
void Fob::update() {
  // for testing when no wires attached to arduino
  // this line tends to float back and forth so just 
  // pin it as not pushed and set indicator to say we are dancing...
  if (always_be_dancing) {
    #ifdef IS_CHATTY
      Serial.println(F("Faking FOB"));
    #endif
    fob_is_dancing = true;
  } else { // only bother to read the pin if we are not faking the fob
    fob_was_pushed = !digitalRead(fob_pin) == fob_open;
    delay(30);    // add a short delay here, checking this seems to tweek the whole system.
    #ifdef IS_CHATTY
      Serial.print(F("Fob read returns: fob_was_pushed="));
      Serial.println(bool_tostr(fob_was_pushed));
    #endif
  }
  
  if (fob_was_pushed) { // we have a switch request....
    fob_was_pushed = false;
    // fob button has been pushed, reset everyting and switch mode
    if (fob_is_dancing) {
      fob_is_dancing = false;  
      Serial.println(F("FOB is now off..."));
    } else {
      fob_is_dancing = true;
      Serial.println(F("FOB is now on..."));
    }
  }

}

// dump the current fob status
void Fob::status(boolean do_print) {
  if (!do_print) {
    return;
  }

  Serial.print(F("Fob::status()  fob_is_dancing="));
  Serial.print(bool_tostr(fob_is_dancing));
  Serial.print(F(", fob_was_pushed="));
  Serial.print(fob_was_pushed);
  Serial.print(F(", always_be_dancing="));
  Serial.println(bool_tostr(always_be_dancing));
}

void Fob::fake_fob_is_dancing(boolean is_on) {
  always_be_dancing = is_on;      // turn this on for fake fob is dancing
  fob_was_pushed = true;          // fake the push the first time to make we catch other setup things
}
#endif
