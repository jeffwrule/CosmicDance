#include <SoftwareSerial.h>

#include <SoftwareSerial.h>
#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

#define IS_BRIEF True
//#define IS_CHATTY True 

//non-amplified star, time etc
#define NORMAL_VOLUME 0          
// amplified most pieces
#define NORMAL_VOLUME 12 

// comment out if there is not xbee card (not fully tested...)
//#define HAS_XBEE True

#define FOB_QUIET_VOLUME 40    // alternate volume to use when using ENHANCED_STANDALONE mode 
#define BEEP_VOLUME  100    

//Which board are we using gravitech or sparkfun
//this is also needs to be set to match the seeeting in the SFEMP3Shield.h file
//GRAVITECH=1 gravitech mp3 board, GRAVITECH=0 (sparkfun mp3 board)

// uncomment the VARY_SPEED define on if you want to have a variable speed motor 
// you must attach PIN3 to the mosfet to make this work.
// you also need to cut the traces for pin three on the MP3 card or you will conflict with the MP3 card.
// The speed of the motor will continiously vary between MIN and MAX speed
// The VARY_SECONDS defines how long it takes to go from MIN to MAX or vise versa
// To create a motor that starts above or below the variable range set INIT speed, above MAX (typical) or below MIN. 
// The montor will then be ramped down or up to MIN/MAX and then vary between MIN/MAX until music ends.
// If you want a montor that just has a jump start set INIT high and MIN = MAX.  
//    This will ramp down(typical)/up the speed until it reaches MIN/MAX which are equal and will not move again.
// SET VARY_SECONDS to set the number of seconds between MIN/MAX values
// SET VARY_SECONDS_INIT to set the number of seconds between INIT and reaching either MIN if INIT is below min or MAX if INIT is above MAX.

//#define VARY_SPEED

//#define VARY_MIN_SPEED 40L   // string wave (oval piece) 
//#define VARY_MAX_SPEED 40L   // string wave (oval piece)
//#define VARY_INIT_SPEED 60L  // string wave (oval piece)
#define VARY_MIN_SPEED 109L    // string ripple 
#define VARY_MAX_SPEED 109L    // string ripple
#define VARY_INIT_SPEED 119L   // string ripple 
#define VARY_PIN 3             // D3 pin, if you are going to use this you must cut the trace on mp3 card for MIDI-IN
#define VARY_OFF 0             // OFF Speed for monto (do not change this)
#define VARY_SECONDS 20L       // number of seconds between min and max
#define VARY_SECONDS_INIT 5L  // number of seconds to vary the init value down/up to min/max

// global variables for VARY speed feature
float vary_step = 1.0;                    // default, how big a step to take when incrementing or decrementing
float vary_step_init = 1.0;               // default, how big a step to take when incrementing or decrementing
float vary_current_speed = VARY_OFF;      // current motor speed
unsigned long vary_prev_millis;           // mills last time we incremented or decremented
char vary_direction = 'i';                // i = increasing; d = decreasing
boolean vary_jumped = false;              // true when we are jumpstarting the motor, less < MIN OR > MAX
unsigned int vary_min = VARY_MIN_SPEED;   // default this value
unsigned int vary_max = VARY_MAX_SPEED;   // default this value
unsigned int vary_init = VARY_INIT_SPEED; //default this value

// if this is defined as true then we just keep 
// playing the music over and over, for testig when no wires attached
// to the arduino
#define FAKE_FOB_IS_DANCING false

#define ENHANCED_STANDALONE false   // in FOB (standalone mode), plays once loud with motors and once quite w/o montors 0 delay
                                    // if false Plays with normal volume, then waits solo_delay_seconds (usualy 5) and starts it all again
                                    // normal volumn and motors on.
int fob_next_volume;

const int PM_MINI = 1;            // play mode mini track001.xxx
const int PM_LONG = 10;           // play mode long track010.xxx
const char file_type[] = "mp3";   // mp3, m4a, acc etc....


/*
 * transmit and recieve pins for the xbee.
 * Note the pins jumpers on the xbee shield are configured just the opposite
 * Tx=5, Rx=10
 */
#ifdef HAS_XBEE
const unsigned int RxD = 5;               // softserial read
const unsigned int TxD = 10;              // softserial transmit
#endif
const unsigned int delay_seconds = 20;    // number of seconds to wait to restart cycle
const int v12Switch = A0;                 // pin to turn the electronics on and off
const int v5Switch = A1;                  // pin to turn electronics on/off
const int fobA = A3;                      // input pin that the fobA button is hooked up too
const int FOB_PIN_ON = 1;                 // the FOB PIN is on (this is a momentary switch) so it just toggles fob_is_dancing;
                                          // we are using a pull_up resistor so NOT_PUSHED = 1 and PUSHED = 0
                                          // we want to connect this pin to ground through the momentary switch
                                          // The fob switch is a momentary off switch, so normally on but off when pushed.
                                          // The fob pin is also configured with an internal pull_up resistor
                                          // so it is normally closed and being pulled down to ground so it reads as 0 normally.
                                          // When the switch is pushed, ground is disconnected and it floats high to 1.
                                          // the fob toggles always dancing or network/director mode.
boolean fob_is_dancing;                   // true if the fob has been pushed so we are dancing, false otherwise

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
//int fobA_status;                  // the last read status of the keyfob
int solo_delay_seconds = 5;       // number of seconds to delay between plays
int play_mode = PM_LONG;          // default to long dance pieces, can be changed by director
int num_dances = 0;               // how many times have we danced
char track_name[] = "trackNNN.ttt"; // place holder for track...

// 1 byte to hold transmit messages
uint8_t payload[] = { 0 };
// 1 byte to hold receive messages
unsigned char new_direction;

#ifdef HAS_XBEE
// software serial port
SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX
#endif

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

void setup() {

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");

  pinMode( 2, OUTPUT);
  pinMode( 3, OUTPUT);
  pinMode( 6, OUTPUT);
  pinMode( 7, OUTPUT);
  pinMode( 8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(SD_SEL, OUTPUT);  // 4 (gravitech) or 9 (sparkfun)

  #if (GRAVITECH == 1)
    pinMode(10, OUTPUT);  // recommended in http://flashgamer.com/hardware/comments/tutorial-making-gravitechs-mp3-player-add-on-work
    Serial.println("using GRAVITECH pin setup");
  #endif

  // setup the soft serial port for xbee reads and writes
  #ifdef HAS_XBEE
    pinMode(RxD, INPUT);
    pinMode(TxD, OUTPUT);
  #endif
  pinMode(v12Switch, OUTPUT);
  digitalWrite(v12Switch, LOW);
  pinMode(v5Switch, OUTPUT);
  digitalWrite(v5Switch, LOW);
  pinMode(fobA, INPUT_PULLUP);
  
  #ifdef VARY_SPEED
    pinMode(VARY_PIN, OUTPUT);
    analogWrite(VARY_PIN, VARY_OFF);
  
    // sanity check for these values
    if (VARY_MIN_SPEED < 0) {vary_min = 0;}
    if (VARY_MAX_SPEED > 255) { vary_max = 255;}
    if (vary_min > vary_max) {vary_max = vary_min;}
    if (VARY_INIT_SPEED < 0) {vary_init = 0;}
    if (VARY_INIT_SPEED > 255) {vary_init=255;}
  
    // update the step values for init and regular steps over time
    if (vary_min < vary_max) { vary_step = ((float)vary_max - (float) vary_min) / (float) VARY_SECONDS;}
    if (vary_step <= 0) { vary_step = 0.1; }
    
    // update the step_init values for init and regular steps over time
    if (vary_init > vary_max) { vary_step_init = ((float)vary_init - (float)vary_max) / (float) VARY_SECONDS;  }
    if (vary_init < vary_min) { vary_step_init = ((float)vary_min - (float)vary_init) / (float) VARY_SECONDS;  }
    if (vary_step_init <= 0) { vary_step_init = 0.1; }
  #endif

  #ifdef HAS_XBEE
    xbeeSerial.begin(9600);
  #endif
  
  is_dancing = false;
  new_direction = empty_request;
  
  if (!sd.begin(SD_SEL, SPI_HALF_SPEED)) { 
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
  
  // default to fob mode off when we start
  fob_is_dancing = false;  
  fob_next_volume = NORMAL_VOLUME;
  if (ENHANCED_STANDALONE == true) solo_delay_seconds = 0;

  // announce we are alive!
  do_beep(200, 50);
  do_beep(200, 50); 
  do_beep(200, 30); 
  
  Serial.println(F("End setup"));
}

unsigned char xbee_read() {
  #ifdef HAS_XBEE
    return xbeeSerial.read();
  #else
    return empty_request;
  #endif
}

int xbee_available() {
  #ifdef HAS_XBEE
    return xbeeSerial.available();
  #else
    return 0;
  #endif
}

void xbee_write(unsigned char msg) {
  #ifdef HAS_XBEE
    xbeeSerial.write(msg);
  #endif
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
  if (xbee_available() > 0) {
    new_direction = xbee_read();
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
  #ifdef VARY_SPEED
    analogWrite(VARY_PIN, VARY_OFF);
    vary_current_speed = VARY_OFF;
  #endif
  #if defined IS_CHATTY
    Serial.println(F("stop_all end..."));
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
  if (fob_is_dancing) {
    #if defined IS_CHATTY
      Serial.println(F("fobA is on, skip sending dancing status to director"));
    #endif
    if (ENHANCED_STANDALONE == true) {
        MP3player.setVolume(fob_next_volume,fob_next_volume);
    }
  } else {
    xbee_write(dancing);
  }
  switch (dance_piece) {
    case solo:     my_track = 1 * play_mode; break;
    case ensembl:  my_track = 2 * play_mode; break;
    default: my_track = 0;
  }
  sprintf(track_name, "track%03d.%s", my_track, file_type);
  #if defined IS_BRIEF
    Serial.print("Starting Track:");
    Serial.println(track_name);
  #endif
  int result = MP3player.playMP3(track_name, 0);
  if(result != 0) {
    Serial.print("Error code: ");
    Serial.print(result);
    Serial.println(" when trying to play track");
  }
  delay(300); // this seems to stick sometimes when we start dancing
  if (fob_is_dancing && ENHANCED_STANDALONE && fob_next_volume == FOB_QUIET_VOLUME) {
    #if defined IS_BRIEF
      Serial.print("ENHANCED_STANDALONE=true, quiet and skipping motors this run");
    #endif
    // do nothing we don't want to start motors when fob mode and the quite play
  } else {
    digitalWrite(v12Switch, HIGH);
    digitalWrite(v5Switch, HIGH);
    #ifdef VARY_SPEED
      analogWrite(VARY_PIN, vary_init);
      delay(500);
      vary_current_speed=vary_init;
      vary_jumped = true;
      if (vary_init < vary_min ) { 
        vary_direction = 'i';
      } else {
        vary_direction = 'd';
      }
    #endif
  }
  is_dancing = true;
  dance_type = dance_piece;
  dance_started = millis();
  last_checkin = dance_started;
  #if defined IS_CHATTY
    Serial.println(F("start_dancing end"));
  #endif
}

// are we currently dancing?
void reply_status() {
  #if defined IS_BRIEF
    Serial.print("sending status: ");
  #endif
  if (is_dancing) {
    xbee_write(dancing);
    #if defined IS_BRIEF
      Serial.println("dancing");
    #endif
  } else {
    xbee_write(finished); 
    #if defined IS_BRIEF
      Serial.println("finished");
    #endif
  }
}

// is the finished playing?
boolean track_is_complete() {
  int retval;
  #if defined IS_CHATTY
    Serial.println("checking is playing...");
  #endif
  retval = MP3player.isPlaying();
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
    if (fob_is_dancing == false) {
      #if defined IS_CHATTY
        Serial.println(F("Fob is off, sending finished message to director"));
      #endif
      xbee_write(finished);
    } else {
      #if defined IS_CHATTY
        Serial.println(F("Fob is on, skipping 'finished' message to director"));
      #endif
    }
    stop_all();
    if (fob_is_dancing) {
      // simulate the sleep between tracks
      #if defined IS_CHATTY
        Serial.print(F("FobA is on, pausing for "));
        Serial.print(solo_delay_seconds);
        Serial.println(F(" between plays"));
      #endif
      delay(1000 * solo_delay_seconds);
      // if enhanced_mode, switch volume for next play
      if (ENHANCED_STANDALONE == true ) {
        if (fob_next_volume == NORMAL_VOLUME) {
          fob_next_volume = FOB_QUIET_VOLUME;
        } else {
          fob_next_volume = NORMAL_VOLUME;
        }
      }
    }
  }
  #if defined IS_CHATTY 
    else { Serial.println("track not done!"); }
  #endif
}

// are we done?
void signal_every_so_often() {

  #if defined IS_CHATTY 
    Serial.println("starting signal_every_so_often"); 
  #endif
  
  // no need to send out a signal we are on our own here
  if (fob_is_dancing) {
    return;
  }
  
  if (millis() - last_checkin > 4000) {
    #if defined IS_CHATTY
      Serial.println(F("signal_every_so_often checking in..."));
    #endif
    xbee_write(dancing);
    last_checkin = millis();
  }
}

void drain_serial() {
  // drain any outstanding input queries....
  while (xbee_available() > 0) {
    # if defined IS_CHATTY
      char x = xbee_read();
      Serial.print(F("Draining Serial got: "));
      Serial.println(x);
    #else
      Serial.println("draining...");
      xbee_read();
    #endif  
  }
}

// make a beeping sound
void do_beep(int num_millis, int delay_after) {
  Serial.println("Making a beep...");
  MP3player.setVolume(BEEP_VOLUME,BEEP_VOLUME);
  MP3player.enableTestSineWave(14);
  delay(num_millis);
  MP3player.disableTestSineWave();
  MP3player.setVolume(NORMAL_VOLUME,NORMAL_VOLUME);
  if (delay_after > 0) {
    delay(delay_after);
  }
}

void check_for_fob() {
  
  int cur_status = digitalRead(fobA);

  delay(30);    // add a short delay here, checking this seems to tweek the whole system.

  #ifdef IS_CHATTY
    Serial.print(F("Fob read returns: "));
    Serial.println(cur_status);
  #endif

  // for testing when no wires attached to arduino
  // this line tends to float back and forth so just 
  // pin it as not pushed and set indicator to say we are dancing...
  if (FAKE_FOB_IS_DANCING) {
    #ifdef IS_CHATTY
      Serial.println(F("Faking FOB"));
    #endif
    cur_status = 0;
    fob_is_dancing = true;
  }
  
  if (cur_status == FOB_PIN_ON) { // we have a switch request....
    // fob button has been pushed, reset everyting and switch mode
    #if defined IS_BRIEF
      Serial.println(F("Fob Pin Pushed, switching status..."));
    #endif
    play_mode = PM_LONG;
    stop_all();
    delay(30);

    if (fob_is_dancing) {
      fob_is_dancing = false;
      do_beep(200, 50);
      do_beep(200, 30);    
      Serial.println(F("FOB is now off..."));
    } else {
      fob_is_dancing = true;
      fob_next_volume = NORMAL_VOLUME;
      do_beep(200, 30);
      Serial.println(F("FOB is now on..."));
    }
    delay(2000);  // this keeps us from getting bouncy pin signal 
  }

  // fob is pushed override any incomming direction  
  if (fob_is_dancing) {
    drain_serial();        // don't let this build up too much...
    new_direction = empty_request;  // default to no incomming command
    // always be dancing...
    if (is_dancing == false) {
      #if defined IS_CHATTY
        Serial.println(F("fobA is on and not dancing, restarting dance"));
      #endif
      new_direction = solo;  // simulate a new request to do a solo 
    }
  }
}

// vary the speed of the motor
// This routine will continuiously vary the montor speed between min and max, 
// To obtain a constant speed set min and max to the same value
// The init speed tells us if we want to start above or below this variable range.
void do_vary_speed()
{
  unsigned long cur_millis;
  float my_step;

  // are we still in the jumped phase?
  if (vary_jumped) {
    if (vary_init < vary_min && vary_current_speed >= vary_min) { vary_jumped = false;}
    if (vary_init > vary_max && vary_current_speed <= vary_max) { vary_jumped = false;}
  }

  // choose our step size based on jumped or regular phase
  if (vary_jumped) {
    my_step = vary_step_init;
  } else {
    my_step = vary_step;
  }

  // are we increasing or decreasing...
  if (vary_direction = 'i' && vary_current_speed >= vary_max) {
    vary_direction = 'd';
  } else if (vary_direction = 'd' && vary_current_speed <= vary_min) {
    vary_direction = 'i';
  }
  
  cur_millis = millis();
  if (cur_millis < vary_prev_millis) { vary_prev_millis = cur_millis;} // handle millis wrapping

  // adjust the speed in my_step intervals
  if (cur_millis - vary_prev_millis > 1000) {
    if (vary_direction = 'i') {
      vary_current_speed += my_step;
    } else {
      vary_current_speed -= my_step;
    }
    if (vary_jumped == false) {
      if (vary_current_speed > vary_max) { vary_current_speed = vary_max; vary_direction = 'd'; }
      if (vary_current_speed < vary_min) { vary_current_speed = vary_min; vary_direction = 'i'; }      
    }
    #if defined IS_CHATTY
      Serial.print(F("do_vary_speed: vary_current_speed: "));
      Serial.print(vary_current_speed,4);
      Serial.print(F(", vary_direction: "));
      Serial.print(vary_direction);
      Serial.print(F(", mills_diff: "));
      Serial.println(cur_millis - vary_prev_millis);
    #endif
    analogWrite(VARY_PIN, (int) vary_current_speed);
    vary_prev_millis = cur_millis;
  }
}

// start and stop dancers, keep the show moving along....
void loop() {
  
  // MP3player.available();  // fill the buffer if we are polling or simpleTimer
  
  check_for_fob();
  if (fob_is_dancing == false) {
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
  
  if (is_dancing) {
    // has the music stopped? shut it down down and notify
    signal_if_done();
  }
  
  // if we are still dancing after checking music status...
  if (is_dancing) {
    signal_every_so_often();    // let the director know we are still doing something
    #ifdef VARY_SPEED
      do_vary_speed();          // if variable speed motor, vary it!
    #endif
  }
}

