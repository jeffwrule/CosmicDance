
#include <TimerOne.h>

class ZeroCross {

  public:
    
  ZeroCross( unsigned char pin_list[],  
      unsigned char num_pins, 
      unsigned char zero_pin) :
    pin_list(pin_list),
    num_pins(num_pins),
    zero_pin(zero_pin) {
      
    }
    
}
unsigned const char zero_cross_pin = 2; // signal back to arduino each time the power cycles
unsigned const char channel_1 = 4;  // Output to Opto Triac pin, channel 1
unsigned const char channel_2 = 5;  // Output to Opto Triac pin, channel 2
unsigned const char channel_3 = 6;  // Output to Opto Triac pin, channel 3
unsigned const char channel_4 = 7;  // Output to Opto Triac pin, channel 4
unsigned const char channel_5 = 8;  // Output to Opto Triac pin, channel 5
unsigned const char channel_6 = 9;  // Output to Opto Triac pin, channel 6
unsigned const char channel_7 = 10; // Output to Opto Triac pin, channel 7
unsigned const char channel_8 = 11; // Output to Opto Triac pin, channel 8
volatile unsigned char levels[8] = {0, 0, 0, 0, 0, 0, 0, 0 }; // the current dim level for each channel
volatile unsigned char channel_map[8] = {channel_1, channel_2, channel_3, channel_4, channel_5, channel_6, channel_7, channel_8 }; // map of channels to pins  
volatile unsigned char clock_tick; // variable for Timer1
volatile unsigned char max_clock_tick=0; // what was the max ticks we have seen
unsigned int delay_time = 40;

//unsigned const char low = 75;
////unsigned const char high = 5;
//unsigned const char high = 50;
//unsigned const char off = 80;


unsigned const char low = 160;
//unsigned const char high = 5;
unsigned const char high = 100; // 0 acutally, but 100 is bright enough for our applicaiton
unsigned const char off = 170;




void setup() {

  for (int i=0; i<8; i++) {
    pinMode(channel_map[i], OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(zero_cross_pin), zero_crosss_int, RISING);
  Timer1.initialize(41); // set a timer of length 100 microseconds for 50Hz or 83 microseconds for 60Hz;
                         // this setting means we call timeIsr routine 100 times during a signle 60Hz or 50Hz cycle
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here

  Serial.begin(250000);

}

void timerIsr()
{
  clock_tick++;

  boolean is_high = false;  // set to true when one or more triacs where set high 
  boolean high_gate[8] = { false, false, false, false, false, false, false, false };

  for (int i=0; i<8; i++) {
    if (levels[i] == clock_tick) {
      digitalWrite(channel_map[i], HIGH);
      is_high = true;
      high_gate[i] = true;
    }
  }

  if (is_high == true ) {
    delayMicroseconds(5);   // let all the set gates lock
    for (int i=0; i<8; i++) {
      if (high_gate[i] == true ) {
        digitalWrite(channel_map[i], LOW);
      }
    }
  }
}



void zero_crosss_int() // function to be fired at the zero crossing to dim the light
{
  // Every zerocrossing interrupt: For 50Hz (1/2 Cycle) => 10ms ; For 60Hz (1/2 Cycle) => 8.33ms
  // 10ms=10000us , 8.33ms=8330us
  max_clock_tick = clock_tick;
  clock_tick=0;
}

void loop() {

  Serial.println("Dimming up");
  for (int i=low;i>=high;i--) {
    for(int j=0; j<8; j++ ) {
      levels[j] = i;
    }
    Serial.print(i);
    Serial.print(" , ");
    Serial.println(max_clock_tick);
    delay(delay_time);
  }

  Serial.println("Dimming down");
  for (int i=high;i<=low;i++) {
    for (int j=0; j<8; j++ ) {
      levels[j] = i;
    }
    Serial.print(i);
    Serial.print(" , ");
    Serial.println(max_clock_tick);
    delay(delay_time);
  }
}
