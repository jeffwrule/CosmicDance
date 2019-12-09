#include <FastLED.h>
#define NUM_LEDS 60
#define DATA_PIN 10

CRGB leds[NUM_LEDS];


void setup() { 
     FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
     Serial.begin(9600);
     pinMode(A0,INPUT);
 }

 void loop() {
        int val = analogRead(A0);
        Serial.println(val);
        int numLedsToLight = constrain(map(val, 60, 970, 0, NUM_LEDS), 0, NUM_LEDS);
        Serial.println(numLedsToLight);

        // First, clear the existing led values
        FastLED.clear();
        for(int led = 0; led < numLedsToLight; led++) { 
            leds[led] = CRGB::Blue; 
        }
        FastLED.show();
    }
