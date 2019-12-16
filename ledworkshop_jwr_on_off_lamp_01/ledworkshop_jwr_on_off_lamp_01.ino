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
 
//int trigPin = 11;    // Trigger
//int echoPin = 12;    // Echo
//long duration, cm, inches;


boolean is_active = true;

CRGB leds[NUM_LEDS];

// music setup
SoftwareSerial mySoftwareSerial(DFMINI_RX,DFMINI_TX); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

//
// LED COLOR SETUP
//


// set the initial and target color
CRGBPalette16 currentPalette; 
CRGBPalette16 targetPalette;
uint8_t current_brightness;
uint8_t target_brightness;

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
  
  //Define inputs and outputs
  //  pinMode(trigPin, OUTPUT);
  //  pinMode(echoPin, INPUT);

  pinMode(IR_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  is_active = true;

//  current_brightness = color_brightness[0];
  target_brightness = current_brightness;
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness( current_brightness );
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  FastLED.delay(100);
  for (uint8_t i=0; i<NUM_COLORS; i++) { palette_list[i] =  CRGBPalette16(colors[i]); }
  ChangeColorOverTime(0);
  
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

// used with Sonic board
//void test_on_off() {
//
//  static unsigned long last_ms=0;
//  unsigned long cur_ms = millis();
//  if (cur_ms < last_ms) last_ms = 0;
//
//  if (cur_ms - last_ms < 2000) return;
//  
//  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
//  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
//  digitalWrite(trigPin, LOW);
//  delayMicroseconds(5);
//  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(trigPin, LOW);
// 
//  // Read the signal from the sensor: a HIGH pulse whose
//  // duration is the time (in microseconds) from the sending
//  // of the ping to the reception of its echo off of an object.
//  pinMode(echoPin, INPUT);
//  duration = pulseIn(echoPin, HIGH);
// 
//  // Convert the time into a distance
//  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
//  inches = (duration/2) / 74;   // Divide by 74 or multiply by 0.0135
//  
////  Serial.print(inches);
////  Serial.print("in, ");
////  Serial.print(cm);
////  Serial.print("cm");
////  Serial.println();
//
//  if (inches <= 20) {
//    is_active = !is_active;
//    last_ms = cur_ms;
//    Serial.print(F("is_active triggered: "));
//    Serial.print(is_active);
//    Serial.print(F(", cm="));
//    Serial.print(cm);
//    Serial.println(F(""));
//  }
//}

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

void adjust_brightness() {
  static uint8_t new_level;
  if (current_brightness == target_brightness) return;

  if (current_brightness < target_brightness ) {
    new_level = current_brightness + COLOR_BRIGHTNESS_CHANGE_RATE;
    current_brightness = new_level < current_brightness ? 255 : new_level;
  }  else {
    new_level = current_brightness - COLOR_BRIGHTNESS_CHANGE_RATE;
    current_brightness = new_level > current_brightness ? 0 : new_level;
  }
  FastLED.setBrightness( current_brightness );
}

void adjust_lights() {
  static uint8_t startIndex = 0;

  ChangeColorOverTime(HOLD_COLOR_MS);

  EVERY_N_MILLISECONDS(  1000 / UPDATES_PER_SECOND ) {
    adjust_brightness();
    nblendPaletteTowardPalette( currentPalette, targetPalette, MAX_CHANGES);
    FillLEDsFromPaletteColors( startIndex); 
    if (!target_reached) {
      if (!color_reached && leds[0].r == targetPalette[0].r  && leds[0].g == targetPalette[0].g  && leds[0].b == targetPalette[0].b) {
        Serial.print(F("%%% COLOR REACHED : "));
        print_color_control();
        Serial.print(F(", target_reached: "));
        Serial.print(target_reached);
        Serial.print(F(", color_count: "));
        Serial.print(color_count);
        Serial.print(F(", "));
        Serial.println(millis() - color_change_start_ms); 
        Serial.println();
        color_reached = true;        
      }
      if (!brightness_reached && current_brightness == target_brightness) {
        Serial.print(F("%%% BRIGHTNESS REACHED : "));
        Serial.print(F(", current_brightness: "));
        Serial.print(current_brightness);
        Serial.print(F(", target_brightness: "));
        Serial.print(target_brightness);
        Serial.print(F(", diff: "));
        Serial.println(millis() - color_change_start_ms); 
        Serial.println();
        brightness_reached = true;         
      }

      if (brightness_reached && color_reached) {
        Serial.print(F("%%% TARGET REACHED: diff: "));  
        Serial.println(millis() - color_change_start_ms); 
        Serial.println();  
        target_reached = true;    
      }
      color_count += 1;      
    } 
  }
  
  // startIndex = startIndex + 1; /* motion speed */
  FastLED.show();
  FastLED.delay(10);  
}

void printCRGB(CRGB input) {
  Serial.print("0x");
  for (int i=0; i<3; i++){
    char tmp[3];
    sprintf(tmp, "%02X", input[i]);
    Serial.print(tmp);
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 0; i < NUM_LEDS; i++, colorIndex++) {
    leds[i] = ColorFromPalette( currentPalette, i, brightness);
  }
}

void print_color_control() {
  Serial.print(F("Curr Colors led: "));
  printCRGB(leds[0]);
  Serial.print(F(", curr: "));
  printCRGB(currentPalette[0]);
  Serial.print(F(", target: "));
  printCRGB(targetPalette[0]);
}

void ChangeColorOverTime(unsigned long delay_ms) {
  
    static uint8_t palette_index=NUM_COLORS;   // index into our list of ordered colors to change to
    static unsigned long last_ms=0;   // milliseconds the last time we switched
    unsigned long cur_ms = millis();  // current milliseconds

    if (cur_ms < last_ms) last_ms=0;

    if (cur_ms - last_ms >= delay_ms) {
      Serial.print(F("%%%% Color Change: "));
      Serial.print(F("cur_ms: "));
      Serial.print(cur_ms);
      Serial.print(F(", last_ms: "));
      Serial.print(last_ms);
      Serial.print(F(", diff: "));
      Serial.println(last_ms - cur_ms);      
      palette_index += 1;
      palette_index = palette_index < NUM_COLORS ? palette_index : 0;
      if (palette_index >= NUM_COLORS) {
        palette_index = 0;
      }
      targetPalette = palette_list[palette_index];
      target_brightness = color_brightness[palette_index];
      last_ms=cur_ms;
      color_count=0;
      target_reached=false;
      color_reached=false;
      brightness_reached=false;
      color_change_start_ms = cur_ms;
      Serial.print(F("    new color: "));
      Serial.print(color_names[palette_index]);
      Serial.print(F(", "));
      print_color_control();
      Serial.println();
      Serial.print(F("    current_brightness: "));
      Serial.print(current_brightness);
      Serial.print(F(", target_brightness: "));
      Serial.print(target_brightness);
      Serial.print(F(", target_reached: "));
      Serial.print(target_reached);
      Serial.print(F(", color_reached: "));
      Serial.print(color_reached);
      Serial.print(F(", color_reached: "));
      Serial.print(target_reached);
      Serial.println();    
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
      adjust_lights();
  }

}
