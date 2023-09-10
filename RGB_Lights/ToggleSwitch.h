#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H


// only update the button once every 1/10 of a second
#define UPDATE_TOGGLE_EVERY_MS 100
#define DEBOUNCE_PRECENT 80
const uint8_t kOnOffPin=12;                     // digital pin to read when switch is triggered


      

/* 
 *  Motion Sensor Button
 */

class ToggleSwitch {

  private:
  
    uint16_t      switch_pin_;                // pin used to read switch; active low
    boolean       toggled_;                   // current button state;
    unsigned long last_update_;               // the last time we updated the button state
    char          *id_;                        // name of switch

  public:

    void ToggleSwitch::update(unsigned long cur_ms);
    boolean ToggleSwitch::state();

  
    ToggleSwitch(uint8_t switch_pin, char *id) 
      : switch_pin_(switch_pin),                // The pin that triggers this switch
        id_(id),                                // identifier for this switch
        toggled_(LOW),                          // default to off, but it will get updated shortly
        last_update_(0)                         // set to zero so next call will exceed and update
    {
      Serial.println();
      Serial.println("ToggleSwitch INIT BEGIN");

      pinMode(switch_pin_, INPUT_PULLUP);
      pinMode(13, OUTPUT);

      digitalWrite(13, LOW);

      Serial.print("ToggleSwitch INIT: switch_pin_="); Serial.print(switch_pin_);
      Serial.print(", id: "); Serial.print(id_);
      Serial.println();

      Serial.println("ToggleSwitch INIT END");
      Serial.println();
    };

  
};

void ToggleSwitch::update(unsigned long cur_ms) {
      /* 
       *  switch to the new state but only if 80% of the reads are in agreement
       */
      const uint8_t     num_reads=5;                                      // how many reads will we do
      const double      on_threshold=num_reads * DEBOUNCE_PRECENT / 100;  // if 80% of reads on 
      const double      off_threshold=num_reads - on_threshold;           // if 80% of reads off

      // check if it is time to update yet
      if (cur_ms - last_update_ < UPDATE_TOGGLE_EVERY_MS) {
        return;
      }

      last_update_ = cur_ms;    // reset to last read
      
      double pin_reads=0;               // lets average 3 reads
      for (uint16_t i=0; i < num_reads; i++) {
        pin_reads += digitalRead(switch_pin_)==LOW;
      }
      
      if (!toggled_ && pin_reads >= on_threshold) { 
        toggled_=HIGH; 
        digitalWrite(13,HIGH);
 
        Serial.println();
        Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        Serial.print("ToggleSwitch "); Serial.print(id_), Serial.print(" changed state to: On");
        Serial.println();
        Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        Serial.println();
      }
      
      if (toggled_ && pin_reads <=off_threshold) {
        toggled_=LOW;
        digitalWrite(13,LOW);

        Serial.println();
        Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        Serial.print("ToggleSwitch "); Serial.print(id_), Serial.print(" changed state to: Off");
        Serial.println();
        Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        Serial.println();
      }
}

boolean ToggleSwitch::state() {
  /* return the current state of the switch */
  return toggled_;
}



#endif
