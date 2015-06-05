#include <SoftwareSerial.h>
#include <SPI.h>


//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

#define IS_BRIEF True
//#define IS_CHATTY False 

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
char track_name[] = "trackxxx.xxx"; // place holder for track...


// motor direction and top speed
#define STOPPED 0
#define FORWARD 1
#define BACKWARD 2

const int LeftLimit = 30;                 // Left will also be down limit
const int RightLimit = 31;                // Right will also be up

uint8_t motor_direction = FORWARD;  // default to running the motor forward
uint8_t motor_top_speed = 255;
uint8_t motor_current_speed = 0;   // for spin up/down to gradually step up the motor...
uint8_t limit_is_active=false;     // are eithe of the limit switches currently..
uint8_t num_limit_checks = 0;      // lets us send a limit check message out every so often...
uint8_t print_every_nth = 50;     // 0-255 only print ever nth limit check status....


// 1 byte to hold transmit messages
uint8_t payload[] = { 0 };
// 1 byte to hold receive messages
unsigned char new_direction;

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
  
  pinMode(v12Switch, INPUT);
  digitalWrite(v12Switch, LOW);
  pinMode(v5Switch, OUTPUT);
  digitalWrite(v5Switch, LOW);
  
  // the fob/switch input
  pinMode(fobA, INPUT_PULLUP);
  
  Serial1.begin(9600);
  
  is_dancing = false;
  new_direction = empty_request;
  
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  if (!sd.chdir("/")) sd.errorHalt("sd.chdir");  
  
  MP3player.begin();
  MP3player.setVolume(12,12);
  
  // just let it settle for a bit
  delay(10000);  // delay 15 seconds to let things settle down
  Serial.print("FobA status=");
  Serial.println(digitalRead(fobA));
  Serial.print("MP3_RESET is: ");
  Serial.println(MP3_RESET);
  Serial.println("End setup");
}

/*
 * check the network to see if we any new directions 
 * from the director
 */
void check_for_direction() {
  // read until we get what we want or noting left to read
  #if defined IS_CHATTY
    Serial.println("checking for direction...");
  #endif
  if (Serial1.available() > 0) {
    new_direction = Serial1.read();
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
  int my_track;
  
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
    Serial1.write(dancing);
  }
  switch (dance_piece) {
    case solo:     my_track = 1 * play_mode; break;
    case ensembl:  my_track = 2 * play_mode; break;
    default: my_track = 0;
  }
  sprintf(track_name, "track%03d.m4a", my_track);
  #if defined IS_BRIEF
    Serial.print("Starting Track:");
    Serial.println(track_name);
  #endif
  int result = MP3player.playMP3(track_name, 0);
  if(result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to play track");
  } else {
    #if defined IS_CHATTY
      Serial.println("Dance queued");
    #endif
  }
  //delay(300); // this seems to stick sometimes when we start dancing
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
    Serial1.write(dancing);
    #if defined IS_BRIEF
      Serial.println("dancing");
    #endif
  } else {
    Serial1.write(finished); 
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
    #if defined IS_CHATTY
      Serial.println("signal_if_done track status: complete");
    #endif
    if (fobA_status != FOB_ON) {
      #if defined IS_CHATTY
        Serial.println("Fob is off, sending finished message to director");
      #endif
      Serial1.write(finished);
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

  // no need to send out a signal we are on our own here
  if (fobA_status == FOB_ON) {
    return;
  }
  
  if (millis() - last_checkin > 4000) {
    #if defined IS_CHATTY
      Serial.println("signal_every_so_often checking in...");
    #endif
    Serial1.write(dancing);
    last_checkin = millis();
  }
}

void drain_serial() {
  // drain any outstanding input queries....
  while (Serial1.available() > 0) {
    # if defined IS_CHATTY
      char x = Serial1.read();
      Serial.print("Draining Serial got: ");
      Serial.println(x);
    #else
      Serial.println("draining...");
      Serial1.read();
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

// quickly slow down the motor....
void shutdown_motor() {
  uint8_t i;
  for (i=motor_current_speed; i!=0; i--) {
    //myMotor->setSpeed(i);
    if ((i % 50) == 0) {
      delay(10);
    }
  }
  //myMotor->run(RELEASE);
  motor_current_speed = 0;
  delay(30);
}

// slowly startup the motor in the currenct direction
void ramp_up_motor() {
  if (is_dancing || limit_is_active) {
    if (motor_current_speed == 0) {
      //myMotor->setSpeed(1);
      //myMotor->run(motor_direction);
    } 
    if (motor_current_speed == motor_top_speed) {
      return;
    }
    motor_current_speed += 5;
    if (motor_current_speed > motor_top_speed) {
      motor_current_speed == motor_top_speed;
    }
    //myMotor->setSpeed(motor_current_speed);
  }
}

// dump the status of the limit pins
void print_limit_status(int left_status, int right_status) {
  
  num_limit_checks += 1;
  if ((num_limit_checks % print_every_nth) != 0) {
    return;
  }
  Serial.print(F("Stop Limit Triggered Left: "));
  Serial.print(left_status);
  Serial.print(F(" right: "));
  Serial.print(right_status);
  Serial.print(F(" limit_is_active: "));
  Serial.print(limit_is_active);
  Serial.print(F(" Motor Direction "));
  switch (motor_direction) {
    case FORWARD:  Serial.println(F("FORWARD"));
                   break;
    case BACKWARD:  Serial.println(F("BACKWARD"));
                   break;
    default:  Serial.print(F("unexpected:"));
              Serial.print(motor_direction, DEC);
                   break;
  }   
}

// reverse the motor if we have reached a limit
void check_limit_switches() {
  
  int left_status = digitalRead(LeftLimit);
  delay(20);
  int right_status = digitalRead(RightLimit);
  delay(20);
  
  if (left_status == HIGH && right_status == HIGH) {
    // something is very wrong both switches should
    // not be on at the same time, sshut it all down
    // could be the switches are not hooked up an float
    // b/c of the pull up resistor, they will look on
    // when not connected.
    limit_is_active = false;
    shutdown_motor();
    print_limit_status(left_status, right_status);
    return;
  }
  if (left_status == HIGH || right_status == HIGH) {
     limit_is_active = true;
     if (left_status == HIGH) {
       if (motor_direction == BACKWARD) {
         shutdown_motor();
       }
       motor_direction = FORWARD;
     } else {
       if (motor_direction == FORWARD) {
         shutdown_motor();
       }
       motor_direction = BACKWARD;
     }
  } else {
    // limit switch just cleared and we aren't dancing, kill the motor
    if (limit_is_active && !is_dancing) { 
      shutdown_motor();
    }
    limit_is_active = false;
  }
  print_limit_status(left_status, right_status);
  // increase motor speed or move it along if necessary
  ramp_up_motor();
}

// start and stop dancers, keep the show moving along....
void loop() {
  
  check_for_fob();
  check_limit_switches();  // have we hit a limit switch?

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

