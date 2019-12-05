#ifndef LIGHT_H
#define LIGHT_H


/*
 * a light object that can have a 
 *     min_level
 *     target_elvel
 *     current_level
 *     delay 
 *     breath  
 */

class Light {  

  private:


  public:

    const uint8_t pin;              // pin this light is associated with
    volatile int16_t level;         // current level for this light; this value will be handed to the ZeroCrossDimmer to set the current level
    int16_t dimm_target;            // where we are headed
    int8_t  dimm_direction;         // use one of the constants DIMM_UP, DIMM_DOWN, DIMM_STOP
    boolean is_oscellating;         // should we reverse the level 
    

    uint32_t  dimm_delay_until_ms;  // how long to delay between steps to dimm this light in the given time
    uint32_t  dimm_delay_size_ms;   // how long is each step

    void display();                                                 // dump our dimmer structure
    void set_dimm_delay_size_ms(uint32_t dimm_total_length);        // re-compute the step size and duration based on new length
    void adjust_dimmer(LightPrep *cur_prep, Step *cur_step);        // adjust this dimmer level if it is time


    Light (uint8_t pin, int16_t level, uint16_t dimm_target, int8_t dimm_direction, bool is_oscellating) :
      pin(pin),
      level(level),
      dimm_target(dimm_target),
      dimm_direction(dimm_direction),
      is_oscellating(is_oscellating) 
      {
        Serial.println(F("Light Constructor Starting"));
        dimm_delay_size_ms = 1000;
        dimm_delay_until_ms = cur_ms  + dimm_delay_size_ms;   // default to slow dimming, will be updated later
      }
};

void Light::set_dimm_delay_size_ms(uint32_t dimm_total_length) {

  uint32_t abs_diff = abs(level - dimm_target);
  if (abs_diff == 0) {
    dimm_delay_size_ms = dimm_total_length;
  } else {
    dimm_delay_size_ms =  dimm_total_length / abs_diff; 
  }

  if (dimm_delay_size_ms == 0) dimm_delay_size_ms=1;
  
  dimm_delay_until_ms = cur_ms + dimm_delay_size_ms;
    
}

void Light::adjust_dimmer(LightPrep *cur_prep, Step *cur_step) {

  if (cur_ms < dimm_delay_until_ms) return;   // time to make the donughts?
  
  if (dimm_target == level) {     // is the dimming complete?
    if (is_oscellating) {         // is this an oscellating light?
        switch (dimm_direction) { // yes, reverse direction
          case DIMM_UP: 
                        dimm_direction = DIMM_DOWN;
                        dimm_target = cur_prep->dimm_min;
                        break;

          
          case DIMM_DOWN: 
                        dimm_direction = DIMM_UP;
                        dimm_target = cur_prep->dimm_max;
                        break;

          
          case DIMM_STOP:
                        break;

        }
        // use cycle length for oscellating is in the original step
        set_dimm_delay_size_ms(cur_step->dimm_length_ms);
      } else {
        if (dimm_delay_size_ms < 10000) dimm_delay_size_ms = 10000; // dimmer has arrived and not cycling increase delay time to quiet it down
      }

    } else if (dimm_target < level) { // dimm up here lower number are brighter
      // Serial.println(F("Level up --"));
      level--;
    } else {                                                // dimm down here, higher numbers are dimmer
      // Serial.println(F("Level down ++"));
      level++;
    }

    dimm_delay_until_ms = cur_ms + dimm_delay_size_ms;
}

    const uint8_t pin;              // pin this light is associated with
    volatile int16_t level;         // current level for this light; this value will be handed to the ZeroCrossDimmer to set the current level
    int16_t dimm_target;            // where we are headed
    int8_t  dimm_direction;         // use one of the constants DIMM_UP, DIMM_DOWN, DIMM_STOP
    boolean is_oscellating;         // should we reverse the level 
    

    uint32_t  dimm_delay_until_ms;  // how long to delay between steps to dimm this light in the given time
    uint32_t  dimm_delay_size_ms;   // how long is each step

void Light::display() {
  Serial.print(F("Light::display() (Light: pin="));
  Serial.print(pin);
  Serial.print(F(", level="));
  Serial.print(level);
  Serial.print(F(", dimm_target="));
  Serial.print(dimm_target);
  Serial.print(F(", dimm_direction="));
  Serial.print(dimm_direction);
  Serial.print(F(", is_oscellating="));
  Serial.print(is_oscellating);
  Serial.print(F(", dimm_delay_until_ms="));
  Serial.print(dimm_delay_until_ms);
  Serial.print(F(", dimm_delay_size_ms="));
  Serial.print(dimm_delay_size_ms);
  Serial.print(F(", dimm_delay_until_ms-cur_ms="));
  Serial.print(dimm_delay_until_ms-cur_ms);
  Serial.println(F(") "));
}
#endif
