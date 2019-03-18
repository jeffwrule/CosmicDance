/*
    Basic Pin setup:
    ------------                                  ---u----
    ARDUINO   13|-> SCLK (pin 25)           OUT1 |1     28| OUT channel 0
              12|                           OUT2 |2     27|-> GND (VPRG)
              11|-> SIN (pin 26)            OUT3 |3     26|-> SIN (pin 11)
              10|-> BLANK (pin 23)          OUT4 |4     25|-> SCLK (pin 13)
               9|-> XLAT (pin 24)             .  |5     24|-> XLAT (pin 9)
               8|                             .  |6     23|-> BLANK (pin 10)
               7|                             .  |7     22|-> GND
               6|                             .  |8     21|-> VCC (+5V)
               5|                             .  |9     20|-> 2K Resistor -> GND
               4|                             .  |10    19|-> +5V (DCPRG)
               3|-> GSCLK (pin 18)            .  |11    18|-> GSCLK (pin 3)
               2|                             .  |12    17|-> SOUT
               1|                             .  |13    16|-> XERR
               0|                           OUT14|14    15| OUT channel 15
    ------------                                  --------

    -  Put the longer leg (anode) of the LEDs in the +5V and the shorter leg
         (cathode) in OUT(0-15).
    -  +5V from Arduino -> TLC pin 21 and 19     (VCC and DCPRG)
    -  GND from Arduino -> TLC pin 22 and 27     (GND and VPRG)
    -  digital 3        -> TLC pin 18            (GSCLK)
    -  digital 9        -> TLC pin 24            (XLAT)
    -  digital 10       -> TLC pin 23            (BLANK)
    -  digital 11       -> TLC pin 26            (SIN)
    -  digital 13       -> TLC pin 25            (SCLK)
    -  The 2K resistor between TLC pin 20 and GND will let ~20mA through each
       LED.  To be precise, it's I = 39.06 / R (in ohms).  This doesn't depend
       on the LED driving voltage.
    - (Optional): put a pull-up resistor (~10k) between +5V and BLANK so that
                  all the LEDs will turn off when the Arduino is reset.

    If you are daisy-chaining more than one TLC, connect the SOUT of the first
    TLC to the SIN of the next.  All the other pins should just be connected
    together:
        BLANK on Arduino -> BLANK of TLC1 -> BLANK of TLC2 -> ...
        XLAT on Arduino  -> XLAT of TLC1  -> XLAT of TLC2  -> ...
    The one exception is that each TLC needs it's own resistor between pin 20
    and GND.

    This library uses the PWM output ability of digital pins 3, 9, 10, and 11.
    Do not use analogWrite(...) on these pins.

    This sketch does the Knight Rider strobe across a line of LEDs.

    Alex Leone <acleone ~AT~ gmail.com>, 2009-02-03 */

#include "SparkFun_Tlc5940.h"

#define GENERIC_DELAY 20000
#define SERIAL_SPEED 25000
#define TLC_DELAY 20
#define PRINT_SKIP 200

int increment=-1;
unsigned int level = 4095;
int channel = 0;

void setup()
{

  // Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.begin(115200);          // setup the interal serial port for debug messages
  Serial.println(F("\nStart setup"));
  Serial.println(F("HighpowerLEDDimm.ino"));
  Serial.print(F("Channel: "));
  Serial.println(channel);

  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init(4095);
  Tlc.set(channel, 4095);
  Tlc.set(channel+1, 4095);
  Tlc.set(channel+2, 4095);
  // Tlc.clear();
  Tlc.update();
  Serial.print(F("Starting delay..."));
  Serial.println(F("done"));

}

/* This loop will create a Knight Rider-like effect if you have LEDs plugged
   into all the TLC outputs.  NUM_TLCS is defined in "tlc_config.h" in the
   library folder.  After editing tlc_config.h for your setup, delete the
   Tlc5940.o file to save the changes. */
   


void loop()
{    
    unsigned long cur_millis;
    unsigned long start_millis;
    
    // catch rollover of unsigned int
    if (increment < 0 && level < abs(increment)) {
      level = 0;
    } else {
      level += increment;
    }
    
    if (level <= 0) { 
        increment = -1 * increment; 
        level=0; 
        start_millis = millis(); 
        }
    if (level >= 4095) { 
        increment = -1 * increment; 
        level=4095;  
        start_millis = millis(); 
        }

    cur_millis = millis();


    // level = 0;
    // level = 4094;


//    if (level % PRINT_SKIP == 0) {
//      Serial.print(F("increment: "));
//      Serial.print(increment);
//      Serial.print(F(", level: "));
//      Serial.print(level);
//      Serial.print(F(" series_time: "));
//      Serial.println((cur_millis - start_millis) / 60000.0);
//    }

    Tlc.set(channel, level);
    Tlc.set(channel+1, level);
    Tlc.set(channel+2, level);
    
    /* Tlc.update() sends the data to the TLCs.  This is when the LEDs will
       actually change. */
    Tlc.update();
    Tlc.update(); /// just gonna do it twice a few times I see it not quite go off.

    delay(TLC_DELAY);

    if (level == 0 || level == 4095) {
      if (level == 0) {
//        Serial.print(F("Max bright reached pausing..."));
      } else {
//        Serial.print(F("Min (off) bright reached pausing..."));
      }
      delay(GENERIC_DELAY);
//      Serial.println(F("continue!"));
      start_millis = millis();      
    }
}


