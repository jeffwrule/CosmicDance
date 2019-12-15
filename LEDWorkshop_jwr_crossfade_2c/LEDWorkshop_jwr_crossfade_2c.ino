#include <FastLED.h>

#define LED_PIN     10
#define NUM_LEDS    60
#define BRIGHTNESS  96
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];



// This example shows how to cross-fade between different color palettes
// using the function nblendPaletteTowardPalette.
//
// The basic idea is that you always have a "current palette" that you're
// pulling colors from with ColorFromPalette, and you have a "target palette"
// which is the 'next' palette that you want to get to.
// 
// After that, implementation is relatively simple: just periodically call
//   nblendPaletteTowardPalette( currentPalette, targetPalette);
// If the current palette is not yet equal to the target palette, this 
// function will make a few small changes to the current palette to make
// it slightly more like the target.  Over time, the current palette will
// come to be equal to the target.
// There's no need to test the current and target for equality; it's safe
// to keep calling nblendPaletteTowardPalette even after current reaches target.
// For faster blending, call nblendPaletteTowardPalette twice per loop.


// Crossfade current palette slowly toward the target palette
//
// Each time that nblendPaletteTowardPalette is called, small changes
// are made to currentPalette to bring it closer to matching targetPalette.
// You can control how many changes are made in each call:
//   - the default of 24 is a good balance
//   - meaningful values are 1-48.  1=veeeeeeeery slow, 48=quickest
//   - "0" means do not change the currentPalette at all; freeze
#define MAX_CHANGES 34
// wait this long before the next color change
#define HOLD_COLOR_MS 10000

#define NUM_COLORS 4
// CRGB colors[NUM_COLORS] = {CRGB(CRGB::Grey), CRGB(CRGB::SkyBlue), CRGB(CRGB::SeaGreen), CRGB(CRGB::DarkGrey)};
CRGB colors[NUM_COLORS] = { CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::DarkGrey};

// how many colors will you fade between
#define NUM_PALETTES 4
// arrage the palettes in order 
CRGBPalette16 palette_list[NUM_PALETTES];

// how long to delay between light adjustments
#define CHANGE_DELAY_MS 30
#define UPDATES_PER_SECOND 50

// set the initial and target color
CRGBPalette16 currentPalette; // ( CRGB::Black);
CRGBPalette16 targetPalette; // ( CRGB::SeaGreen);

unsigned long color_count=0;
bool target_reached=false;
unsigned long color_change_start_ms=0;

void setup() {
  Serial.begin(250000);
  delay( 500 ); // power-up safety delay
  Serial.println();
  Serial.println("Start setup");
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  FastLED.delay(1000);
  for (uint8_t i=0; i<NUM_PALETTES; i++) { palette_list[i] =  CRGBPalette16(colors[i]); }
  ChangeColorOverTime(0);
  Serial.println(F("Setup complete"));
  Serial.println();
}


void loop()
{
  static uint8_t startIndex = 0;

  ChangeColorOverTime(HOLD_COLOR_MS);

  EVERY_N_MILLISECONDS(  1000 / UPDATES_PER_SECOND ) {
    nblendPaletteTowardPalette( currentPalette, targetPalette, MAX_CHANGES);
    FillLEDsFromPaletteColors( startIndex); 
    if (target_reached || (leds[0].r == targetPalette[0].r && leds[0].g == targetPalette[0].g && leds[0].b == targetPalette[0].b)) {
      if (target_reached == false) {
        Serial.print(F("%%% TARGET REACHED : "));
        print_color_control();
        Serial.print(F(", target_reached: "));
        Serial.print(target_reached);
        Serial.print(F(", color_count: "));
        Serial.print(color_count);
        Serial.print(F(", "));
        Serial.println(millis() - color_change_start_ms); 
        Serial.println();
        target_reached = true;
      }
    } else {
      color_count += 1;      
    }
  }
  
  // startIndex = startIndex + 1; /* motion speed */
  FastLED.show();
  // FastLED.delay(30);
}

void printCRGB(CRGB input) {
  Serial.print("0x");
  for (int i=0; i<3; i++){
    char tmp[3];
    sprintf(tmp, "%02X", input[i]);
    Serial.print(tmp);
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 0; i < NUM_LEDS; i++, colorIndex++) {
    leds[i] = ColorFromPalette( currentPalette, i, brightness);
  }
}

void print_color_control() {
  Serial.print(F("Curr Colors led: "));
  printCRGB(leds[0]);
  Serial.print(F(", curr: "));
  printCRGB(currentPalette[0]);
  Serial.print(F(", target: "));
  printCRGB(targetPalette[0]);
}

void ChangeColorOverTime(unsigned long delay_ms) {
  
    static uint8_t palette_index=0;   // index into our list of ordered colors to change to
    static unsigned long last_ms=0;   // milliseconds the last time we switched
    unsigned long cur_ms = millis();  // current milliseconds

    if (cur_ms < last_ms) last_ms=0;

    if (cur_ms - last_ms >= delay_ms) {
      // currentPalette = targetPalette;
      palette_index += 1;
      palette_index = palette_index < NUM_PALETTES ? palette_index : 0;
      if (palette_index >= NUM_PALETTES) {
        palette_index = 0;
      }
      targetPalette = palette_list[palette_index];
      last_ms=cur_ms;
      color_count=0;
      target_reached=false;
      color_change_start_ms = cur_ms;
      Serial.print(F("%%%% Color Change: "));
      Serial.print(palette_index);
      Serial.print(F(", "));
      print_color_control();
      Serial.print(F(", target_reached: "));
      Serial.print(target_reached);
      Serial.println();    
     }
}

//void ChangePalettePeriodically()
//{
//  uint8_t secondHand = (millis() / 1000) % 60;
//  static uint8_t lastSecond = 99;
//
//  
//  if( lastSecond != secondHand) {
//    lastSecond = secondHand;
//    CRGB p = CHSV( HUE_PURPLE, 255, 255);
//    CRGB g = CHSV( HUE_GREEN, 255, 255);
//    CRGB b = CRGB::Black;
//    CRGB w = CRGB::White;
//    if( secondHand ==  0)  { targetPalette = RainbowColors_p; }
//    if( secondHand == 10)  { targetPalette = CRGBPalette16( g,g,b,b, p,p,b,b, g,g,b,b, p,p,b,b); }
//    if( secondHand == 20)  { targetPalette = CRGBPalette16( b,b,b,w, b,b,b,w, b,b,b,w, b,b,b,w); }
//    if( secondHand == 30)  { targetPalette = LavaColors_p; }
//    if( secondHand == 40)  { targetPalette = CloudColors_p; }
//    if( secondHand == 50)  { targetPalette = PartyColors_p; }
//  }
//}
