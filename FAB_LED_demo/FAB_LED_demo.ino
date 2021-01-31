#include <FAB_LED.h>
const uint16_t numPixels = 16;
ws2812b<D,6> myLeds;

#define   RED(x, i) x[i]
#define GREEN(x, i) x[i+1]
#define  BLUE(x, i) x[i+2]

void colorN(uint8_t red, uint8_t green, uint8_t blue)
{
  // We multiply by 3 because A pixel is 3 bytes, {G,R,B}
  uint8_t array[3*numPixels] = {};

  // Set each set of 3 bytes for every pixel
  for (uint16_t i = 0; i < numPixels; i++) {
      RED(array, i) = red;
    GREEN(array, i) = green;
     BLUE(array, i) = blue;
  }

  myLeds.sendPixels(numPixels, array);
}
