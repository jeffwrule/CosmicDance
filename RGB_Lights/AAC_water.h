#include <FAB_LED.h>

#ifndef AAC_WATER_H
#define AAC_WATER_H

#include "AAC_water_config.h"
#include "PrintTimer.h"
#include "Nucleon.h"

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

void setup() {  
  
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Setup Begin Antimatter"));
  delay(500);

  print_timer->update();
  print_timer->print_now();

#ifdef SK6812B_LED
  my_nucleon = new Nucleon(print_timer, yellow, cyan, magenta, "YELLOW", "CYAN", "MAGENTA"); 
#endif

#ifdef WS2812B_LED
  my_nucleon = new Nucleon(print_timer, red, green, blue, "RED", "GREEN", "BLUE"); 
#endif


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


#endif
