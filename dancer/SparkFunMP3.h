#ifndef SPARKFUNMP3_H
#define SPARKFUNMP3_H

#include </Users/jrule/Documents/Arduino/Dancer/GenericMP3.h>
#include <SdFat.h>
#include <SFEMP3Shield.h>
#include <SPI.h>

SdFat sd;

class SparkFunMP3: public GenericMP3 {

  public:
    void mp3_stop();
    boolean mp3_play_track(const char *track_name) ;
    void mp3_set_volume(int volume);
    int mp3_is_playing();
    boolean track_is_complete();
    void do_beep(int num_millis, int delay_after);
    void status(boolean do_print);

    // normal_volume standard play volume
    // beep_volume     volume to play the bee at
    // sd_sel          sd_select pin (use the Arduino Constant SD_SEL)
    // spi_speed       speed to run spi interface at typeical (SPI_FULL_SPEED)
    SparkFunMP3(unsigned int normal_volume, 
                unsigned int beep_volume, 
                uint8_t sd_sel, 
                SPISettings spi_speed) : 
      normal_volume(normal_volume),
      beep_volume(beep_volume),
      sd_sel(sd_sel),
      spi_speed(spi_speed) {
        Serial.println(F("using Sparkfun MP3 Card pin setup"));
        delay(2000);
        // retry the sd.begin up to 5 times.
        boolean sd_status;
        for(int i=0; i<5; i++) {
          sd_status = sd.begin(sd_sel, spi_speed);
          if (sd_status) { Serial.println(F("sd.begin completed")); break; }
          Serial.println(F("sd.begin failed..."));
          delay(1000);
        }
        if (!sd_status) {
          Serial.println(F("sd.being, too many fails rebooting"));
          delay(2000);
          asm(" jmp 0");
        }
        for (int i=0; i<5; i++) {
          sd_status = sd.chdir("/"); 
          if (sd_status) { Serial.println(F("sd.chdir completed")); break; }
          Serial.println(F("sd.chdir failed..."));
          delay(1000);
        }
        if (!sd_status) {
          Serial.println(F("sd.chdir, too many fails rebooting"));
          delay(2000);
          asm(" jmp 0");
        }
                
        MP3player.begin();
        MP3player.setVolume(normal_volume,normal_volume);    
  
        Serial.print(F("SparkFunMP3::CONSTRUCTOR normal_volume="));
        Serial.print(normal_volume);
        Serial.print(F(", GRAVITECH="));
        Serial.println(GRAVITECH);
  
        if (GRAVITECH == 1) {
          Serial.println(F("ERROR: GRAVITECH=1 when using SparkFun MP3, edit SFEMP3ShieldConfig.h"));
        }
    }

  private:
//    SdFat sd;
    SFEMP3Shield MP3player;
    unsigned int normal_volume;
    unsigned int beep_volume;
    uint8_t sd_sel;
    SPISettings spi_speed;
};

void SparkFunMP3::status(boolean do_print) {
  if (! do_print) { return; }
  Serial.print(F("SparkFunMP3::status() mp3_is_playing()="));
  Serial.print(bool_tostr(mp3_is_playing()));
  Serial.print(F(", track_is_complete()="));
  Serial.println(bool_tostr(track_is_complete()));
}
void SparkFunMP3::mp3_stop() {
  MP3player.stopTrack();
}

boolean SparkFunMP3::mp3_play_track(const char *track_name) {
  int result = MP3player.playMP3(track_name, 0);
    if(result != 0) {
      Serial.print(F("Error code: "));
      Serial.print(result);
      Serial.println(F(" when trying to play track"));
    }
  return(result == 0);
}

void SparkFunMP3::mp3_set_volume(int volume) {
  MP3player.setVolume(volume,volume);
}

int SparkFunMP3::mp3_is_playing() {
  return(MP3player.isPlaying());
}

// is the finished playing?
boolean SparkFunMP3::track_is_complete() {
  int retval;
  #if defined IS_CHATTY
    Serial.println(F("checking is playing..."));
  #endif
  retval = mp3_is_playing();
  #if defined IS_CHATTY
    Serial.println(F("check complete..."));
  #endif
  return(retval == 0);
}


// make a beeping sound
void SparkFunMP3::do_beep(int num_millis, int delay_after) {
  if (mp3_is_playing()) { mp3_stop(); }
  Serial.println(F("Making a beep..."));
  MP3player.setVolume(beep_volume,beep_volume);
  MP3player.enableTestSineWave(14);
  delay(num_millis);
  MP3player.disableTestSineWave();
  MP3player.setVolume(NORMAL_VOLUME,NORMAL_VOLUME);
  if (delay_after > 0) {
    delay(delay_after);
  }
}

#endif
