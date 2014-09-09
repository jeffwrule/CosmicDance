#include <SPI.h>
#include <SdFat.h>
#include <SdFatUtil.h>
#include <SFEMP3Shield.h>


/**
 * \brief Object instancing the SdFat library.
 *
 * principal object for handling all SdCard functions.
 */
SdFat sd;

/**
 * \brief Object instancing the SFEMP3Shield library.
 *
 * principal object for handling all the attributes, members and functions for the library.
 */
SFEMP3Shield MP3player;

void setup() {
  Serial.begin(9600);
  Serial.println(F("setup starting"));
  
  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");  
  
  MP3player.begin();
  MP3player.setVolume(30,30);

  Serial.println(F("Looking 0=play track 0, 1=play track 1, i=info, s=stop"));
  Serial.println(F("setup complete"));
}

void loop() {
  
  if (Serial.available() > 0) {
    char command = Serial.read();
    
    switch (command) {
      case '1': 
                Serial.println("Starting Track 1");
                delay(100);
                //MP3player.playTrack(1);
                MP3player.playMP3("track001.mp3");
                break;
      case '2':
                Serial.println("Starting Track 2");
                MP3player.playMP3("track020.acc");
                break;
      case 's':
                Serial.println("Stopping playback");
                MP3player.stopTrack();
                break;
      case 'i':
                Serial.print(F("isPlaying() = "));
                Serial.println(MP3player.isPlaying());
                break;
      case '+':
      case '-':
                union twobyte mp3_vol; // create key_command existing variable that can be both word and double byte of left and right.
                mp3_vol.word = MP3player.getVolume(); // returns a double uint8_t of Left and Right packed into int16_t
            
                if(command == '-') { // note dB is negative
                  // assume equal balance and use byte[1] for math
                  if(mp3_vol.byte[1] >= 254) { // range check
                    mp3_vol.byte[1] = 254;
                  } else {
                    mp3_vol.byte[1] += 2; // keep it simpler with whole dB's
                  }
                } else {
                  if(mp3_vol.byte[1] <= 2) { // range check
                    mp3_vol.byte[1] = 2;
                  } else {
                    mp3_vol.byte[1] -= 2;
                  }
                }
                // push byte[1] into both left and right assuming equal balance.
                MP3player.setVolume(mp3_vol.byte[1], mp3_vol.byte[1]); // commit new volume
                Serial.print(F("Volume changed to -"));
                Serial.print(mp3_vol.byte[1]>>1, 1);
                Serial.println(F("[dB]"));
                break;
      default:
                Serial.println("Unknown command: ");
                Serial.println(command);
                break;
    }
  }
  
}

