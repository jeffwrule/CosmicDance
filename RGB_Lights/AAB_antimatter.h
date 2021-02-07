#include <FAB_LED.h>

#ifndef AAB_ANTIMATTER_H
#define AAB_ANTIMATTER_H

#include "AAB_antimatter_config.h"
#include "PrintTimer.h"
#include "Nucleon.h"
#include "Fob.h"
#include "config.h"

PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

Nucleon *my_nucleon;

#include <FAB_LED.h>

const uint8_t numPixels = QUARK_NUM_PIXELS;

//// Declare the LED protocol and the port
#ifdef SK6812B_LED
sk6812b<D,6>  quark1;   // pin D6
sk6812b<D,7>  quark2;   // pin D7
sk6812b<B,0>  quark3;   // pin D8
rgbw  pixels[QUARK_NUM_PIXELS] = {};
#endif

#ifdef WS2812B_LED
ws2812b<D,6>  quark1;   // pin D6
ws2812b<D,7>  quark2;   // pin D7
ws2812b<B,4>  quark3;   // pin D12
rgb  pixels[QUARK_NUM_PIXELS] = {};
#endif


// grbw  pixels[QUARK_NUM_PIXELS] = {};
// rgb  pixels[QUARK_NUM_PIXELS] = {};

Fob *fob;

void updateColors(char r, char g, char b);    // forward decleration


void setup() {  
  
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Setup Begin Antimatter v2 2021-02-07"));
  delay(500);

  print_timer->update();
  print_timer->print_now();

#ifdef SK6812B_LED
  my_nucleon = new Nucleon(print_timer, yellow, cyan, magenta, "YELLOW", "CYAN", "MAGENTA"); 
#endif

#ifdef WS2812B_LED
  my_nucleon = new Nucleon(print_timer, red, green, blue, "RED", "GREEN", "BLUE"); 
#endif

  updateColors(50,50,50);
  quark1.clear(2 * numPixels);
  quark1.clear(2 * numPixels);
  delay(1000);
  quark1.sendPixels(numPixels, pixels);
  delay(1000);
  quark1.clear(2 * numPixels);
  delay(1000);
  quark1.sendPixels(numPixels, pixels);
  delay(1000);
  quark1.clear(2 * numPixels);
  delay(1000);
  
  fob = new Fob(FOB_PIN);

  // Turn on all the LEDs
  delay(10);
  Serial.println(F("Setup Complete"));
}

// set the given color for a box
void Nucleon::setQuarkColourRgb(PrintTimer *pt, unsigned int quark) {

  boolean orig_print = pt->get_do_print();

  pt->print_now();

  if (pt->get_do_print() && IS_VERBOSE) {
    Serial.print(F("setQuarkColourRgb QuarkID="));
    Serial.print(quark);  
    Serial.print(F(", R="));
    Serial.print(quark_list[quark].current_color.r);
    Serial.print(F(", G="));
    Serial.print(quark_list[quark].current_color.g);
    Serial.print(F(", B="));
    Serial.println(quark_list[quark].current_color.b);    
  }

  for (int i=0; i<QUARK_NUM_PIXELS; i++) {
    pixels[i].r = quark_list[quark].current_color.r;
    pixels[i].g = quark_list[quark].current_color.g;
    pixels[i].b = quark_list[quark].current_color.b;
#ifdef SK6812B_LED 
    pixels[i].w = 0; 
#endif
  }

  switch (quark) {
    case 0: quark1.sendPixels(QUARK_NUM_PIXELS, pixels);
            quark1.sendPixels(QUARK_NUM_PIXELS, pixels); 
            break;
    case 1: quark2.sendPixels(QUARK_NUM_PIXELS, pixels);
            quark2.sendPixels(QUARK_NUM_PIXELS, pixels); 
            break;
    case 2: quark3.sendPixels(QUARK_NUM_PIXELS, pixels); 
            quark3.sendPixels(QUARK_NUM_PIXELS, pixels); 
            break;      
  }
  delay(LED_UPDATE_DELAY);

  if (!orig_print) pt->print_off();
  
 }

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//
// New code here to drive pixels when fob is off
//
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


int cur_level=0;                      // when dimming this is the current level
int dimm_increment=1;                 // either 1 or -1 depending on dimm up or dimm down
  
unsigned long last_ms = 0;            // milliseconds when we last updated 
unsigned int off_delay_ms=10000;      // when we are at 0, make a longer delay
// unsigned int max_brightness=150;      // approximately 60% bright
// unsigned int increment_delay_ms=66;   // delay between updates, 80 * 256 will give us approximately 20 seconds up and down
unsigned int max_brightness=30;         // out of 255
unsigned int increment_delay_ms=333;     // should work out to about 10 seconds up and 10 seconds down


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

void dimm_all(PrintTimer *pt) 
{
  if (cur_level == max_brightness) dimm_increment=-1;  // moving down
  if (cur_level == 0) dimm_increment=1;     // moving up 
  cur_level += dimm_increment;
  updateColors(cur_level, cur_level, cur_level);
  quark1.sendPixels(numPixels, pixels);
  delay(2);
  if (cur_level % 10 == 0 || cur_level == 0 or cur_level==max_brightness) {
    Serial.print(F("New Dimm Level: "));
    Serial.print(cur_level);
    Serial.print(F(" last_ms: "));
    Serial.println(last_ms);    
  }


}

////////////////////////////////////////////////////////////////////////////////
/// @brief This method is automatically called repeatedly after setup() has run.
/// It is the main loop.
////////////////////////////////////////////////////////////////////////////////



void do_dimm_up_down(PrintTimer *pt)
{

  unsigned long cur_ms = pt->current_millis;

  if (cur_ms < last_ms) last_ms=0;   // handle loop around of mills

  switch (cur_level) {
    case 0:   if (cur_ms > last_ms + off_delay_ms) {    // we delay much longer when we are off
                  last_ms = cur_ms;
                  dimm_all(pt);
                  
              } 
              break;
              
    default:  if (cur_ms > last_ms + increment_delay_ms) {    // standard delay during the rest of the up/down cycle
                  last_ms = cur_ms;
                  dimm_all(pt);
              }
              break;
    }
}



#endif
