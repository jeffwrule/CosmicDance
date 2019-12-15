#ifndef CONFIG_H
#define CONFIG_H

#define SKETCH_ID "00"

#define IR_PIN 11

// LED PINS AND SIZE AND TYPE
#define LED_PIN     10
#define NUM_LEDS    60
#define BRIGHTNESS  96
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// SOUND CONTROL PINS AND VOLUME
#define DFMINI_RX 5
#define DFMINI_TX 6
#define DFMINI_VOLUME 20

#define HAS_MUSIC true


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
const char *color_names[NUM_COLORS] = { "Red", "Green", "Blue", "DarkGrey" };

// how many colors will you fade between
#define NUM_PALETTES 4
// arrage the palettes in order 
CRGBPalette16 palette_list[NUM_PALETTES];

// how long to delay between light adjustments
#define CHANGE_DELAY_MS 30
#define UPDATES_PER_SECOND 50

#endif
