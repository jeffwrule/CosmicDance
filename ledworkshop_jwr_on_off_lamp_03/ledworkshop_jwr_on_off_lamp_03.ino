/*
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */
 
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <FastLED.h>
#include "config.h"
 

boolean is_active = true;

// music setup
SoftwareSerial mySoftwareSerial(DFMINI_RX,DFMINI_TX); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


unsigned long color_count=0;
bool target_reached=false;
bool color_reached=false;
bool brightness_reached=false;
unsigned long color_change_start_ms=0;

void setup() {
  delay(1000); // sanity delay
  //Serial Port begin
  Serial.begin(250000);
  Serial.println();
  Serial.print(F("LEDWorkshop_on_off_lamp "));
  Serial.println(SKETCH_ID);


  pinMode(IR_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  is_active = true;


  
  if (HAS_MUSIC) {
    // setup communications with the MP3 card
    mySoftwareSerial.begin(9600);
    Serial.println();
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true);
    }
    Serial.println(F("DFPlayer Mini online."));
    delay(100);
    myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
    myDFPlayer.volume(DFMINI_VOLUME);  //Set volume value. From 0 to 30
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
    myDFPlayer.play(1);  //Play the first mp3 
    myDFPlayer.enableLoop();  
  } else {
    Serial.println(F("Music Configured OUT!"));
  }

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( BRIGHTNESS );
  
  Serial.println(F("Setup complete"));
  Serial.println();
  
}

// print detail messages about what is wrong with the music.
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}


// routine for osoyoo IR device  (bump avoidance)
void test_on_off() {
    static unsigned long last_ms;
    unsigned long cur_ms = millis();
    if (cur_ms < last_ms) last_ms = 0;
    if (cur_ms - last_ms < 2000) return;

    int num_reads=0;    
    for (int i=0; i<3; i++) {
      num_reads += !digitalRead(IR_PIN);
    }
    
    if (num_reads >= 2) {
      is_active = !is_active;   // switch active state
      last_ms = cur_ms;
      Serial.print(F("is_active triggered: "));
      Serial.print(is_active);
      Serial.print(F(", num_reads=")); 
      Serial.println(num_reads);
      }
}

void flash_leds(uint16_t num_flashes) {
  static CRGB old = leds[5];
  int num_flash_leds;

  num_flash_leds = constrain(5, 0, NUM_LEDS);

  for (int i=0; i<num_flash_leds; i++) {
    old[i] = leds[i];
  }

  for (int i=0; i<num_flashes; i++) {
    fill_solid(leds, num_flash_leds, CRGB::Black);
    FastLED.show();
    delay(100);
    fill_solid(leds, num_flash_leds, CRGB::White);
    FastLED.show();
    delay(100);
  }

  for (int i=0; i<num_flash_leds; i++) {
    leds[i] = old[i];
  }
}



void printCRGB(CRGB input) {
  Serial.print("0x");
  for (int i=0; i<3; i++){
    char tmp[3];
    sprintf(tmp, "%02X", input[i]);
    Serial.print(tmp);
  }
}


// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


void Fire2012()
{
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void loop() {

  static boolean is_off=true;
  static int      i=0;

  EVERY_N_MILLISECONDS(100) {
      test_on_off();
  }

  if (is_off != is_active) {
    if (is_active) {
      digitalWrite(LED_BUILTIN, HIGH);
      flash_leds(1);
      Serial.println(F("Light ON"));
      if (HAS_MUSIC) {
        myDFPlayer.play(1);   // play the first mp3 file on repeat
      }
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      flash_leds(2);
      Serial.println(F("Light OFF"));
      if (HAS_MUSIC) {
        myDFPlayer.pause();   // play the first mp3 file on repeat            
      }
    }
    is_off=is_active;
  }


  if (HAS_MUSIC) {
    EVERY_N_MILLISECONDS(500) {
      if (myDFPlayer.available()) {
        printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
      }
    }
  }

  // when system is active run the lights.
  if (is_active) {
        Fire2012(); // run simulation frame
  }

  FastLED.show(); // display this frame
  FastLED.delay(1000 / FRAMES_PER_SECOND);

}
