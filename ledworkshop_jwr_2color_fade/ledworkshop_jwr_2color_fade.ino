#include <FastLED.h>

// fadeTowardColor example code.
//
// Sample code that includes a function for fading one RGB color toward a target RGB color
// Also includes a function for fading a whole array of pixels toward a given color
//
// Both of these functions _modify_ the existing color, in place.
//
// All fades are done in RGB color space.
//
// Mark Kriegsman
// December 2016


#define NUM_LEDS 60
#define LED_PIN 10
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define NUM_COLORS 2
CRGB colors[NUM_COLORS] = { CRGB::BlueViolet, CRGB::DarkRed };
uint8_t to_color=0;

void setup() {
  delay(3000); // sanity delay
  Serial.begin(9600);
  FastLED.addLeds<LED_TYPE,LED_PIN, COLOR_ORDER>(leds,NUM_LEDS);
  FastLED.setBrightness(180);
  to_color=1;
  fill_solid(leds, NUM_LEDS, colors[0]);
  FastLED.show();
}



// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
  for( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
  }
}


void loop() 
{
  // CRGB bgColor( 0, 15, 2); // pine green ?
  
  // fade all existing pixels toward bgColor by "5" (out of 255)
  fadeTowardColor( leds, NUM_LEDS, colors[to_color], 10);


  // periodically set random pixel to a random color, to show the fading
  EVERY_N_MILLISECONDS( 2000 ) {
    if (leds[0] == colors[to_color]) {
      Serial.println("Color Switch");
      to_color += 1;
      if (to_color >= NUM_COLORS) to_color=0;
      Serial.print(to_color);
      Serial.print(", ");
      Serial.print(colors[to_color].r);
      Serial.print(", ");
      Serial.print(colors[to_color].g);
      Serial.print(", ");
      Serial.println(colors[to_color].b);
    }
  }

  FastLED.show();
  FastLED.delay(10);
}
