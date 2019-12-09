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

#include <FastLED.h>
 
//int trigPin = 11;    // Trigger
//int echoPin = 12;    // Echo
//long duration, cm, inches;

#define IR_PIN 11

boolean is_active = true;

#define NUM_LEDS 60
#define LED_PIN 10
#define LED_TYPE WS2811
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

#define NUM_COLORS 3
CRGB colors[NUM_COLORS] = { CRGB::BlueViolet, CRGB::DarkRed, CRGB::Black };
uint8_t to_color=0;
 
void setup() {
  delay(3000); // sanity delay
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
//  pinMode(trigPin, OUTPUT);
//  pinMode(echoPin, INPUT);

  pinMode(IR_PIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  is_active = true;

  FastLED.addLeds<LED_TYPE,LED_PIN, COLOR_ORDER>(leds,NUM_LEDS);
  FastLED.setBrightness(180);
  to_color=1;
  fill_solid(leds, NUM_LEDS, colors[0]);
  FastLED.show();

  for (int i=0; i<NUM_COLORS; i++) {
    Serial.print(F("Color: "));
    Serial.print(i);
    Serial.print(", ");
    Serial.print(colors[i].r);
    Serial.print(", ");
    Serial.print(colors[i].g);
    Serial.print(", ");
    Serial.println(colors[i].b);
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

// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8( uint8_t& cur, const uint8_t target, uint8_t amount)
{
  if( cur == target) return;
  
  if( cur < target ) {
    uint8_t delta = target - cur;
    delta = scale8_video( delta, amount);
    cur += delta;
  } else {
    uint8_t delta = cur - target;
    delta = scale8_video( delta, amount);
    cur -= delta;
  }
}

// Blend one CRGB color toward another CRGB color by a given amount.
// Blending is linear, and done in the RGB color space.
// This function modifies 'cur' in place.
CRGB fadeTowardColor( CRGB& cur, const CRGB& target, uint8_t amount)
{
  nblendU8TowardU8( cur.red,   target.red,   amount);
  nblendU8TowardU8( cur.green, target.green, amount);
  nblendU8TowardU8( cur.blue,  target.blue,  amount);
  return cur;
}

// Fade an entire array of CRGBs toward a given background color by a given amount
// This function modifies the pixel array in place.
void fadeTowardColor( CRGB* L, uint16_t N, const CRGB& bgColor, uint8_t fadeAmount)
{
  for( uint16_t i = 0; i < N; i++) {
    fadeTowardColor( L[i], bgColor, fadeAmount);
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
  FastLED.show();
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
    } else {
      digitalWrite(LED_BUILTIN, LOW);
      flash_leds(2);
      Serial.println(F("Light OFF"));
    }
    is_off=is_active;
  }

  if (!is_active) to_color=2;
  
  // fade all existing pixels toward bgColor by "5" (out of 255)
  fadeTowardColor( leds, NUM_LEDS, colors[to_color], 10);

//  i += 1;
//  Serial.print(F("    Transition: "));
//  Serial.print(i);
//  Serial.print(F(", "));
//  Serial.print(leds[0].r);
//  Serial.print(F(", "));
//  Serial.print(leds[0].g);
//  Serial.print(F(", "));
//  Serial.println(leds[0].b);
        
  if (is_active) {
    // periodically set random pixel to a random color, to show the fading
    EVERY_N_MILLISECONDS( 2000 ) {
      if (leds[0] == colors[to_color]) {
        Serial.println("Color Switch");
        to_color += 1;
        if (to_color >= NUM_COLORS-1) to_color=0;
        Serial.print(to_color);
        Serial.print(", ");
        Serial.print(colors[to_color].r);
        Serial.print(", ");
        Serial.print(colors[to_color].g);
        Serial.print(", ");
        Serial.println(colors[to_color].b);
        i=0;
      }
    }
  }

  FastLED.show();
  FastLED.delay(10);

}
