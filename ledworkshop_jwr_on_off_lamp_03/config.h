#ifndef CONFIG_H
#define CONFIG_H

#define SKETCH_ID "03"


#define IR_PIN 11

#define LED_PIN     10
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define NUM_LEDS    90

#define BRIGHTNESS  200
#define FRAMES_PER_SECOND 60

bool gReverseDirection = false;

CRGB leds[NUM_LEDS];

// SOUND CONTROL PINS AND VOLUME
#define DFMINI_RX 5
#define DFMINI_TX 6
#define DFMINI_VOLUME 20

#define HAS_MUSIC true


// how long to delay between light adjustments
#define CHANGE_DELAY_MS 30
#define UPDATES_PER_SECOND 10

#endif
