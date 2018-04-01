#ifndef GRAVITECHMP3_H
#define GRAVITECHMP3_H

#include "GenericMP3.h"

/// from setup()

  #if (GRAVITECH == 1 )
    pinMode( 2, OUTPUT);
    pinMode( 3, OUTPUT);
    pinMode( 6, OUTPUT);
    pinMode( 7, OUTPUT);
    pinMode( 8, OUTPUT);
    pinMode(10, OUTPUT);  // recommended in http://flashgamer.com/hardware/comments/tutorial-making-gravitechs-mp3-player-add-on-work
    pinMode(11, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(SD_SEL, OUTPUT);  // 4 (gravitech) or 9 (sparkfun)
    Serial.println(F("using GRAVITECH pin setup"));


#include <SdFat.h>
    
//and the MP3 Shield Library
#include <SFEMP3Shield.h>

SdFat sd;
SFEMP3Shield MP3player;
        
class SparkFunMP3: public GenericMP3 {

  public:
    void mp3_stop();
    int mp2_play_track(unsigned char dance_piece);
    void mp3_set_volume(int volume);
    void int mp3_is_playing();
    boolean track_is_complete();
    void do_beep(int num_millis, int delay_after);
    void status(boolean do_print);

    SparkFunMP3(

  private:
    
};

    
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



  #endif // HAS_ADAFRUIT_MP3


//Which board are we using gravitech or sparkfun
//this is also needs to be set to match the setting in the SFEMP3Shield.h file
//GRAVITECH=1 gravitech mp3 board, GRAVITECH=0 (sparkfun mp3 board)


void mp3_stop() {
  #ifdef REMOTE_MP3
    digitalWrite(solo_pin, LOW);
    last_solo_write = LOW;
    digitalWrite(ensembl_pin, LOW);
    last_ensembl_write = LOW;
  #else
    #ifdef HAS_ADAFRUIT_MP3
      musicPlayer.stopPlaying();
    #else
      MP3player.stopTrack();
    #endif
  #endif
}

int mp3_play_track(unsigned char dance_piece) {
  int result = 0;

  #ifdef REMOTE_MP3

    #ifdef IS_BRIEF
      Serial.print(F("Starting remote MP3 music setting remote pins for dance type: "));
      Serial.println((char)dance_piece);
    #endif
    switch(dance_piece) {
      case solo: Serial.println(F("case: solo chosen"));
                 digitalWrite(solo_pin, HIGH);
                 digitalWrite(ensembl_pin, LOW);
                 last_solo_write = HIGH;
                 last_ensembl_write = LOW;
                 break;
                 
      case ensembl: Serial.println(F("case: ensembl chosen"));
                    digitalWrite(ensembl_pin, HIGH);
                    digitalWrite(solo_pin, LOW);
                    last_solo_write = LOW;
                    last_ensembl_write = HIGH;
                    break;
                    
      default: Serial.println(F("case: default chosen, probably an ERROR!"));
               digitalWrite(solo_pin, LOW);
               digitalWrite(ensembl_pin, LOW);
               last_solo_write = LOW;
               last_ensembl_write = LOW;
               break;
    }
    remote_mp3_status();
    // give the remote device a chance to see our request 
    for (int i = 0;  i < ((solo_delay_seconds * 2) + 1); i++) {
      delay(500);
      last_mp3_playing_read = digitalRead(mp3_playing_pin);
      #ifdef IS_BRIEF
          Serial.print(F("mp3_play_track: mp3_is_playing: "));
          Serial.println(last_mp3_playing_read);
      #endif
      if (last_mp3_playing_read == 1) { break; }
    }
    #ifdef IS_BRIEF
      Serial.print(F("mp3_play_track: mp3_is_playing FINAL: "));
      Serial.println(last_mp3_playing_read);
    #endif
  #else
    #ifdef HAS_ADAFRUIT_MP3
      result =  musicPlayer.startPlayingFile(track_name);
    #else
      result = MP3player.playMP3(track_name, 0);
    #endif // HAS_ADAFRUIT_MP3
  #endif

  return(result);
}



void mp3_set_volume(int volume) {

  #ifndef REMOTE_MP3
    #ifdef HAS_ADAFRUIT_MP3
      musicPlayer.setVolume(volume, volume);
    #else
      MP3player.setVolume(volume,volume);
    #endif // HAS_ADAFRUIT_MP3
  #endif     
}



int mp3_is_playing() {
  #ifdef REMOTE_MP3
    return(digitalRead(mp3_playing_pin));
  #else
    #ifdef HAS_ADAFRUIT_MP3
      return(musicPlayer.playingMusic);
    #else
      return(MP3player.isPlaying());
    #endif // HAS_ADAFRUIT_MP3
  #endif
}

// is the finished playing?
boolean track_is_complete() {
  int retval;
  #if defined IS_CHATTY
    Serial.println(F("checking is playing..."));
  #endif
  retval = mp3_is_playing();
//  return MP3player.isPlaying() == 0;
  #if defined IS_CHATTY
    Serial.println(F("check complete..."));
  #endif
  return(retval == 0);
}


// make a beeping sound
void do_beep(int num_millis, int delay_after) {
#ifdef REMOTE_MP3
  delay(1);
#else
  Serial.println(F("Making a beep..."));
  #ifdef HAS_ADAFRUIT_MP3
    musicPlayer.setVolume(BEEP_VOLUME,BEEP_VOLUME);
    musicPlayer.sineTest(14, num_millis);
    musicPlayer.setVolume(NORMAL_VOLUME,NORMAL_VOLUME);
  #else
    MP3player.setVolume(BEEP_VOLUME,BEEP_VOLUME);
    MP3player.enableTestSineWave(14);
    delay(num_millis);
    MP3player.disableTestSineWave();
    MP3player.setVolume(NORMAL_VOLUME,NORMAL_VOLUME);
  #endif // HAS_ADAFRUIT_MP3
  if (delay_after > 0) {
    delay(delay_after);
  }
#endif
}



#endif
