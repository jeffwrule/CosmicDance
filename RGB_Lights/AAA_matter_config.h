#ifndef MATTER_CONFIG_H
#define MATTER_CONFIG_H

#define SMD_LED_COUNT 3   // number of LED in each SMD package RGB 3 or RGBW 4

#define MAX_GRAY  4095
#define WEAK_GRAY 500

#define NUM_FIBONOCCI 11
unsigned int fibonicci_sequence[NUM_FIBONOCCI] = { 34000, 21000, 13000, 8000, 5000, 3000, 2000, 1000, 1000, 1000, 2000 };

#define LED_UPDATE_DELAY 1

#endif
