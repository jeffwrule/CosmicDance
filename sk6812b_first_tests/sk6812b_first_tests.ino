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
sk6812b<D,6>  strip3;
sk6812b<D,7>  strip2;
sk6812b<B,0>  strip;

// How many pixels to control
const uint8_t numPixels = 36;

// How bright the LEDs will be (max 255)
const uint8_t maxBrightness = 255;
const uint8_t minBrightness = 15;
const uint8_t halfBrightness = 127;


// The pixel array to display
//grbw  pixels[numPixels] = {};
grbw  pixels[numPixels] = {};


////////////////////////////////////////////////////////////////////////////////
// Sets the array to specified color
////////////////////////////////////////////////////////////////////////////////
void updateColors(char r, char g, char b, char w)
{
  for(int i = 0; i < numPixels; i++)
  {
    pixels[i].r = r;
    pixels[i].g = g;
    pixels[i].b = b;
    pixels[i].w = w;
  }
}

void dimmColors(unsigned char r, unsigned char g, unsigned char b, unsigned char w)
{
  unsigned char r_level = 0;
  unsigned char g_level = 0;
  unsigned char b_level = 0;
  unsigned char w_level = 0;
  unsigned max_bright = 0;

  max_bright = max(max_bright, r);
  max_bright = max(max_bright, g);
  max_bright = max(max_bright, b);
  max_bright = max(max_bright, w);
//  Serial.print(F("max_bright="));
//  Serial.println(int(max_bright));
  for (int i=0; i<=max_bright; i++) {
//    Serial.println(F("i="));
//    Serial.print(i);
//    Serial.print(F("r_level="));
//    Serial.println(r_level);
    if (r_level < r) r_level++; 
    if (g_level < g) g_level++; 
    if (b_level < b) b_level++; 
    if (w_level < w) w_level++;
    updateColors(r_level, g_level, b_level, w_level);
    strip.sendPixels(numPixels, pixels);
    delay(10);
  }

  for (int i=max_bright; i>=0; i--) {
    if (r_level > 0) r_level--; 
    if (g_level > 0) g_level--; 
    if (b_level > 0) b_level--; 
    if (w_level > 0) w_level--;
    updateColors(r_level, g_level, b_level, w_level);
    strip.sendPixels(numPixels, pixels);
    delay(10);
  }
}

void transitionColors(unsigned char r_target, unsigned char g_target, unsigned char b_target, unsigned char w_target) {
  /*
   * Current values for the this pixel string
   */
  unsigned char r_level = pixels[0].r;    
  unsigned char g_level = pixels[0].g;
  unsigned char b_level = pixels[0].b;
  unsigned char w_level = pixels[0].w;
    
  unsigned long delay_amount=50;

  for (int i=0; i<=255; i++) {
    boolean values_changed=false;
    // check for descending values
    if (r_level > r_target) { r_level--; values_changed=true;}
    if (g_level > g_target) { g_level--; values_changed=true;}
    if (b_level > b_target) { b_level--; values_changed=true;}
    if (w_level > w_target) { w_level--; values_changed=true;}
    // check for ascending values 
    if (r_level < r_target) { r_level++; values_changed=true;}
    if (g_level < g_target) { g_level++; values_changed=true;}
    if (b_level < b_target) { b_level++; values_changed=true;}
    if (w_level < w_target) { w_level++; values_changed=true;}
    if (values_changed) {
       // if something changed push it out
       Serial.print(F("RGBW values=("));
       Serial.print(r_level);
       Serial.print(F(", "));
       Serial.print(g_level);
       Serial.print(F(", "));
       Serial.print(b_level);
       Serial.print(F(", "));
       Serial.println(w_level);
      updateColors(r_level, g_level, b_level, w_level);
      strip.sendPixels(numPixels, pixels);
      delay(delay_amount);      
    }
  }
   Serial.println(F("----- transition complete -----"));
}

void setAllColor(unsigned int r, unsigned int g, unsigned int b, unsigned int w) {
  for (int i=0;  i < numPixels; i++) {
    pixels[i].r = r;
    pixels[i].g = g;
    pixels[i].b = b;
    pixels[i].w = w;
  }
  strip.sendPixels(numPixels, pixels);
  strip2.sendPixels(numPixels, pixels);
  strip3.sendPixels(numPixels, pixels);
}

////////////////////////////////////////////////////////////////////////////////
// This method is automatically called once when the board boots.
////////////////////////////////////////////////////////////////////////////////
void setup()
{
  // Turn off the LEDs
  strip.clear(2 * numPixels);
  strip2.clear(2 * numPixels);
  strip3.clear(2 * numPixels);
  Serial.begin(250000);
  delay(1000);
  Serial.println(F("Setup Complete"));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief This method is automatically called repeatedly after setup() has run.
/// It is the main loop.
////////////////////////////////////////////////////////////////////////////////
//void loop()
//{
//
//  // transition red
//  transitionColors(halfBrightness, 0, 0, halfBrightness);
//  transitionColors(maxBrightness, 0 , 0, 0);
//  delay(1000);
//
//  // transition green 
//  transitionColors(halfBrightness, halfBrightness, 0, halfBrightness);
//  transitionColors(0, maxBrightness, 0, 0);
//  delay(1000);
//  
//
//  // transition blue
//  transitionColors(0, halfBrightness, halfBrightness, halfBrightness);
//  transitionColors(0, 0, maxBrightness, 0);
//  delay(1000);
//  
//  // Transition the pixel array white
//
//  transitionColors( halfBrightness, halfBrightness, halfBrightness, halfBrightness);
//  transitionColors( maxBrightness, maxBrightness, maxBrightness, 0);
//
//  // Turn off the LEDs
//  // strip.clear(numPixels);
//  delay(2000);
//
//
//}

void loop()
{
//  setAllColor(maxBrightness, 0, 0, 0);
//  Serial.println(F("RED"));
//  delay(3000);
//
//  setAllColor(0, maxBrightness, 0, 0);
//  Serial.println(F("GREEN"));
//  delay(3000);
//
//   setAllColor(0, 0, maxBrightness, 0);
//  Serial.println(F("BLUE"));
//  delay(3000);

  setAllColor(maxBrightness, maxBrightness, maxBrightness, maxBrightness);
  Serial.println(F("WHITE"));
  delay(3000);
  
}

