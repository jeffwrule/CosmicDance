#include <FastLED.h>

// CRGBPalette16 p( CRGB::Red);

DEFINE_GRADIENT_PALETTE( gp ) {
  0,     0,  0,  0,   //black
  255,   0,  255,  0   //red
 // 255,   0,  0,  0   //red
}; // black to red

CRGB my_color;

CRGBPalette16 p = gp;


void PrintHexCRGB(CRGB data) // prints 8-bit data in hex with leading zeroes
{     
      char tmp[5];
      Serial.print("0x");
       for (int i=0; i<3; i++) {
         sprintf(tmp, "%.2X",data[i]);
         Serial.print(tmp); 
       }
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  delay(300);
  Serial.println();
  for (unsigned char i=0; i<255; i++) {
    Serial.print(i);
    Serial.print(", ");
    my_color = ColorFromPalette( p, i);
    PrintHexCRGB(my_color);
    Serial.println();
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
