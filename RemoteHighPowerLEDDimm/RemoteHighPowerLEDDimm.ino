/*
 * 
 * Simple dimmer.   Watches input PIN, 
 *                  LOW:  set lights to low dimm level.
 *                  PIN:  set lights to high dimm level
 *                  
 *                  Sets all pins the same 0-16
 *                  
 *                  NOTE the level for the dimm works in reverse 0=full on, 4096=full off
 *                        but we offset it, so that 0 is off, and 4095 is full on.
 */

#include "SparkFun_Tlc5940.h"
#define SERIAL_SPEED 250000
#define PRINT_SKIP 200

// dimmer pins on board
#define NUM_PINS 13
#define INPUT_PIN A0

//// Water configs
//    #define DIMM_UP_SECONDS 30 
//    #define DIMM_DOWN_SECONDS 5
//    
//    // min(off) is 0, max(on) is 4095
//    #define MIN_DIMM  49
//    #define MAX_DIMM  4095
//    
//    #define TLC_MAX 4095
//    
//    #define DIMM_UP_INCREMENT 1
//    #define DIMM_DOWN_INCREMENT -5


// Heaven Configs
    #define DIMM_UP_SECONDS 39 
    #define DIMM_DOWN_SECONDS 5
    
    // min(off) is 0, max(on) is 4095
    #define MIN_DIMM  0
    #define MAX_DIMM  4095
    
    #define TLC_MAX 4095
    
    #define DIMM_UP_INCREMENT 30
    #define DIMM_DOWN_INCREMENT -30

int curr_dimm_level;
boolean is_dancing=false;

unsigned int up_delay;
unsigned int down_delay;

void setup()
{

  // Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  
  Serial.println(F("\nStart setup"));
  Serial.println(F("RemoteHighpowerLEDDimm.ino"));

  pinMode(INPUT_PIN, INPUT_PULLUP);

  // start the dim level half way between min and max. 
  // this will allow the normal dimm, to adjust the level up or down depending
  // on the input pin
  int dimm_steps = MAX_DIMM - MIN_DIMM;
  if (dimm_steps != 0) {
    curr_dimm_level = MIN_DIMM + (dimm_steps/2);
  } else {
    curr_dimm_level = MAX_DIMM;
  }

  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init(4095-curr_dimm_level);

  up_delay = (DIMM_UP_SECONDS * 1000/DIMM_UP_INCREMENT) / dimm_steps;
  up_delay = 9;
  up_delay = 1;
  down_delay = (DIMM_DOWN_SECONDS * 1000 / abs(DIMM_DOWN_INCREMENT)) / dimm_steps;

  Serial.print(F("DELAYS:  up_delay="));
  Serial.print(up_delay);
  Serial.print(F(", dimm_steps="));
  Serial.print(dimm_steps);
  Serial.print(F(", curr_dimm_level="));
  Serial.print(curr_dimm_level);
  Serial.print(F(", DIMM_UP_SECONDS="));
  Serial.print(DIMM_UP_SECONDS);
  Serial.print(F(", DIMM_DOWN_SECONDS="));
  Serial.print(DIMM_DOWN_SECONDS);  
  Serial.print(F(", DIMM_DOWN_INCREMENT="));
  Serial.print(DIMM_UP_INCREMENT);
  Serial.print(F(", DIMM_UP_INCREMENT="));
  Serial.print(DIMM_DOWN_INCREMENT);
  Serial.print(F(", down_delay="));
  Serial.println(down_delay);

  Serial.println(F("done"));

}

/* This loop will create a Knight Rider-like effect if you have LEDs plugged
   into all the TLC outputs.  NUM_TLCS is defined in "tlc_config.h" in the
   library folder.  After editing tlc_config.h for your setup, delete the
   Tlc5940.o file to save the changes. */
   


void do_dimm(int increment, unsigned int tlc_delay) {
  
  curr_dimm_level += increment;

  if (curr_dimm_level < MIN_DIMM) curr_dimm_level = MIN_DIMM;
  if (curr_dimm_level > MAX_DIMM) curr_dimm_level = MAX_DIMM;

  Serial.print(F("do_dimm: curr_dimm_level="));
  Serial.print(curr_dimm_level);
  Serial.print(F(", increment="));
  Serial.print(increment);
  Serial.print(F(", tlc_delay="));
  Serial.println(tlc_delay);

  for (int i=0; i<NUM_PINS; i++)
  {
    Tlc.set(i, TLC_MAX - curr_dimm_level);
    delay(1);
  }
  Tlc.update();
  delay(1);
  Tlc.update();
  delay(tlc_delay);

}

void loop()
{    
    unsigned is_dancing;

    is_dancing = digitalRead(INPUT_PIN) == 1;
//    Serial.print(F("is_dancing="));
//    Serial.println(is_dancing);
    

    if (!is_dancing) {
      // DIMM_DOWN
      if (curr_dimm_level == MAX_DIMM) {
        Serial.println("DIMM_DOWN starting...");
      }
      
      if (curr_dimm_level != MIN_DIMM) {
        
        do_dimm(DIMM_DOWN_INCREMENT, down_delay); 
        
        if (curr_dimm_level == MIN_DIMM) {
          Serial.print("DIMM_DOWN complete");       
        }
      }  
    } else {
      // DIMM_UP
      if (curr_dimm_level == MIN_DIMM) {
        Serial.println("DIMM_UP starting");
      }
      // dimm up
      if (curr_dimm_level != MAX_DIMM) {
        do_dimm(DIMM_UP_INCREMENT, up_delay);

        if (curr_dimm_level == MAX_DIMM) {
          Serial.print(F("DIMM_UP complete"));
      } 
    }
  }
}


