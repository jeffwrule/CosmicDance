#ifndef AAA_MATTER_H
#define AAA_MATTER_H

#include "SparkFun_Tlc5940.h"
#include "PrintTimer.h"
#include "AAA_matter_config.h"
#include "Nucleon.h"

PrintTimer *print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

Nucleon *my_nucleon;

void setup() {
  
  Tlc.init(MAX_GRAY);
  
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Setup Begin Matter"));

  print_timer->update();
  print_timer->print_now();
  
  my_nucleon = new Nucleon(print_timer, red, green, blue, "RED", "GREEN", "BLUE"); 

  Serial.println(F("Setup End"));

}


// set the given color for a box
void Nucleon::setQuarkColourRgb(PrintTimer *pt, unsigned int quark) {

  unsigned int channel = quark * 3;

  if (pt->get_do_print() && IS_VERBOSE) {
    Serial.print(F("quark="));
    Serial.print(quark);  
    Serial.print(F(", C_R="));
    Serial.print(channel);
    Serial.print(F(", C_G="));
    Serial.print(channel+1);
    Serial.print(F(", C_B="));
    Serial.print(channel+2);
    Serial.print(F(", R="));
    Serial.print(quark_list[quark].current_color.r);
    Serial.print(F(", G="));
    Serial.print(quark_list[quark].current_color.g);
    Serial.print(F(", B="));
    Serial.println(quark_list[quark].current_color.b);    
  }

  Tlc.set(channel, MAX_GRAY - quark_list[quark].current_color.r);
  Tlc.set(channel+1, MAX_GRAY - quark_list[quark].current_color.g);
  Tlc.set(channel+2, MAX_GRAY - quark_list[quark].current_color.b);
  Tlc.update();
  delay(LED_UPDATE_DELAY);
 }

#endif
