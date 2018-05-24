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

//unsigned int BOX1_R = 0;
//unsigned int BOX1_G = 1;
//unsigned int BOX1_B = 2;

#define NUM_BOXES 3  // number of boxes we are controling
//#define SMD_LED_COUNT 3   // number of LED in each SMD package RGB 3 or RGBW 4
//#define NUM_CHANNELS  NUM_BOXES * SMD_LED_COUNT   // total channels of lights to control

#define MAX_GRAY  4095


void setup()
{
  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init(MAX_GRAY);
  //setColourRgb(0,0,0);
  Serial.begin(250000);

  
}

void loop()
{
  setAllRgb(MAX_GRAY, 0, 0);  // red
  Serial.println(F("All RED"));
  //Tlc.update();
  delay(3000);

  setAllRgb(0, MAX_GRAY, 0);  // geeen
  Serial.println(F("All GREEN"));
  //Tlc.update();
  delay(3000);

  setAllRgb(0, 0, MAX_GRAY);  // blue
  Serial.println(F("All BLUE"));
  //Tlc.update();
  delay(3000);  
  
  setAllRgb(MAX_GRAY, MAX_GRAY, MAX_GRAY);  // white
  Serial.println(F("All WHITE"));
  //Tlc.update();
  delay(3000);

}



// set the given color for a box
void setBoxColourRgb(unsigned int box, unsigned int red, unsigned int green, unsigned int blue) {
  // analogWrite(redPin, red);
  // analogWrite(greenPin, green);
  // analogWrite(bluePin, blue);
  unsigned int channel = box * 3;
  
  Serial.print(F("Box="));
  Serial.print(box);  
  Serial.print(F(", C="));
  Serial.print(channel);
  Serial.print(F(", R="));
  Serial.print(red);
  Serial.print(F(", G="));
  Serial.print(green);
  Serial.print(F(", B="));
  Serial.println(blue);
  Tlc.set(channel, MAX_GRAY - red);
  //delay(10);
  Tlc.set(channel+1, MAX_GRAY - green);
  //delay(10);
  Tlc.set(channel+2, MAX_GRAY - blue);
  //delay(10);
  Tlc.update();
 }

// set the given color of all boxes
void setAllRgb(unsigned int r, unsigned int g, unsigned int b) {
  for (int i=0; i < NUM_BOXES; i++) {
    setBoxColourRgb(i, r, g, b);
  }
}

