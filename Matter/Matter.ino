/*
    Basic Pin setup:
    ------------                                  ---u----
    ARDUINO   13|-> SCLK (pin 25)           OUT1 |1     28| OUT channel 0
              12|                           OUT2 |2     27|-> GND (VPRG)
              11|-> SIN (pin 26)            OUT3 |3     26|-> SIN (pin 11)
              10|-> BLANK (pin 23)          OUT4 |4     25|-> SCLK (pin 13)
               9|-> XLAT (pin 24)             .  |5     24|-> XLAT (pin 9)
               8|                             .  |6     23|-> BLANK (pin 10)
               7|                             .  |7     22|-> GND
               6|                             .  |8     21|-> VCC (+5V)
               5|                             .  |9     20|-> 2K Resistor -> GND
               4|                             .  |10    19|-> +5V (DCPRG)
               3|-> GSCLK (pin 18)            .  |11    18|-> GSCLK (pin 3)
               2|                             .  |12    17|-> SOUT
               1|                             .  |13    16|-> XERR
               0|                           OUT14|14    15| OUT channel 15
    ------------                                  --------

    -  Put the longer leg (anode) of the LEDs in the +5V and the shorter leg
         (cathode) in OUT(0-15).
    -  +5V from Arduino -> TLC pin 21 and 19     (VCC and DCPRG)
    -  GND from Arduino -> TLC pin 22 and 27     (GND and VPRG)
    -  digital 3        -> TLC pin 18            (GSCLK)
    -  digital 9        -> TLC pin 24            (XLAT)
    -  digital 10       -> TLC pin 23            (BLANK)
    -  digital 11       -> TLC pin 26            (SIN)
    -  digital 13       -> TLC pin 25            (SCLK)
    -  The 2K resistor between TLC pin 20 and GND will let ~20mA through each
       LED.  To be precise, it's I = 39.06 / R (in ohms).  This doesn't depend
       on the LED driving voltage.
    - (Optional): put a pull-up resistor (~10k) between +5V and BLANK so that
                  all the LEDs will turn off when the Arduino is reset.

    If you are daisy-chaining more than one TLC, connect the SOUT of the first
    TLC to the SIN of the next.  All the other pins should just be connected
    together:
        BLANK on Arduino -> BLANK of TLC1 -> BLANK of TLC2 -> ...
        XLAT on Arduino  -> XLAT of TLC1  -> XLAT of TLC2  -> ...
    The one exception is that each TLC needs it's own resistor between pin 20
    and GND.

    This library uses the PWM output ability of digital pins 3, 9, 10, and 11.
    Do not use analogWrite(...) on these pins.

    This sketch does the Knight Rider strobe across a line of LEDs.

    Alex Leone <acleone ~AT~ gmail.com>, 2009-02-03 */

#include "SparkFun_Tlc5940.h"
#include <limits.h>
#include <math.h>

#define IS_VERBOSE false
#define SERIAL_SPEED 250000
//#define SERIAL_SPEED 19200
//#define PRINT_EVERY 100
#define PRINT_EVERY 1000      // number of millisconds to wait to print
unsigned long last_print=0;
boolean do_print=false;

unsigned int BOX1_R = 0;
unsigned int BOX1_G = 1;
unsigned int BOX1_B = 2;

#define NUM_BOXES 3  // number of boxes we are controling
#define SMD_LED_COUNT 3   // number of LED in each SMD package RGB 3 or RGBW 4
#define NUM_CHANNELS  NUM_BOXES * SMD_LED_COUNT   // total channels of lights to control

#define MAX_GRAY  4095
#define WEAK_GRAY 500

//#define NUM_INCREMENTS 200
long num_increments = 200;      // reset with each new color change

#define NUM_FIBONOCCI 9
unsigned int fibonocci_index = 0;
// unsigned int fibonicci_sequence[] = { 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144 };
//unsigned int fibonicci_sequence[] = { 1000, 1000, 2000, 3000, 5000, 8000, 13000, 21000, 34000};
unsigned int fibonicci_sequence[] = { 34000, 21000, 13000, 8000, 5000, 3000, 2000, 1000, 1000};

// values for a single color
struct color {
    long r;
    long g;
    long b;
    };

color red = { MAX_GRAY, 0, 0 };
color dimm_red = { WEAK_GRAY, 0, 0 };

color green = { 0, MAX_GRAY, 0 };
color dimm_green = { 0, WEAK_GRAY, 0 };

color blue = { 0, 0, MAX_GRAY };
color dimm_blue = { 0, 0, WEAK_GRAY };

color white = { MAX_GRAY, MAX_GRAY, MAX_GRAY };
color dimm_white = { WEAK_GRAY, WEAK_GRAY, WEAK_GRAY };

// color sequence
color color_sequence[] = {red, green, blue, white };
color color_sequence_dimm[] = {dimm_red, dimm_green, dimm_blue };
char *color_sequence_names[] = { "RED", "GREEN", "BLUE", "WHITE" };

// amount to adjust each color value to complete the sequence in the given time 
//struct adjustment { int r; int g; int b; };

// the current state of an individual light box
struct light_box { 
    // set once when initialized
    unsigned int box_id;              // sculpture number (abribrarily assigned)
    unsigned int channel_r;           // channel for red LEDs
    unsigned int channel_g;           // channel for green LEDs
    unsigned int channel_b;           // channel for blue LEDs
    // values that change for each round
    unsigned int color_index;         // index into the color list for this box
    color current_color;              // current color level
    color target_color;               // color we are headed for
    color initial_color;              // color when we started this dimm cycle
    long increment_r;         // increment for the red channel
    long increment_g;         // incrmeent for the green channel
    long increment_b;         // increment for the blue channel
    };

#define NUM_SCULPTURES 3
light_box sculpture_list[NUM_SCULPTURES];

// milliseconds / increment 
unsigned int increment_millis=0;

//// num_increments
//unsigned int num_increments = 0;

// global increment counter; when this reaches num_increments reset for the next step
unsigned int current_increment = 0;

struct dimm_cycle_t { 
        boolean  to_white_complete;         // did we finish dimming to white?
        boolean  to_color_complete;         // did we finish returning to a new color?
        unsigned long dimm_duration_ms;     // total duration of the dimm down/up (half down half up)
        unsigned long dimm_start_ms;        // when did we start the current dimm
        unsigned long current_increment;    // number of increments for this run
        unsigned long ms_per_step;          // how long to wait between increments (for longer dimms)     
        unsigned long ms_last_step;         // when did we execute our last color change
} dimm_cycle;

// average time per increment
struct { unsigned long count; unsigned long total_ms; } avg_update_time;

void update_avg_update_time(unsigned long iteration_length_ms) {

    avg_update_time.count += 1;
    avg_update_time.total_ms = avg_update_time.total_ms + (iteration_length_ms == 0 ? 1 : iteration_length_ms);

    if (do_print) {

    }
    
    // stop overflow
    if (avg_update_time.total_ms >=  (pow(2,32) - 1 ) / 2) {
      Serial.print(("AVERAGE HALF WAY TO OVERFLOW RESETTING: (pow(2,32) - 1 ) / 2="));
      Serial.print((pow(2,32) - 1 ) / 2);
      Serial.print(F(", iteration total_ms="));
      Serial.println(avg_update_time.total_ms);
  
      avg_update_time.total_ms = avg_update_time.total_ms / avg_update_time.count;
      avg_update_time.count=1;
    }
}

unsigned long get_avg_update_time() { 
  // if the average < 1 always keep it at one
  return (avg_update_time.total_ms / avg_update_time.count) < 4 ? 4 : avg_update_time.total_ms / avg_update_time.count ; 
}

void new_dimm_cycle(unsigned long current_ms) {

    // setup for our next fib sequence
    fibonocci_index += 1;
    if (fibonocci_index >= NUM_FIBONOCCI) {
      fibonocci_index = 0;
    }

    // setup the dimm_cycle info
    dimm_cycle.to_white_complete = false;
    dimm_cycle.to_color_complete = false;
    dimm_cycle.dimm_duration_ms = fibonicci_sequence[fibonocci_index] / 2;
    dimm_cycle.dimm_start_ms = current_ms;
    // increments for this cycle = total_ms / avg time to update 
    num_increments = dimm_cycle.dimm_duration_ms / get_avg_update_time() > MAX_GRAY ? MAX_GRAY : dimm_cycle.dimm_duration_ms / get_avg_update_time() ;
    dimm_cycle.current_increment = num_increments;
    dimm_cycle.ms_per_step = dimm_cycle.dimm_duration_ms / num_increments; 
    dimm_cycle.ms_last_step = 0;   


    // setup the boxes for their new target color
    for (int i=0; i<NUM_BOXES; i++){
      // set new color
      sculpture_list[i].color_index++;
      if (sculpture_list[i].color_index >= SMD_LED_COUNT) {
        sculpture_list[i].color_index = 0;
      }
      // set the new target color
      sculpture_list[i].target_color = white;
      sculpture_list[i].initial_color = sculpture_list[i].current_color;
      sculpture_list[i].increment_r = (sculpture_list[i].target_color.r - sculpture_list[i].current_color.r) / num_increments;
      sculpture_list[i].increment_g = (sculpture_list[i].target_color.g - sculpture_list[i].current_color.g) / num_increments;
      sculpture_list[i].increment_b = (sculpture_list[i].target_color.b - sculpture_list[i].current_color.b) / num_increments;
    }    
}

void reverse_dimm_cycle(unsigned long current_ms) {

    // update the num increments based on avg time to update our colors
    num_increments = dimm_cycle.dimm_duration_ms / get_avg_update_time() > MAX_GRAY ? MAX_GRAY : dimm_cycle.dimm_duration_ms / get_avg_update_time();
    // reset the cycle to the count down our number of increments
    dimm_cycle.current_increment = num_increments; 
    // track total ms need to update box
    dimm_cycle.dimm_start_ms = current_ms;

    // setup the boxes for their new target color
    for (int i=0; i<NUM_BOXES; i++) {
      // set the new target color(s)
      sculpture_list[i].target_color = color_sequence[sculpture_list[i].color_index];
      sculpture_list[i].initial_color = sculpture_list[i].current_color;
      sculpture_list[i].increment_r = (sculpture_list[i].target_color.r - sculpture_list[i].current_color.r) / num_increments;
      sculpture_list[i].increment_g = (sculpture_list[i].target_color.g - sculpture_list[i].current_color.g) / num_increments;
      sculpture_list[i].increment_b = (sculpture_list[i].target_color.b - sculpture_list[i].current_color.b) / num_increments;
    }    
}

//// make sure we don't go below 0 or above MAX_GRAY (will flicker and funk around with the lights)
//int adjust_color(int current_color, int target, int increment ) {
//  current_color += increment;
//  if (increment < 0 && current_color < target) {
//      current_color = target;
//  }
//  if (increment > 0 && current_color > target) {
//    current_color = target;
//  }
//  return current_color;
//}

// make sure we don't go below 0 or above MAX_GRAY (will flicker and funk around with the lights)
int adjust_color(int initial, int target) {
  int current_increment = dimm_cycle.current_increment;
  // current_increment is count down, so we revers the count here to get a percent done
  double percent_done = ((num_increments - current_increment) * 1.0) / (num_increments * 1.0);
  if (percent_done > 1.00) percent_done = 1.00;
  if (percent_done < 0.00) percent_done = 0.00;
  int num_values = target - initial;        // can be negative for dimm down values
  int change = num_values * percent_done;
  return initial + change;
}

// print a color status line for a single box
void box_color_status(char *msg, int box) {
  Serial.print(F("    dimm_box() "));
  Serial.print(msg);
  Serial.print(F(" final update: "));
  Serial.print(F(" BoxID="));
  Serial.print(sculpture_list[box].box_id);
  Serial.print(F(", color_name="));
  if (dimm_cycle.to_white_complete) {
    Serial.print(color_sequence_names[sculpture_list[box].color_index]);
  } else {
    Serial.print(F("WHITE"));
  }
  Serial.print(F(", color_index="));
  Serial.print(sculpture_list[box].color_index);
  Serial.print(F(", target_color=("));
  Serial.print(sculpture_list[box].target_color.r);
  Serial.print(F(","));
  Serial.print(sculpture_list[box].target_color.g);
  Serial.print(F(","));
  Serial.print(sculpture_list[box].target_color.b);
  Serial.print(F("), current_color=("));
  Serial.print(sculpture_list[box].current_color.r);
  Serial.print(F(","));
  Serial.print(sculpture_list[box].current_color.g);
  Serial.print(F(","));
  Serial.print(sculpture_list[box].current_color.b);
  Serial.print(F(")"));
  Serial.println(F(""));  
}

// adjust the color of this box and it's parameters
void dimm_box(unsigned int box) {

//  sculpture_list[box].current_color.r =  
//      adjust_color(sculpture_list[box].current_color.r, 
//                    sculpture_list[box].target_color.r, 
//                    sculpture_list[box].increment_r);
//  sculpture_list[box].current_color.g = 
//      adjust_color(sculpture_list[box].current_color.g, 
//                    sculpture_list[box].target_color.g, 
//                    sculpture_list[box].increment_g);
//  sculpture_list[box].current_color.b = 
//      adjust_color(sculpture_list[box].current_color.b, 
//                    sculpture_list[box].target_color.b, 
//                    sculpture_list[box].increment_b);

  sculpture_list[box].current_color.r =  
      adjust_color( sculpture_list[box].initial_color.r,
                    sculpture_list[box].target_color.r);
  sculpture_list[box].current_color.g = 
      adjust_color(sculpture_list[box].initial_color.g, 
                    sculpture_list[box].target_color.g);
  sculpture_list[box].current_color.b = 
      adjust_color(sculpture_list[box].initial_color.g, 
                    sculpture_list[box].target_color.b);
                    
  // make sure we reach our target on the last increment
  if (dimm_cycle.current_increment <= 0 ) {
    status("dimm_box() INCREMENT 0 REACHED");
    box_color_status("BEFORE", box);
    sculpture_list[box].current_color = sculpture_list[box].target_color;
    box_color_status("AFTER", box);
//    if (!dimm_cycle.to_white_complete) {
//      Serial.println(F("Setting current color = white"));
//      sculpture_list[box].current_color = white; 
//    } else /* second part of the loop we are tracking to color */ {
//      Serial.println(F("Setting current color = target_color"));
//      sculpture_list[box].current_color = sculpture_list[box].target_color;
//    }
  }
}

// set the given color for a box
void setBoxColourRgb(unsigned int box) {

  unsigned int channel = box * 3;

  if (do_print && IS_VERBOSE) {
    Serial.print(F("Box="));
    Serial.print(box);  
    Serial.print(F(", C_R="));
    Serial.print(channel);
    Serial.print(F(", C_G="));
    Serial.print(channel+1);
    Serial.print(F(", C_B="));
    Serial.print(channel+2);
    Serial.print(F(", R="));
    Serial.print(sculpture_list[box].current_color.r);
    Serial.print(F(", G="));
    Serial.print(sculpture_list[box].current_color.g);
    Serial.print(F(", B="));
    Serial.println(sculpture_list[box].current_color.b);    
  }

  Tlc.set(channel, MAX_GRAY - sculpture_list[box].current_color.r);
  Tlc.set(channel+1, MAX_GRAY - sculpture_list[box].current_color.g);
  Tlc.set(channel+2, MAX_GRAY - sculpture_list[box].current_color.b);
  Tlc.update();
  delay(1);
 }



void status(char *msg) {

  Serial.println(F(""));
  Serial.print("Source:"); Serial.print(msg); Serial.println(F("---------------------"));
  
  Serial.print(F("num_imcrements="));
  Serial.print(num_increments);
  Serial.println(F(""));
  
  // dimmer
  Serial.print(F("Dimmer: twc="));
  Serial.print(dimm_cycle.to_white_complete);
  Serial.print(F(", tcc="));
  Serial.print(dimm_cycle.to_color_complete);
  Serial.print(F(", dimm_duration_ms="));
  Serial.print(dimm_cycle.dimm_duration_ms);
  Serial.print(F(", current_increment="));
  Serial.print(dimm_cycle.current_increment);
  Serial.print(F(", ms_per_step="));
  Serial.print(dimm_cycle.ms_per_step);
  Serial.print(F(", ms_last_step="));
  Serial.print(dimm_cycle.ms_last_step);
  if (dimm_cycle.ms_last_step > 0) {
    Serial.print(F(", dimm_duration="));
    Serial.print(dimm_cycle.ms_last_step - dimm_cycle.dimm_start_ms);
  }
  Serial.println(F(""));

  // moving average info
  Serial.print(F("Moving Average get_avg_update_time()="));
  Serial.print(get_avg_update_time());
  Serial.print(F(", actual avg_ms="));
  Serial.print(avg_update_time.total_ms/avg_update_time.count);
  Serial.print(F(", count="));
  Serial.print(avg_update_time.count);
  Serial.print(F(", total_ms="));
  Serial.println(avg_update_time.total_ms);

  Serial.println(F(""));

  // light boxes
  for (int i=0; i<NUM_BOXES; i++) {
    Serial.print(F("BoxID="));
    Serial.print(sculpture_list[i].box_id);
    Serial.print(F(", color_name="));
    if (dimm_cycle.to_white_complete) {
      Serial.print(color_sequence_names[sculpture_list[i].color_index]);
    } else {
      Serial.print(F("WHITE"));
    }
    Serial.print(F(", Channel_R="));
    Serial.print(sculpture_list[i].channel_r);
    Serial.print(F(", Channel_G="));
    Serial.print(sculpture_list[i].channel_g);
    Serial.print(F(", Channel_B="));
    Serial.print(sculpture_list[i].channel_b);
    Serial.print(F(", color_index="));
    Serial.print(sculpture_list[i].color_index);
    Serial.print(F(", increment_r="));
    Serial.print(sculpture_list[i].increment_r);
    Serial.print(F(", increment_g="));
    Serial.print(sculpture_list[i].increment_g);
    Serial.print(F(", increment_b="));
    Serial.print(sculpture_list[i].increment_b);
    Serial.print(F(", target_color=("));
    Serial.print(sculpture_list[i].target_color.r);
    Serial.print(F(","));
    Serial.print(sculpture_list[i].target_color.g);
    Serial.print(F(","));
    Serial.print(sculpture_list[i].target_color.b);
    Serial.print(F("), current_color=("));
    Serial.print(sculpture_list[i].current_color.r);
    Serial.print(F(","));
    Serial.print(sculpture_list[i].current_color.g);
    Serial.print(F(","));
    Serial.print(sculpture_list[i].current_color.b);
    Serial.print(F(")"));
    
    Serial.println(F(""));
  }
}


void setup()
{
  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  
  Tlc.init(MAX_GRAY);
  //setColourRgb(0,0,0);
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Setup Begin"));

  sculpture_list[0].box_id = 0;
  sculpture_list[0].color_index = 2;  
  sculpture_list[0].channel_r = 0;
  sculpture_list[0].channel_g = 1;
  sculpture_list[0].channel_b = 2;
  sculpture_list[0].current_color = color_sequence[0];
  sculpture_list[0].target_color = white;
  sculpture_list[0].initial_color = sculpture_list[0].current_color;
  

  sculpture_list[1].box_id = 1;
  sculpture_list[1].color_index = 0;  
  sculpture_list[1].channel_r = 3;
  sculpture_list[1].channel_g = 4;
  sculpture_list[1].channel_b = 5;
  sculpture_list[1].current_color = color_sequence[1];
  sculpture_list[1].target_color = white;
  sculpture_list[1].initial_color = sculpture_list[1].current_color;

  sculpture_list[2].box_id = 2;
  sculpture_list[2].color_index = 1;
  sculpture_list[2].channel_r = 6;
  sculpture_list[2].channel_g = 7;
  sculpture_list[2].channel_b = 8;
  sculpture_list[2].current_color = color_sequence[2];
  sculpture_list[2].target_color = white;
  sculpture_list[2].initial_color = sculpture_list[2].current_color;


  // start at the end w/ a switch which sends us back to the start of the color cycle
  fibonocci_index = NUM_FIBONOCCI;  // will start with the first color in the cycle of sculpture 1 

  // if both of these are true; we trigger a new cycle
  dimm_cycle.to_white_complete = true;
  dimm_cycle.to_color_complete = true;

  // init the total_ms variables
  avg_update_time.count=1;
  avg_update_time.total_ms=1;  

  Serial.println(F("Setup End"));
  
}


void loop() {

  unsigned long current_ms = millis();

  if (last_print > current_ms) last_print = 0;  //c check for wrap around
  do_print=false;
  if (current_ms - last_print  >= PRINT_EVERY) {
    do_print=true;   
    last_print = current_ms;
  } 
  if (do_print) {
    status("normal status");      
  }

  // did we finish our loop? queue a new color sequence
  if (dimm_cycle.to_white_complete &&  dimm_cycle.to_color_complete ) {
    Serial.println(F("CYCLE COMPLETE, NEW CYCLE"));
    status("new dimm_cycle BEFORE update");
    new_dimm_cycle(current_ms);
    status("new dimm_cycle AFTER update");
  }

  if (current_ms - dimm_cycle.ms_last_step >= dimm_cycle.ms_per_step) {
    // update the dimm cycle
    dimm_cycle.ms_last_step = current_ms;
    dimm_cycle.current_increment--;
    // change the color on the boxes
    for (int i=0; i<NUM_BOXES; i++) {
      dimm_box(i);
      setBoxColourRgb(i);
      }

     // track on average how long it takes to run a dimm step
     unsigned long end_ms = millis();
     if (end_ms < current_ms) current_ms = 0; // handle wrap around
     update_avg_update_time(end_ms - current_ms);
  }

  // did we complete a dimming cycle
  if (dimm_cycle.current_increment <= 0) {
    if (dimm_cycle.to_white_complete == false) {
      Serial.println(F("INCREMENT=0, setting TO_WHITE_COMPLETE=1"));
      // completed first half of cycle
      dimm_cycle.to_white_complete = true;
      status("End Dimm White BEFORE reverse");
      reverse_dimm_cycle(current_ms);
      status("End Dimm White AFTER reverse");
    } else {
      Serial.println(F("INCREMENT=0, setting TO_COLOR_COMPLETE=1"));
      // completed second half of cycle
      dimm_cycle.to_color_complete = true;
      status("End Dimm Color");
    } /* else */
  } /* if */
  
}




