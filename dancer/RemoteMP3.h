#ifndef REMOTEMP3_H
#define REMOTEMP3_H
#ifdef REMOTE_MP3


setup()
  Serial.println(F("MP3 is remote no MP3 card configured"));

    Serial.println(F("Setting up solo_pin, ensembl_pin: OUTPUT, mp3_playing_pin: INPUT"));
  
  pinMode(solo_pin, OUTPUT);
  digitalWrite(solo_pin, LOW);
  delay(50);
  last_solo_write = LOW;
  
  pinMode(ensembl_pin, OUTPUT);
  digitalWrite(ensembl_pin, LOW);
  delay(50);
  last_ensembl_write = LOW;

  pinMode(mp3_playing_pin, INPUT);
  delay(50);
  last_mp3_playing_read = digitalRead(mp3_playing_pin);
// end setup

void remote_mp3_status()
{
  #ifdef IS_CHATTY
    Serial.print(F("remote_mp3_status: solo "));
    Serial.print(last_solo_write);
    Serial.print(F(", ensembl "));
    Serial.print(last_ensembl_write);
    Serial.print(F(", mp3_is_playing "));
    Serial.println(last_mp3_playing_read);
  #endif
}
#endif



#ifdef REMOTE_MP3
  #define  solo_pin    A4
  #define ensembl_pin A5
  #define mp3_playing_pin 3

  int last_solo_write;
  int last_ensembl_write;
  int last_mp3_playing_read;



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
