#include <XBee.h>
#include <SoftwareSerial.h>


#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

#define IS_BRIEF True
//#define IS_CHATTY True 

const int PM_MINI = 1;      // play mode mini
const int PM_LONG = 10;     // play mode long

/*
 * transmit and recieve pins for the xbee.
 * Note the pins jumpers on the xbee shield are configured just the opposite
 * Tx=5, Rx=10
 */
const unsigned int RxD = 5;
const unsigned int TxD = 10;
const unsigned int delay_seconds = 20;    // number of seconds to wait to restart cycle
const int v12Switch = A0;                 // pin to turn the electronics on and off
const int v5Switch = A1;                  // in to turn on/off
const int fobA = A3;                      // input pin that the fobA button is hooked up too
const int FOB_ON = 1;                     // the FOB is on.

/*
This example is for Series 1 XBee
Sends a TX16 or TX64 request with the value of analogRead(pin5) and checks the status response for success
Note: In my testing it took about 15 seconds for the XBee to start reporting success, so I've added a startup delay
*/

// director commands  
const unsigned char solo = 's';         // dance a solo
const unsigned char ensembl = 'e';      // everyone dance
const unsigned char halt = 'h';         // everyone stop what they are doing
const unsigned char whatchadoing = 'w';  // are you dancing or not?
const unsigned char mini_track = 'm';          // play the sort track, track001 and track 002
const unsigned char long_track = 'l';          // play the full version of the songs track010 and track020


// dancer responses 
const unsigned char finished = 'f';       // peice has been completed
const unsigned char acknowledge = 'a';    // I head you!
const unsigned char dancing = 'd';        // dancing

const unsigned char empty_request = '\0';  // not a request

boolean is_dancing = false;       // is someone currently dancing
unsigned char dance_type;         // which dance type to performe (solo or ensembl) 
unsigned long dance_started;      // what time the dance started
unsigned long last_checkin;       // when did we last send our status to the director.
int fobA_status;                  // the last read status of the keyfob
int solo_delay_seconds = 5;       // number of seconds to delay between plays
int play_mode = PM_LONG;          // default to long dance pieces, can be changed by director
int num_dances = 0;               // how many times have we danced

// 1 byte to hold transmit messages
uint8_t payload[] = { 0 };
// 1 byte to hold receive messages
unsigned char new_direction;

// create an Xbee object to communite with the XBee card
XBee xbee = XBee();             
// 16-bit transmit request
Tx16Request tx = Tx16Request(0xFFFE, payload, sizeof(payload));
// 16-bit recieve frame
Rx16Response rx16 = Rx16Response();
// 16-bit status response
TxStatusResponse txStatus = TxStatusResponse();
// software serial port
SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX

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

void setup() {

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  
  // setup the soft serial port for xbee reads and writes
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(v12Switch, OUTPUT);
  digitalWrite(v12Switch, LOW);
  pinMode(v5Switch, OUTPUT);
  digitalWrite(v5Switch, LOW);
  pinMode(fobA, INPUT);
  
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  
  is_dancing = false;
  new_direction = empty_request;
  
  if(!sd.begin(9, SPI_FULL_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");  
  
  MP3player.begin();
  MP3player.setVolume(20,20);
  
  // just let it settle for a bit
  delay(10000);  // delay 15 seconds to let things settle down
  Serial.print("FobA status=");
  Serial.println(digitalRead(fobA));
  Serial.println("End setup");
}

/*
 * check the network to see if we any new directions 
 * from the director
 */
void check_for_direction() {
  // read until we get what we want or noting left to read
  if (xbeeSerial.available() > 0) {
    new_direction = xbeeSerial.read();
    #if defined IS_BRIEF
      Serial.print("check_for_direction:Got something: ");
      Serial.println((char) new_direction);
    #endif
  }
}

// stop the music and motor
void stop_all() {
  #if defined IS_BRIEF
    Serial.println("stop_all begin...");
  #endif
  is_dancing = false;
  MP3player.stopTrack();
  digitalWrite(v12Switch, LOW);
  digitalWrite(v5Switch, LOW);
  #if defined IS_CHATTY
    Serial.println("stop_all end...");
  #endif
}

// start playing the solo or the ensembl
void start_dancing(unsigned char dance_piece) {
  #if defined IS_BRIEF
    Serial.print("start_dancing starting: ");
    Serial.print((char)dance_piece);
    num_dances += 1;
    Serial.print(" total dances: ");
    Serial.println(num_dances);
  #endif
  // don't write a response about starting file fobA is on
  if (fobA_status == FOB_ON) {
    #if defined IS_CHATTY
      Serial.println("fobA is on, skip sending dancing status to director");
    #endif
  } else {
    xbeeSerial.write(dancing);
  }
  switch (dance_piece) {
    case solo:     MP3player.playTrack(1 * play_mode); break;
    case ensembl:  MP3player.playTrack(2); break;
  }
  delay(100); // this seems to stick sometimes when we start dancing
  digitalWrite(v12Switch, HIGH);
  digitalWrite(v5Switch, HIGH);
  is_dancing = true;
  dance_type = dance_piece;
  dance_started = millis();
  last_checkin = dance_started;
  #if defined IS_CHATTY
    Serial.println("start_dancing end");
  #endif
}

// are we currently dancing?
void reply_status() {
  #if defined IS_BRIEF
    Serial.print("sending status: ");
  #endif
  if (is_dancing) {
    xbeeSerial.write(dancing);
    #if defined IS_BRIEF
      Serial.println("dancing");
    #endif
  } else {
    xbeeSerial.write(finished); 
    #if defined IS_BRIEF
      Serial.println("finished");
    #endif
  }
}

// is the finished playing?
boolean track_is_complete() {
  return MP3player.isPlaying() == 0;
}

// are we done?
void signal_if_done() {
  // simulate the MP3 stopping after 13 seconds
  // Serial.print("signal_if_done track status: ");
  if (track_is_complete()) {
    #if defined ISCHATTY
      Serial.println("signal_if_done track status: complete");
    #endif
    if (fobA_status != FOB_ON) {
      #if defined IS_CHATTY
        Serial.println("Fob is off, sending finished message to director");
      #endif
      xbeeSerial.write(finished);
    } else {
      #if defined IS_CHATTY
        Serial.println("Fob is on, skipping 'finished' message to director");
      #endif
    }
    stop_all();
    if (fobA_status == FOB_ON) {
      // simulate the sleep between tracks
      #if defined IS_CHATTY
        Serial.print("FobA is on, pausing for ");
        Serial.print(solo_delay_seconds);
        Serial.println(" between plays");
      #endif
      delay(1000 * solo_delay_seconds);
    }
  }
}

// are we done?
void signal_every_so_often() {

  // no need to send out a signe we are on our own here
  if (fobA_status == FOB_ON) {
    return;
  }
  
  if (millis() - last_checkin > 4000) {
    #if defined IS_CHATT
      Serial.println("signal_every_so_often checking in...");
    #endif
    xbeeSerial.write(dancing);
    last_checkin = millis();
  }
}

void drain_serial() {
  // drain any outstanding input queries....
  while (xbeeSerial.available() > 0) {
    char x = xbeeSerial.read();
    # if defined IS_CHATTY
      Serial.print("Draining Serial got: ");
      Serial.println(x);
    #endif  
  }
}

void check_for_fob() {
  
  int cur_status = digitalRead(fobA);
  delay(30);    // add a short delay here, checking this seems to tweek the whole system.
  
  // detect a fob state, if we get one reset everything 
  if (cur_status != fobA_status) {
    #if defined IS_BRIEF
      Serial.print("FobA status changed to: ");
      Serial.println(cur_status);
    #endif
    fobA_status = cur_status;
    play_mode = PM_LONG;
    stop_all();
  }

  // fob is pushed override any incomming direction  
  if (cur_status == FOB_ON) {
    drain_serial();        // don't let this build up too much...
    if (new_direction != empty_request) {
      #if defined IS_CHATTY
        Serial.print("fobA is on, ignoring direction: ");
        Serial.println((char)new_direction);
      #endif
    }
    new_direction = empty_request;  // default to no incomming command
    // always be dancing...
    if (is_dancing == false) {
      #if defined IS_CHATTY
        Serial.println("fobA is on and not dancing, restarting dance");
      #endif
      new_direction = solo;  // simulate a new request to do a solo 
    }
  }
}

// start and stop dancers, keep the show moving along....
void loop() {
  
  check_for_fob();
  if (fobA_status != FOB_ON) {
    check_for_direction();
  }
  switch (new_direction) {
    case halt: 
              stop_all(); break;
    case solo: 
    case ensembl: 
              start_dancing(new_direction); break;
    case whatchadoing: 
              reply_status(); break;
    case mini_track:
              play_mode = PM_MINI; break;
    case long_track:
              play_mode = PM_LONG; break;
  }
  new_direction = empty_request;
  // has the music stopped?
  if (is_dancing) {
    signal_if_done();
    signal_every_so_often();
  }
}

