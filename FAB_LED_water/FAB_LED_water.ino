////////////////////////////////////////////////
//
//  Water has a set of 88 WS2818B LEDS that we program in mostly blue with some blinking white randomly sprinkled through out.  It should not change that often...
//
//  This was for the Taipei 2021 Winter show
//
////////////////////////////////////////////////
#include <FAB_LED.h>

// Declare the LED protocol and the port
ws2812b<D,6>  strip;

// How many pixels to control
const uint8_t numPixels = 100;

// How bright the LEDs will be (max 255)
// const uint8_t maxBrightness = 16;

// The pixel array to display
grb       pixels[numPixels] = {};
grb       target[numPixels] = {};
uint32_t  keep_until[numPixels] = {};

grb  blue = {0,0,255};
grb  white = {255,255,255};

#define NUM_COLORS 6
grb  colors[NUM_COLORS] = {white, blue, blue, blue, blue, blue};   /* we only want at most 25 percent white, so random should it blue 3 out of 4 chances */

#define MAX_KEEP_TIME_MS 20000
uint32_t cur_time_ms;

#define STATUS_WAIT_MS 2000
uint32_t  last_status=0;

#define RANDOM_WAIT 100
uint32_t last_random=0;



void status(boolean always_print = false) {
  if (always_print == false) {
    if (last_status + STATUS_WAIT_MS >= cur_time_ms) return;  
    }
  last_status = cur_time_ms;
  
  Serial.print(F("LED Wait Times: cur_time_ms="));
  Serial.print(cur_time_ms);
  Serial.print(F(" :: "));
  
  for (int i=0; i<numPixels; i++) {
    grb p = pixels[i];
//    if (i != 0) Serial.print(", ");
//    Serial.print(i);
    if (p.r == blue.r && p.g == blue.g && p.b == blue.b) {
      Serial.print(F("B"));
    } else if (p.r == white.r && p.g == white.g && p.b == white.b) {
      Serial.print(F("W"));
    } else {
      Serial.print(F("?"));
    }
//    Serial.print(keep_until[i]);
//    Serial.print(F(":"));
//    if (cur_time_ms > keep_until[i]) {
//      Serial.print(F("expired"));
//    } else {
//      Serial.print(keep_until[i] - cur_time_ms);    
//    }
  }
  Serial.println();
}

////////////////////////////////////////////////////////////////////////////////
// Sets the array to specified color
////////////////////////////////////////////////////////////////////////////////
boolean assign_random_color()
{
  boolean found_change=false;

  if (cur_time_ms < last_random) return;
  
  for(int i=0; i < numPixels; i++) {
    if (cur_time_ms > keep_until[i]) {
      pixels[i] = colors[random(NUM_COLORS)];
      keep_until[i] = cur_time_ms + random(MAX_KEEP_TIME_MS);
      found_change=true;
    }
  }
  return found_change;
}


////////////////////////////////////////////////////////////////////////////////
// This method is automatically called once when the board boots.
////////////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(250000);
  cur_time_ms = millis();
  for (int i=0; i<numPixels; i++) {
    keep_until[i] = cur_time_ms + random(MAX_KEEP_TIME_MS);
    pixels[i] = colors[random(NUM_COLORS)];
  }
  strip.sendPixels(numPixels, pixels);
  status(true);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief This method is automatically called repeatedly after setup() has run.
/// It is the main loop.
////////////////////////////////////////////////////////////////////////////////
void loop()
{  
  boolean changed;  

  /* check for millis wraparound */
  if (millis() < cur_time_ms) {
    cur_time_ms = millis();
    last_status=0;
    last_random=0;
    for (int i=0; i<numPixels; i++) {
      keep_until[i] = cur_time_ms + random(MAX_KEEP_TIME_MS);
    }
  }

  cur_time_ms = millis();
  changed = assign_random_color();
  if (changed) strip.sendPixels(numPixels, pixels);
  status();
}
