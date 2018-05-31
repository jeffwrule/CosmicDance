#ifndef AAB_ANTIMATTER_H
#define AAB_ANTIMATTER_H

#include "AAB_antimatter_config.h"
#include "PrintTimer.h"
#include "Nucleon.h"

PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

Nucleon *my_nucleon;

#include <FAB_LED.h>

//// Declare the LED protocol and the port
sk6812b<D,6>  quark1;
sk6812b<D,7>  quark2;
sk6812b<B,0>  quark3;

const uint8_t numPixels = QUARK_NUM_PIXELS;

grbw  pixels[QUARK_NUM_PIXELS] = {};

void setup() {  
  
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Setup Begin Antimatter"));
  delay(500);

  print_timer->update();
  print_timer->print_now();
  
  my_nucleon = new Nucleon(print_timer, yellow, cyan, magenta, "YELLOW", "CYAN", "MAGENTA"); 

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
//    pixels[i].r = 255;
//    pixels[i].g = 255;
//    pixels[i].b = 255; 
    pixels[i].w = 0; 
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
