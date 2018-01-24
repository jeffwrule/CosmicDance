#include <SoftwareSerial.h>

#include <SoftwareSerial.h>
#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

#define IS_BRIEF True
#define IS_CHATTY True 

//non-amplified star, time etc
#define NORMAL_VOLUME 0          
// amplified most pieces
#define NORMAL_VOLUME 12 

#define FOB_QUIET_VOLUME 40    // alternate volume to use when using ENHANCED_STANDALONE mode 
#define BEEP_VOLUME  100    

//Which board are we using gravitech or sparkfun
//this is also needs to be set to match the seeeting in the SFEMP3Shield.h file
//GRAVITECH=1 gravitech mp3 board, GRAVITECH=0 (sparkfun mp3 board)



int fob_next_volume;

const int PM_LONG = 10;           // play mode long track010.xxx
const char file_type[] = "mp3";   // mp3, m4a, acc etc....


/*
 * transmit and recieve pins for the xbee.
 * Note the pins jumpers on the xbee shield are configured just the opposite
 * Tx=5, Rx=10
 */

/*
This example is for Series 1 XBee
Sends a TX16 or TX64 request with the value of analogRead(pin5) and checks the status response for success
Note: In my testing it took about 15 seconds for the XBee to start reporting success, so I've added a startup delay
*/

// director commands  
const unsigned char solo = 's';         // dance a solo
const unsigned char ensembl = 'e';      // everyone dance
const unsigned char halt = 'h';         // everyone stop what they are doing

const unsigned char empty_request = '\0';  // not a request

boolean is_dancing = false;       // is someone currently dancing
unsigned char dance_type;         // which dance type to performe (solo or ensembl) 
unsigned long dance_started;      // what time the dance started
unsigned long last_checkin;       // when did we last send our status to the director.
//int fobA_status;                  // the last read status of the keyfob
int solo_delay_seconds = 5;       // number of seconds to delay between plays
//int play_mode = PM_LONG;          // default to long dance pieces, can be changed by director
int num_dances = 0;               // how many times have we danced
char track_name[] = "trackNNN.ttt"; // place holder for track...

// 1 byte to hold transmit messages
uint8_t payload[] = { 0 };
// 1 byte to hold receive messages
unsigned char new_direction;
unsigned char last_direction;

unsigned int last_solo_read;
unsigned int last_ensembl_read;
unsigned int last_mp3_playing_write;

#define  solo_pin    A4
#define ensembl_pin A5
#define mp3_playing_pin A3

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

// reset the arduino
void(* resetFunc) (void) = 0;

//------------------------------------------------------------------------------
/**
 * \brief Setup the Arduino Chip's feature for our use.
 *
 * After Arduino's kernel has booted initialize basic features for this
 * application, such as Serial port and MP3player objects with .begin.
 * Along with displaying the Help Menu.
 *
 * \note returned Error codes are typically passed up from MP3player.
 * Whicn in turns creates and initializes the SdCard objects.
 *
 * \see
 * \ref Error_Codes
 */
  char buffer[6]; // 0-35K+null

void mp3_set_volume(int volume) {
  MP3player.setVolume(volume,volume);
}

// make a beeping sound
void do_beep(int num_millis, int delay_after) {
  Serial.println("Making a beep...");
  mp3_set_volume(BEEP_VOLUME);
  MP3player.enableTestSineWave(14);
  delay(num_millis);
  MP3player.disableTestSineWave();
  mp3_set_volume(NORMAL_VOLUME);
  if (delay_after > 0) {
    delay(delay_after);
  }
}

void setup() {

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");

  #if (GRAVITECH == 1)
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
    Serial.println("using GRAVITECH pin setup");
  #else
    Serial.println("using Sparkfun MP3 Card pin setup");
  #endif

  
  is_dancing = false;
  new_direction = empty_request;
  last_direction = empty_request;

  pinMode(solo_pin, INPUT);
  last_solo_read = 0;
  pinMode(ensembl_pin, INPUT);
  last_ensembl_read = 0;
  digitalWrite(ensembl_pin, LOW);
  pinMode(mp3_playing_pin, OUTPUT);
  last_mp3_playing_write = LOW;
  digitalWrite(mp3_playing_pin, LOW);
  last_direction = halt;                // assume we are halted at the start
  
  delay(2000);
  if (!sd.begin(SD_SEL, SPI_FULL_SPEED)) { 
      // sd.initErrorHalt();
      Serial.println("sd.begin failed, restarting...");
      delay(1000);
      resetFunc();
  }
  if (!sd.chdir("/")) {
    // sd.errorHalt("sd.chdir");  
    Serial.println("sd.chdir failed, restarting...");
    delay(1000);
    resetFunc();
  }
  
  MP3player.begin();
  MP3player.setVolume(NORMAL_VOLUME,NORMAL_VOLUME);
  
  // just let it settle for a bit
  delay(10000);  // delay 15 seconds to let things settle down

  // announce we are alive!
  do_beep(200, 50);
  do_beep(200, 50); 
  do_beep(200, 30); 
  
  Serial.println(F("End setup"));
}


/*
 * check the network to see if we any new directions 
 * from the director
 */
void check_for_direction() {
  // read until we get what we want or noting left to read
  #if defined IS_CHATTY
    Serial.println(F("checking for direction..."));
  #endif
  
  last_solo_read = digitalRead(solo_pin);
  last_ensembl_read = digitalRead(ensembl_pin);

  #ifdef IS_CHATTY
    Serial.print("Status: last_solo_read ");
    Serial.print(last_solo_read);
    Serial.print(", last_ensembl_read ");
    Serial.print(last_ensembl_read);
    Serial.print(", last_mp3_playing_write ");
    Serial.print(last_mp3_playing_write);
    Serial.print(", last_direction ");
    Serial.println((char)last_direction);
  #endif
    
  if (last_solo_read == 1) {
    new_direction = solo;
  } else if (last_ensembl_read == 1) {
    new_direction = ensembl;
  } else {
    new_direction = halt;
  }
  if (new_direction == last_direction) {  // we have already seen this state, nothing new here
    new_direction = empty_request;
  } else {                                // time to return something to do
    #if defined IS_BRIEF
      Serial.print("check_for_direction:Got something: ");
      Serial.println((char) new_direction);
    #endif
    last_direction = new_direction;
  }
}

void mp3_stop() {
    MP3player.stopTrack();
}

// stop the music and motor
void stop_all() {
  #if defined IS_BRIEF
    Serial.println("stop_all begin...");
  #endif
  is_dancing = false;
  last_direction = halt;
  mp3_stop();
  digitalWrite(mp3_playing_pin, LOW);
  last_mp3_playing_write = LOW;
  #if defined IS_CHATTY
    Serial.print(F("Stopping for "));
    Serial.print(solo_delay_seconds);
    Serial.println(F(" for dancer to see new status"));
  #endif
  delay(1000 * solo_delay_seconds); // give the remote player a chance to sense this and react
  #if defined IS_CHATTY
    Serial.println(F("stop_all end..."));
  #endif
}

int mp3_play_track(unsigned char dance_piece) {
  digitalWrite(mp3_playing_pin, HIGH);
  last_mp3_playing_write = HIGH;
  int result = MP3player.playMP3(track_name, 0);
  return(result);
}

// start playing the solo or the ensembl
void start_dancing(unsigned char dance_piece) {
  int my_track;
  
  #if defined IS_BRIEF
    Serial.print("start_dancing starting: ");
    Serial.print((char)dance_piece);
    num_dances += 1;
    Serial.print(" total dances: ");
    Serial.println(num_dances);
  #endif
  // don't write a response about starting file fobA is on

  switch (dance_piece) {
    case solo:     my_track = 1 * PM_LONG; break;
    case ensembl:  my_track = 2 * PM_LONG; break;
    default: my_track = 0;
  }
  sprintf(track_name, "track%03d.%s", my_track, file_type);
  #if defined IS_BRIEF
    Serial.print("Starting Track:");
    Serial.println(track_name);
  #endif
  int result = mp3_play_track(dance_piece);
  if(result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to play track");
  }
  delay(300); // this seems to stick sometimes when we start dancing
  is_dancing = true;
  dance_type = dance_piece;
  dance_started = millis();
  last_checkin = dance_started;
  #if defined IS_CHATTY
    Serial.println(F("start_dancing end"));
  #endif
}

// are we currently dancing?


int mp3_is_playing() {
  return(MP3player.isPlaying());
}

// is the finished playing?
boolean track_is_complete() {
  int retval;
  #if defined IS_CHATTY
    Serial.println("checking is playing...");
  #endif
  retval = mp3_is_playing();
//  return MP3player.isPlaying() == 0;
  #if defined IS_CHATTY
    Serial.println("check complete...");
  #endif
  return(retval == 0);
}

// are we done?
void signal_if_done() {
  // simulate the MP3 stopping after 13 seconds
  #if defined IS_CHATTY 
    Serial.print("signal_if_done track status: ");
  #endif
  if (track_is_complete()) {
    #if defined IS_CHATTY
      Serial.println(F("signal_if_done track status: complete"));
    #endif
    #if defined IS_CHATTY
      Serial.println(F("Fob is off, sending finished message to director"));
    #endif
    stop_all();
  }
  #if defined IS_CHATTY 
    else { Serial.println("track not done!"); }
  #endif
}

// start and stop dancers, keep the show moving along....
void loop() {
  
  // MP3player.available();  // fill the buffer if we are polling or simpleTimer
  
  check_for_direction();

  switch (new_direction) {
    case halt: 
              stop_all(); break;
    case solo: 
    case ensembl: 
              start_dancing(new_direction); break;
  }
  new_direction = empty_request;
  
  if (is_dancing) {
    // has the music stopped? shut it down down and notify
    signal_if_done();
  }
  
}

