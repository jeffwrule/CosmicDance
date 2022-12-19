#ifndef AAB_ANTIMATTER_CONFIG_H
#define AAB_ANTIMATTER_CONFIG_H

// there are two types of LED's in these scupptures
// SK6812b RGBW (SMD 5050) and WS8212b LEDs RGB (SMD 5050)
// make sure one one of these are defined. 
//#define SK6812B_LED     # the three original scupltures antimatter (CYM) would use these LEDs 
#define WS2812B_LED   # the three matter sculptures would use this type of LED

#define SMD_LED_COUNT 3   // number of LED in each SMD package RGB 3 or RGBW 4

#define MAX_GRAY  255
//#define WEAK_GRAY 500

#define NUM_FIBONOCCI 11
unsigned int fibonicci_sequence[NUM_FIBONOCCI] = { 34000, 21000, 13000, 8000, 5000, 3000, 2000, 1000, 1000, 1000, 1000 };

// orig antimater # pixels (reconfigured into time-machine up and time machine down
// #define QUARK_NUM_PIXELS 40

// New value for timeMachineDown (time machine downp)
#define QUARK_NUM_PIXELS 132

#define LED_UPDATE_DELAY 20

#ifdef SK6812B_LED
#define COLOR_FACTOR_COLOR "YELLOW"
#define COLOR_FACTOR 0.7
#endif

#ifdef WS2812B_LED
#define COLOR_FACTOR_COLOR "RED"
#define COLOR_FACTOR 0.7
#endif

//#define FOB_PIN 12
//#define HAS_FOB

#endif
