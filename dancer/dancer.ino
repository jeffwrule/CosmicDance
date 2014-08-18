#include <XBee.h>
#include <SoftwareSerial.h>


#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

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
const int fobA = A5;                      // input pin that the fobA button is hooked up too

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

// dancer responses 
const unsigned char finished = 'f';       // peice has been completed
const unsigned char acknowledge = 'a';    // I head you!
const unsigned char dancing = 'd';        // dancing

const unsigned char empty_request = '\0';  // not a request

boolean is_dancing = false;       // is someone currently dancing
unsigned char dance_type;         // which dance type to performe (solo or ensembl) 
unsigned long dance_started;      // what time the dance started
unsigned long last_checkin;      // when did we last send our status to the director.
int fobA_status;                 // the last read status of the keyfob
int solo_delay_seconds = 30;     // number of seconds to delay between plays

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

  // start = millis();            // when the program first started
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
  
  if(!sd.begin(9, SPI_HALF_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");  
  
  MP3player.begin();
  MP3player.setVolume(30,30);
  
  // just let it settle for a bit
  delay(10000);  // delay 15 seconds to let things settle down
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
    Serial.print("check_for_direction:Got something: ");
    Serial.println((char) new_direction);
  }
}

// stop the music and motor
void stop_all() {
  Serial.println("stop_all begin...");
  is_dancing = false;
  MP3player.stopTrack();
  digitalWrite(v12Switch, LOW);
  digitalWrite(v5Switch, LOW);
  Serial.println("stop_all end...");
}

boolean director_heard_me() {
  if (xbeeSerial.available() > 0) {
    return xbeeSerial.read() == acknowledge;
  }
  return false;
}

// start playing the solo or the ensembl
void start_dancing(unsigned char dance_piece) {
  Serial.print("start_dancing starting: ");
  Serial.println((char)dance_piece);
  xbeeSerial.write(dancing);
  switch (dance_piece) {
    case solo:     MP3player.playTrack(1); break;
    case ensembl:  MP3player.playTrack(2); break;
  }
  digitalWrite(v12Switch, HIGH);
  digitalWrite(v5Switch, HIGH);
  is_dancing = true;
  dance_type = dance_piece;
  dance_started = millis();
  last_checkin = dance_started;
  Serial.println("start_dancing end");
}

// are we currently dancing?
void reply_status() {
  Serial.print("sending status: ");
  if (is_dancing) {
    xbeeSerial.write(dancing);
    Serial.println("dancing");
  } else {
    xbeeSerial.write(finished); 
    Serial.println("finished");
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
      Serial.println("signal_if_done track status: complete");
    if (fobA == 0) {
      Serial.println("Fob is off, sending message to director");
      xbeeSerial.write(finished);
    }
    stop_all();
    if (fobA == 1) {
      // simulate the sleep between tracks
      Serial.print("Fob is on, pausing for ");
      Serial.print(solo_delay_seconds);
      Serial.println(" between plays");
      delay(1000 * solo_delay_seconds);
    }
  }
}

// are we done?
void signal_every_so_often() {

  // no need to send out a signe we are on our own here
  if (fobA_status == 1) {
    return;
  }
  
  if (millis() - last_checkin > 4000) {
    Serial.println("signal_every_so_often checking in...");
    xbeeSerial.write(dancing);
    last_checkin = millis();
  }
  // Serial.println("playing"); 
}

void check_for_fob() {
  
  int cur_status = digitalRead(fobA);
  
  // detect going from on to off 
  if (cur_status != fobA_status) {
    Serial.print("FobA status changed to: ");
    Serial.println(cur_status);
    fobA_status = cur_status;
    // immediately stop everything when the fob goes dark
    if (cur_status == 0) {
      stop_all();
    }
  }

  // fob is pushed override the enviornment  
  if (cur_status == 1) {
    new_direction = empty_request;  // default to no incomming command
    // always be dancing...
    if (is_dancing == false) {
      new_direction = solo;  // simulate a new request to do a solo 
    }
  }
}

// start and stop dancers, keep the show moving along....
void loop() {
  
  check_for_direction();
  check_for_fob();
  switch (new_direction) {
    case halt: 
              stop_all(); break;
    case solo: 
    case ensembl: 
              start_dancing(new_direction); break;
    case whatchadoing: 
              reply_status(); break;
  }
  new_direction = empty_request;
  // has the music stopped?
  if (is_dancing) {
    signal_if_done();
    signal_every_so_often();
  }
}

