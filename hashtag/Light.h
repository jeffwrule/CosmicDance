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

typedef struct LightLevel {
  int16_t target_level;
  boolean is_oscellating;
};

class Light {

  public:

    static  uint32_t next_step_ms;  // when to start executing the next step
    static  int16_t dimm_low;       // level we are starting from (for constraining light and oscellation)
    static  int16_t dimm_high;      // lower bound of our dimm range (will likely be same as either dimm_low or dimm_high)
    int16_t dimm_target;            // where we are headed
    uint8_t pin;                    // pin this light is associated with
    volatile int16_t level;         // current level for this light; this value will be handed to the ZeroCrossDimmer to set the current level

    boolean is_oscellating; // should we reverse the level 

    void run(uint32_t cur_ms);     // adjust the light levels
    void Light::display();
      
    Light (uint8_t pin, int16_t level) :
      pin(pin),
      level(level)
    {
      
    }

};

static  uint32_t Light::next_step_ms=0;  // when to start executing the next step
static  int16_t  Light::dimm_low=0;       // level we are starting from (for constraining light and oscellation)
static  int16_t  Light::dimm_high=0;      // lower bound of our dimm range (will likely be same as either dimm_low or dimm_high)

void Light::run(uint32_t cur_ms) {
  if (cur_ms > next_step_ms) {
    if (level > dimm_target) {
      level = constrain(level - 1 , dimm_low, dimm_high);
    } else {
      level = constrain(level + 1,  dimm_low, dimm_high); 
    }
  }  
}

void Light::display() {
  Serial.print(F(" (Light: pin="));
  Serial.print(pin);
  Serial.print(F(", level="));
  Serial.print(level);
  Serial.print(F(") "));
}
#endif
