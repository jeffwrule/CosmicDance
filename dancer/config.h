#ifndef CONFIG_H
#define CONFIG_H

// FORWARD DECLARATIONS
String bool_tostr(bool input_bool);

#define SERIAL_SPEED  250000
#define PRINT_AFTER_SECONDS 3    // print every n seconds 

#define IS_BRIEF
// #define IS_CHATTY 

// Volume settings
#define NORMAL_VOLUME 12
#define BEEP_VOLUME 100 

#define FADE_SECONDS 2

// set this to always be dancing
#define FAKE_FOB_IS_DANCING false

// set this true to make the dancer react to the seat switch, not the director or FOB
#define IS_SEAT_SWITCH false            

// configured PINS
#define PRIMARY_DEVICE_PIN A0         // pin to turn on the primary motor/device
#define SECONDARY_DEVICE_PIN A1       // pin to turn on secondary motor/devce  
#define FOB_PIN   A3                  // Fob INPUT_PULLUP Pin
#define SEAT_PIN A5                   // input pin, used by IS_SEAT_SWITCH=true mode

// XBEE Constants
#define XBEE_READ_PIN   5   
#define XBEE_WRITE_PIN  10

// MP3 track names
#define SOLO_TRACK_NAME   "solo.mp3"        //  const char *solo_track_name       Name of the solo track on the microSD card
#define ENSEMBL_TRACK_NAME "ensemble.mp3"   //  const char *ensembl_track_name    Name of the ensemble track on the microSD card

// Dancer Delay Configs
#define PRE_DELAY_SECNDS  0   //  unsigned int dance_delay_seconds  number of seconds to delay between director/fob start and music start
#define FOB_DELAY_SECONDS 8   //  unsigned int fob_delay_seconds    number of seconds to delay between plays when in fob mode

#define STOP_BEFORE_FOB_DELAY false   //  when using FOB_DELAY_SECONDS, do we stop with the song ends or when the delay ends. (false was old behavior)

#define CHECK_IN_SECONDS 4    // how often to check in with the director when active (not on fob)

#endif
