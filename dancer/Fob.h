#ifndef FOB_H
#define FOB_H
#
class Fob {

  public:
    void update();                                // update the fob status
    void status(boolean do_print);                // dump the fob status
    void fake_fob_is_dancing(boolean is_on);      // called to turn on fake fob (when pin is floating);
    boolean Fob::get_seat_active();               // is the seat switch active
    boolean get_seat_switch();                    // return the status of the seat switch
    boolean fob_is_dancing;                       // has the fob been pushed so we are dancing
    boolean always_be_dancing;                    // fake it as if the fob was pushed
    boolean use_seat_switch;                      // are we using the seat switch]
    boolean seat_active;                           // is the seat being used;
    
    Fob(unsigned short fob_pin) : 
      fob_pin(fob_pin),
      always_be_dancing(FAKE_FOB_IS_DANCING), 
      use_seat_switch(IS_SEAT_SWITCH),
      fob_was_pushed(false),
      fob_is_dancing(false) {
        pinMode(fob_pin, INPUT_PULLUP);
        delay(100);
        pinMode(SEAT_PIN, INPUT_PULLUP);
  
        fob_open = digitalRead(fob_pin);
        delay(100);
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

        seat_active = false;
    }

  private:
    unsigned short  fob_pin;         // the fob pin
    unsigned short  fob_open;        // how pin reads when it is not pushed
    boolean fob_was_pushed;          // was the fob just pushed
    boolean fob_read1;               // was the fob just pushed
    boolean fob_read2;               // was the fob just pushed
    boolean fob_read3;               // was the fob just pushed
    boolean seat_read1;               // was the seat is active 
    boolean seat_read2;               // was the seat is active 
    boolean seat_read3;               // was the seat is active 
};

// check the fob status
void Fob::update() {
  // for testing when no wires attached to arduino
  // this line tends to float back and forth so just 
  // pin it as not pushed and set indicator to say we are dancing...

  
  // override if we are seat switched
  if (use_seat_switch == true) {
    fob_was_pushed = false;

    // attempt to debounce....
    delay(10);
    seat_read1 = digitalRead(SEAT_PIN) == 0 ? false : true;    // 0 is resting state for seat, switch is normally closed so pulled low
    delay(10);
    seat_read2 = digitalRead(SEAT_PIN)  == 0 ? false : true;
    delay(500);
    seat_read3 = digitalRead(SEAT_PIN)  == 0 ? false : true;
    if ( seat_read1 == seat_read2 && seat_read2 == seat_read3) {
      // we have a switched state
      if (seat_active != seat_read1) {
        fob_was_pushed = true;
        seat_active = seat_read1;
        fob_is_dancing = seat_active;
      } 
    }     
  } else {
    if (always_be_dancing) {
      #ifdef IS_CHATTY
        Serial.println(F("Faking FOB"));
      #endif
      fob_is_dancing = true;
    } else { // only bother to read the pin if we are not faking the fob
      // attempt to debounce....
      fob_read1 = !digitalRead(fob_pin) == fob_open;
      delay(10);
      fob_read2 = !digitalRead(fob_pin) == fob_open;
      delay(10);
      fob_read3 = !digitalRead(fob_pin) == fob_open;
      if ( fob_read1 == fob_read2 && fob_read2 == fob_read3) {
        fob_was_pushed = fob_read1;
        delay(1000);
      } else {
        fob_was_pushed = false;
        delay(1000);
      }
    }
    
    delay(30);    // add a short delay here, checking this seems to tweek the whole system.
    #ifdef IS_CHATTY
      Serial.print(F("Fob read returns: fob_was_pushed="));
      Serial.println(bool_tostr(fob_was_pushed));
    #endif
  
    if (fob_was_pushed) { // we have a switch request....
      fob_was_pushed = false;
      // fob button has been pushed, reset everything and switch mode
      if (fob_is_dancing) {
        fob_is_dancing = false;  
        Serial.println(F("FOB is now off..."));
      } else {
        fob_is_dancing = true;
        Serial.println(F("FOB is now on..."));
      }
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
  Serial.print(F(", use_seat_switch="));
  Serial.print(bool_tostr(use_seat_switch));
  Serial.print(F(", seat_active="));
  Serial.print(bool_tostr(seat_active));
  Serial.print(F(", seat_read1="));
  Serial.print(seat_read1);;
  Serial.print(F(", seat_read2="));
  Serial.print(seat_read2);;
  Serial.print(F(", seat_read3="));
  Serial.print(seat_read3);;
  Serial.print(F(", SEAT_PIN="));
  Serial.print(digitalRead(SEAT_PIN));
  Serial.print(F(", always_be_dancing="));
  Serial.println(bool_tostr(always_be_dancing));
}

void Fob::fake_fob_is_dancing(boolean is_on) {
  always_be_dancing = is_on;      // turn this on for fake fob is dancing
  fob_was_pushed = true;          // fake the push the first time to make we catch other setup things
}

boolean Fob::get_seat_switch() {
  return use_seat_switch;
}

boolean Fob::get_seat_active() {
  return seat_active;
}
#endif
