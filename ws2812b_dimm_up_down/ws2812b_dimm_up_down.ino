////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Fast Adressable Bitbang LED Library
/// Copyright (c)2015, 2017 Dan Truong
///
/// This is the simplest exmple to use the library.
///
/// This example is for an Arduino Uno board with a LED strip connected to
/// port D6. Targetting any other board requires you to change something.
/// The program sends an array of pixels to display on the strip.
/// "strip" represents the hardware: LED types and port configuration,
/// "pixels" represents the data sent to the LEDs: a series of colors.
///
/// Wiring:
///
/// The LED strip DI (data input) line should be on port D6 (Digital pin 6 on
/// Arduino Uno). If you need to change the port, change all declarations below
/// from, for example from "ws2812b<D,6> myWs2812" to "ws2812b<B,4> myWs2812"
/// if you wanted to use port B4.
/// The LED power (GND) and (+5V) should be connected on the Arduino Uno's GND
/// and +5V.
///
/// Visual results:
///
/// If the hardware you use matches this program you will see the LEDs blink
/// repeatedly red, green, blue, white, in that order.
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include <FAB_LED.h>

// Declare the LED protocol and the port
ws2812b<D,6>  strip;

// How many pixels to control
const uint8_t numPixels = 132;

// How bright the LEDs will be (max 255)
// const uint8_t maxBrightness = 16;

// The pixel array to display
grb  pixels[numPixels] = {};


////////////////////////////////////////////////////////////////////////////////
// Sets the array to specified color
////////////////////////////////////////////////////////////////////////////////
void updateColors(char r, char g, char b)
{
  for(int i = 0; i < numPixels; i++)
  {
    pixels[i].r = r;
    pixels[i].g = g;
    pixels[i].b = b;
  }
}




////////////////////////////////////////////////////////////////////////////////
// This method is automatically called once when the board boots.
////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(250000);
  Serial.println(F("Setup Begin 2812B_dimm_up_down"));
  delay(500);
  // Turn off the LEDs
  strip.clear(2 * numPixels);
}

int cur_level=0;          // when dimming this is the current level
int dimm_increment=1;     // either 1 or -1 depending on dimm up or dimm down
  
unsigned long last_ms = 0;            // milliseconds when we last updated 
unsigned int increment_delay_ms=66;  // delay between updates, 80 * 256 will give us approximately 20 seconds up and down
unsigned int off_delay_ms=10000;      // when we are at 0, make a longer delay
unsigned int max_brightness=150;       // approximately 60% bright

void dimm_up_down() 
{
  if (cur_level == max_brightness) dimm_increment=-1;  // moving down
  if (cur_level == 0) dimm_increment=1;     // moving up 
  cur_level += dimm_increment;
  updateColors(cur_level, cur_level, cur_level);
  strip.sendPixels(numPixels, pixels);
  delay(2);
  Serial.print(F("New Dimm Level: "));
  Serial.print(cur_level);
  Serial.print(F(" last_ms: "));
  Serial.println(last_ms);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief This method is automatically called repeatedly after setup() has run.
/// It is the main loop.
////////////////////////////////////////////////////////////////////////////////

void loop()
{

  unsigned long cur_ms = millis();

  if (cur_ms < last_ms) last_ms=0;   // handle loop around of mills

  switch (cur_level) {
    case 0:   if (cur_ms > last_ms + off_delay_ms) {    // we delay much longer when we are off
                  last_ms = cur_ms;
                  dimm_up_down();
                  
              } 
              break;
              
    default:  if (cur_ms > last_ms + increment_delay_ms) {    // standard delay during the rest of the up/down cycle
                  last_ms = cur_ms;
                  dimm_up_down();
              }
              break;
    }
  
//  // Write the pixel array red
//  updateColors(maxBrightness, 0 , 0);
//  // Display the pixels on the LED strip
//  strip.sendPixels(numPixels, pixels);
//  // Wait 0.1 seconds
//  delay(100);
//
//  // Write the pixel array green
//  updateColors(0, maxBrightness, 0);
//  // Display the pixels on the LED strip
//  strip.sendPixels(numPixels, pixels);
//  // Wait 0.1 seconds
//  delay(100);
//
//  // Write the pixel array blue
//  updateColors(0, 0, maxBrightness);
//  // Display the pixels on the LED strip
//  strip.sendPixels(numPixels, pixels);
//  // Wait 0.1 seconds
//  delay(100);
//
//  // Write the pixel array white
//  updateColors( maxBrightness, maxBrightness, maxBrightness);
//  // Display the pixels on the LED strip
//  strip.sendPixels(numPixels, pixels);
//  // Wait 0.1 seconds
//  delay(100);
//
//  // Turn off the LEDs
//  strip.clear(numPixels);
//  delay(600);
}
