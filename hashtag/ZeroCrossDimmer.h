#ifndef ZEROCROSSDIMMER_H
#define ZEROCROSSDIMMER_H

#include <TimerOne.h>
#include "Light.h"


#define ZC_MAX_DIMMERS 8
#define ZC_USECONDS_PER_SECOND 1000000.0
#define ZC_HZ 60
#define ZC_ZERO_CROSSES_PER_CYCLE 2



class ZeroCrossDimmer {

 private:

    // parameter initialized variables
    Light **lights;                                // our list of lights
    volatile uint8_t pin_list[ZC_MAX_DIMMERS];    // list of pin to dimmer mappings
    volatile uint8_t num_dimmers;                 // how many pins/dimmers are we working with
    uint8_t zero_cross_pin;                       // pin to get zero crossing interrupts (INPUT)
    uint16_t num_dimm_levels;                     // the cycle is broken up into this many buckets
                                                  // turning the device on at 0 is 100% on turning it on at 200 is 1/200 on (or probably off)
                                         
    int16_t *levels[ZC_MAX_DIMMERS];         // pointer to the current dimm level(s) inside each Light struct for each pin

    volatile uint32_t clock_tick=0;          // variable for Timer1, tracks how many cycles we have processed this dim
    volatile uint32_t max_clock_tick=0;      // what was the max ticks we have seen in the last cycle, help track cycle stability
    volatile uint32_t triac_lock_delay_us;   // how long do we wait to lock the triac is microseconds, typical is 5


    static ZeroCrossDimmer* zero_cross_instance;  // global pointer to the current instance of zeroCrossDimmer class
    
    static void zero_cross_isr();     // called for each zeroCross event
    static void refresh_triac_isr();  // called every interrupt_delay milliseconds to generate num_dimm_levels interrupts / AC cycle
    void zero_cross_int();            // routine called on each zero cross, resets the ticks counter to 0 to start a new cycle
    void refresh_triac();             // excites the triac(s) when the desired number of ticks has passed, this creates a specific dimm level 0 is full on.
                                      // NUM_DIMM_LEVELS + 1 is full off

  public:

    void set_dimmer_level(int16_t *input_list);     // sets each dimmer to the value given in the input, should be num_dimmers long
    void set_dimmer_level(int16_t input_level);     // sets all dimmers the same
    void set_dimmer_level(int16_t input_level, 
                          uint8_t dimmer_number);   // sets the individual dimmer to the given value dimmer_numbers is >= 0 and < num_dimmers
    uint32_t get_max_clock_tick();                  // return the number of ticks in the last cycle
    void ZeroCrossDimmer::display();           // print the details about this zeroCross dimmer config
    
    volatile uint32_t ticks_min=0;      // least value seen
    volatile uint32_t ticks_max=0;      // max value seen
    volatile uint32_t ticks_over=0;     // number of misses over the top
    volatile uint32_t ticks_under=0;    // number of misses under counted
    volatile uint32_t zero_crosses=0;   // number of zero crosses

    ZeroCrossDimmer( Light **light_list,
        unsigned char num_dimmers, 
        unsigned char zero_cross_pin,
        int num_dimm_levels,
        uint32_t triac_lock_delay_us) :
      lights(light_list),
      num_dimmers(num_dimmers),
      zero_cross_pin(zero_cross_pin),
      num_dimm_levels(num_dimm_levels),
      triac_lock_delay_us(triac_lock_delay_us) {        
        // make a list of the pins availabe in a local variable
        // make list of levles that point into the light levels
        for (uint8_t i=0; i<num_dimmers; i++) {
          pin_list[i] = lights[i]->pin;
          levels[i] =  &(lights[i]->level);  // a pointer to the level in the lights array; changes to lights will be reflected in this array too
          pinMode(pin_list[i], OUTPUT);     // initialize the output pins
        }
        
        zero_cross_instance = this;
        attachInterrupt(digitalPinToInterrupt(zero_cross_pin), zero_cross_isr, RISING);
        unsigned long interrupt_delay = ZC_USECONDS_PER_SECOND / ZC_HZ  / ZC_ZERO_CROSSES_PER_CYCLE / num_dimm_levels; 
        Timer1.initialize(interrupt_delay);           // set a delay that achieves num_dim_levels interrupts during each A/C Cycle (see above calculation)
        Timer1.attachInterrupt( refresh_triac_isr ); // attach the service routine here
    }

};

static ZeroCrossDimmer* ZeroCrossDimmer::zero_cross_instance=NULL;    // global pointer to the instance of zeroCrossDimmer

static void ZeroCrossDimmer::zero_cross_isr() {
  if ( zero_cross_instance ) zero_cross_instance->zero_cross_int();
}

static void ZeroCrossDimmer::refresh_triac_isr() {
  if ( zero_cross_instance ) zero_cross_instance->refresh_triac();
}

void ZeroCrossDimmer::refresh_triac() {
  clock_tick++;

  boolean is_high = false;  // set to true when one or more triacs where set high 
  boolean high_gate[ZC_MAX_DIMMERS] = { false, false, false, false, false, false, false, false };

  for (int i=0; i<num_dimmers; i++) {
    if (*levels[i] == clock_tick) {
      digitalWrite(pin_list[i], HIGH);
      is_high = true;
      high_gate[i] = true;
    }
  }

  if (is_high == true ) {
    delayMicroseconds(triac_lock_delay_us);   // let all the set gates lock
    for (int i=0; i<num_dimmers; i++) {
      if (high_gate[i] == true ) {
        digitalWrite(pin_list[i], LOW);
      }
    }
  }
}

/*
 * This function is called each time the A/C line switchs polarity either 60/second or 50/second
 * depending on the power source in your country
 * This resets the clock tick global variable and restarts the process
 * of turning on the dimmers 
 */
void ZeroCrossDimmer::zero_cross_int() // function to be fired at the zero crossing to dim the light
{
  zero_crosses++;

  if (clock_tick < 180) ticks_under++;
  if (clock_tick > 205) ticks_over++;

  if (clock_tick < ticks_min) ticks_min = clock_tick;
  if (clock_tick > ticks_max) ticks_max = clock_tick;

  max_clock_tick=clock_tick;
  clock_tick=0;
}

void ZeroCrossDimmer::set_dimmer_level(int16_t *input_list) {
  for (int i=0; i<num_dimmers; i++) {
    *(levels[i]) = input_list[i];
  }
}

void ZeroCrossDimmer::set_dimmer_level(int16_t input_level) {
  for (int i=0; i<num_dimmers; i++) {
    *(levels[i]) = input_level;
  }
}

void ZeroCrossDimmer::set_dimmer_level(int16_t input_level, uint8_t dimmer_number) {
  *(levels[dimmer_number]) = input_level;
}

uint32_t ZeroCrossDimmer::get_max_clock_tick() {
  return max_clock_tick;
}

// display the state of the instnace variables
void ZeroCrossDimmer::display() {
  Serial.println(F("ZeroCrossDimmer->Light List: "));
  for (uint8_t i=0; i<num_dimmers; i++) {
    Serial.print(F("    Dimmer "));
    Serial.print(i);
    Serial.print(F(": "));
    lights[i]->display();
    Serial.println(F(""));
  }
  Serial.print(F("    pin_list: "));
  for (uint8_t i=0; i<num_dimmers; i++) {
    Serial.print(pin_list[i]);
    i < num_dimmers - 1 ? Serial.print(F(" , ")) : Serial.println(F(""));
  }
  Serial.print(F("    light levels: "));
  for (uint8_t i=0; i<num_dimmers; i++) {
    Serial.print(*levels[i]);
    i < num_dimmers - 1 ? Serial.print(F(" , ")) : Serial.println(F(""));
  }  
  Serial.print(F("    num_dimmers="));
  Serial.println(num_dimmers);
  Serial.print(F("    zero_cross_pin="));
  Serial.println(zero_cross_pin);
  Serial.print(F("    num_dimm_levels="));
  Serial.println(num_dimm_levels);
  Serial.print(F("    triac_lock_delay_us="));
  Serial.print(triac_lock_delay_us);
  Serial.print(F("    clock_tick="));
  Serial.print(clock_tick);
  Serial.print(F("    max_clock_tick="));
  Serial.print(max_clock_tick);
  Serial.print(F("    triac_lock_delay_us="));
  Serial.print(triac_lock_delay_us);
  Serial.println(F(""));
}

#endif
