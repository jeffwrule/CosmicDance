#ifndef ADAFRUITMP3_H
#define ADAFRUITMP3_H

#include </Users/jrule/Documents/Arduino/Dancer/GenericMP3.h>

  #ifdef HAS_ADAFRUIT_MP3
    #include <Adafruit_VS1053.h>
    #include <SD.h>
  
    // define the pins used
    #define CLK 13       // SPI Clock, shared with SD card
    #define MISO 12      // Input data, from VS1053/SD card
    #define MOSI 11      // Output data, to VS1053/SD card
    // Connect CLK, MISO and MOSI to hardware SPI pins. 
    // See http://arduino.cc/en/Reference/SPI "Connections"
  
    // These are the pins used for the breakout example
    //#define BREAKOUT_RESET  9      // VS1053 reset pin (output)
    //#define BREAKOUT_CS     10     // VS1053 chip select pin (output)
    //#define BREAKOUT_DCS    8      // VS1053 Data/command select pin (output)
    // These are the pins used for the music maker shield
    #define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
    #define SHIELD_CS     7      // VS1053 chip select pin (output)
    #define SHIELD_DCS    6      // VS1053 Data/command select pin (output)
    
    // These are common pins between breakout and shield
    #define CARDCS 4     // Card chip select pin
    // DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
    #define DREQ 3       // VS1053 Data request, ideally an Interrupt pin
    Adafruit_VS1053_FilePlayer musicPlayer = 
      // create shield-example object!
      Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

setup()
      Serial.println(F("using Adafruit MP3 Card pin setup"));

            if (! musicPlayer.begin()) { // initialise the music player
       Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
       while (1);
    }
    Serial.println(F("VS1053 found"));
    
     if (!SD.begin(CARDCS)) {
      Serial.println(F("SD failed, or not present"));
      while (1);  // don't do anything more
    }
    
    // Set volume for left, right channels. lower numbers == louder volume!
    musicPlayer.setVolume(NORMAL_VOLUME,NORMAL_VOLUME);
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int

    // end setup
#endif
