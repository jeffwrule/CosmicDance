#ifndef FOB_H
#define FOB_H
#
class Fob {

  public:
    void update();                                // update the fob status
    void status(boolean do_print);                // dump the fob status
    boolean fob_is_dancing;                       // has the fob been pushed so we are dancing
    
    Fob(unsigned short fob_pin) : 
      fob_pin(fob_pin),
      fob_was_pushed(false),
      fob_is_dancing(false) {
        pinMode(fob_pin, INPUT_PULLUP);
        delay(100);
  
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

  // attempt to debounce....
  fob_read1 = !digitalRead(fob_pin) == fob_open;
  delay(10);
  fob_read2 = !digitalRead(fob_pin) == fob_open;
  delay(10);
  fob_read3 = !digitalRead(fob_pin) == fob_open;
  if ( fob_read1 == fob_read2 && fob_read2 == fob_read3) {
    fob_was_pushed = fob_read1;
    // delay(1000);
  } else {
    fob_was_pushed = false;
   //  delay(1000);
  }
  
  // delay(30);    // add a short delay here, checking this seems to tweek the whole system.


  if (fob_was_pushed) { // we have a switch request....
    fob_was_pushed = false;
    // fob button has been pushed, reset everything and switch mode
    if (fob_is_dancing) {
      fob_is_dancing = false;  
      Serial.println(F("FOB is now off..."));
      digitalRead(fob_pin);   // just because
      delay(1000);
    } else {
      fob_is_dancing = true;
      Serial.println(F("FOB is now on..."));
      digitalRead(fob_pin);   // just because
      delay(1000);
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
  Serial.print(F(", seat_read1="));
  Serial.print(seat_read1);;
  Serial.print(F(", seat_read2="));
  Serial.print(seat_read2);;
  Serial.print(F(", seat_read3="));
  Serial.print(seat_read3);;
}

#endif
