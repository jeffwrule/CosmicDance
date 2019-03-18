#ifndef NUCLEON_H
#define NUCLEON_H

#include "PrintTimer.h"
#include "Nucleon_config.h"

class Nucleon {

  public:
  
    void update(PrintTimer *pt);   // update the nucleon
    status(PrintTimer *pt, char *msg);    // print the current dancer status
    
    Nucleon(PrintTimer *pt, color color1, color color2, color color3, char *colorname1, char *colorname2, char *colorname3) {

      color_sequence[0] = color1;
      color_sequence[1] = color2;
      color_sequence[2] = color3;

      color_sequence_names[0] = colorname1;
      color_sequence_names[1] = colorname2;
      color_sequence_names[2] = colorname3;
      color_sequence_names[3] = "WHITE";
      
      for(int i=0; i < NUM_QUARKS; i++) {
        quark_list[i].quark_id = i;
        quark_list[i].color_index = i; 
        quark_list[i].channel_r = SMD_LED_COUNT*i;
        quark_list[i].channel_g = SMD_LED_COUNT*i+1 ;
        quark_list[i].channel_b = SMD_LED_COUNT*i+2;
        quark_list[i].current_color = white;
      }
      

      // setup to trigger a new cycle on the first run
      dimm_cycle.to_color_complete = true;
      dimm_cycle.to_white_complete = true;

      // initlaize the avg counter
      avg_update_time.count=1;
      avg_update_time.total_ms=1; 

      for (int quark=0; quark<NUM_QUARKS; quark++) {
              setQuarkColourRgb(pt, quark);
      }
    }
    
    private:
      quark quark_list[NUM_QUARKS];
      dimm_cycle_t dimm_cycle;

      unsigned int fibonocci_index = NUM_FIBONOCCI;

      // color sequence
      color color_sequence[4] = {red, green, blue, white };
      //color color_sequence_dimm[] = {dimm_red, dimm_green, dimm_blue };
      char *color_sequence_names[4];

      //#define NUM_INCREMENTS 200
      long num_increments = 200;      // reset with each new color change

      avg_update_time_t avg_update_time;

      // NOTE: this routine is device specific and will be implemented 
      // for each device in the AAA_xxx.h files
      void setQuarkColourRgb(PrintTimer *pt, unsigned int quark);

      
      void new_dimm_cycle(PrintTimer *pt, unsigned long current_ms);
      void dimm_quark(PrintTimer *pt, unsigned int quark);
      void reverse_dimm_cycle(unsigned long current_ms);
      void quark_color_status(PrintTimer *pt, char *msg, int quark);
      int  adjust_color(int initial, int target, float color_factor);
      void update_avg_update_time(unsigned long iteration_length_ms);
      unsigned long get_avg_update_time();

};

Nucleon::status(PrintTimer *pt,  char *msg) {

  
  if (!pt->get_do_print()) return;
  
  Serial.println(F(""));
  Serial.print("Source:"); Serial.print(msg); Serial.println(F("---------------------"));

  pt->status();
  Serial.println(F(""));

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

  // quarks
  for (int i=0; i<NUM_QUARKS; i++) {
    Serial.print(F("QuarkID="));
    Serial.print(quark_list[i].quark_id);
    Serial.print(F(", target_color_name="));
    if (!dimm_cycle.to_color_complete) {
      Serial.print(color_sequence_names[quark_list[i].color_index]);
    } else {
      Serial.print(color_sequence_names[3]);
    }
    Serial.print(F(", Channel_R="));
    Serial.print(quark_list[i].channel_r);
    Serial.print(F(", Channel_G="));
    Serial.print(quark_list[i].channel_g);
    Serial.print(F(", Channel_B="));
    Serial.println(quark_list[i].channel_b);
    Serial.print(F("    color_index="));
    Serial.print(quark_list[i].color_index);
    Serial.print(F(", increment_r="));
    Serial.print(quark_list[i].increment_r);
    Serial.print(F(", increment_g="));
    Serial.print(quark_list[i].increment_g);
    Serial.print(F(", increment_b="));
    Serial.print(quark_list[i].increment_b);
    Serial.print(quark_list[i].initial_color.r);
    Serial.print(F(","));
    Serial.print(quark_list[i].initial_color.g);
    Serial.print(F(","));
    Serial.print(quark_list[i].initial_color.b);
    Serial.print(F("), target_color=("));
    Serial.print(quark_list[i].target_color.r);
    Serial.print(F(","));
    Serial.print(quark_list[i].target_color.g);
    Serial.print(F(","));
    Serial.print(quark_list[i].target_color.b);
    Serial.print(F("), current_color=("));
    Serial.print(quark_list[i].current_color.r);
    Serial.print(F(","));
    Serial.print(quark_list[i].current_color.g);
    Serial.print(F(","));
    Serial.print(quark_list[i].current_color.b);
    Serial.print(F(")"));
    
    Serial.println(F(""));
  }
}


// primary loop to control the lights.
void Nucleon::update(PrintTimer *pt) {

  unsigned long current_ms = pt->current_millis;

  // print a status every so often
  status(pt, "normal status");      

  // did we finish our loop? queue a new color sequence
  if (dimm_cycle.to_white_complete &&  dimm_cycle.to_color_complete ) {
    pt->print_now();
    Serial.println(F("CYCLE COMPLETE, NEW CYCLE"));
    status(pt, "new dimm_cycle BEFORE update");
    new_dimm_cycle(pt, current_ms);
    status(pt, "new dimm_cycle AFTER update");
  }

  // time to step down the dimm cycle? and send out a new color
  if (current_ms - dimm_cycle.ms_last_step >= dimm_cycle.ms_per_step) {
    
    // update the dimm cycle
    dimm_cycle.ms_last_step = current_ms;
    dimm_cycle.current_increment--;
    
    // change the color on the boxes
    for (int i=0; i<NUM_QUARKS; i++) {
      dimm_quark(pt, i);        // adjust the brightness/color levels
      setQuarkColourRgb(pt, i); // write the new color out
      }

    // track on average how long it takes to run a dimm step
    unsigned long end_ms = millis();
    if (end_ms < current_ms) current_ms = 0; // handle wrap around
    update_avg_update_time(end_ms - current_ms);
  }

  // did we complete a dimming cycle
  if (dimm_cycle.current_increment <= 0) {
    pt->print_now();
    
    if (dimm_cycle.to_color_complete == false) {  // first half of cycle is dimm to color
      Serial.println(F("INCREMENT=0, setting TO_COLOR_COMPLETE=1"));
      // completed first half of cycle
      dimm_cycle.to_color_complete = true;
      dimm_cycle.to_white_complete = true;  // this short circuits the whole thing and only does one of each cycle
      status(pt, "End Dimm color BEFORE reverse");
      reverse_dimm_cycle(current_ms);
      status(pt, "End Dimm color AFTER reverse");
    } else { // second half of cycle is dimm back to white 
      Serial.println(F("INCREMENT=0, setting TO_WHITE_COMPLETE=1"));
      // completed second half of cycle
      dimm_cycle.to_white_complete = true;
      status(pt, "End Dimm white");
    } /* else */
  } /* if */
  
}

void Nucleon::update_avg_update_time(unsigned long iteration_length_ms) {

    avg_update_time.count += 1;
    avg_update_time.total_ms = avg_update_time.total_ms + (iteration_length_ms == 0 ? 1 : iteration_length_ms);
    
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

unsigned long Nucleon::get_avg_update_time() { 
  // if the average < LED_UPDATE_DELAY + 3 return LED_UPDATE_DELAY + 3
  return (avg_update_time.total_ms / avg_update_time.count) < LED_UPDATE_DELAY + 3 ? LED_UPDATE_DELAY + 3 : avg_update_time.total_ms / avg_update_time.count ; 
}

// called to set a new color 
// we are going to set a new solid if reset, then we are going to dimm to white
void Nucleon::new_dimm_cycle(PrintTimer *pt, unsigned long current_ms) {

    boolean new_color = false;
    
    // setup for our next fib sequence
    fibonocci_index += 1;
    if (fibonocci_index >= NUM_FIBONOCCI) {
      fibonocci_index = 0;
    }

    // setup the dimm_cycle info
    dimm_cycle.to_color_complete = false;
    dimm_cycle.to_white_complete = false;
    dimm_cycle.dimm_duration_ms = fibonicci_sequence[fibonocci_index] / 2;
    dimm_cycle.dimm_start_ms = current_ms;
    // increments for this cycle = total_ms / avg time to update 
    num_increments = dimm_cycle.dimm_duration_ms / get_avg_update_time();
    if (num_increments > MAX_GRAY ) num_increments = MAX_GRAY;
    dimm_cycle.num_increments = num_increments;
    dimm_cycle.current_increment = num_increments;
    dimm_cycle.ms_per_step = dimm_cycle.dimm_duration_ms / num_increments; 
    dimm_cycle.ms_last_step = 0;   

    // setup the boxes for their new target color
    for (int i=0; i<NUM_QUARKS; i++){

      // set the new color the next to the last item in the sequence
      if (fibonocci_index+1 == NUM_FIBONOCCI) {
        quark_list[i].color_index++;      
        if (quark_list[i].color_index >= SMD_LED_COUNT) {
          quark_list[i].color_index = 0;
        }  
        Serial.print(F("NEW COLOR FOR QUARKID: "));
        Serial.print(i);
        Serial.print(F(", color="));
        Serial.println(color_sequence_names[quark_list[i].color_index]);    
//          Serial.println("WHITE");  
        new_color = true;  
      }

      // current color always jumps back to full color
      quark_list[i].current_color = color_sequence[quark_list[i].color_index];

      // set the new target color
      quark_list[i].target_color = white;
      quark_list[i].initial_color = quark_list[i].current_color;
      quark_list[i].increment_r = (quark_list[i].target_color.r - quark_list[i].current_color.r) / num_increments;
      quark_list[i].increment_g = (quark_list[i].target_color.g - quark_list[i].current_color.g) / num_increments;
      quark_list[i].increment_b = (quark_list[i].target_color.b - quark_list[i].current_color.b) / num_increments;
    }    

    if (new_color) {
      // change the color on the boxes
      for (int i=0; i<NUM_QUARKS; i++) {
        setQuarkColourRgb(pt, i); // write the new color out
      }
      Serial.println(F("COLOR CHANGE DELAY..."));
      delay(INIT_DELAY_MS);
      dimm_cycle.to_color_complete = true;
      dimm_cycle.to_white_complete = true;
    }

}

// we are going to set solid color here then dimm to white
void Nucleon::reverse_dimm_cycle(unsigned long current_ms) {

    // update the num increments based on avg time to update our colors
    num_increments = dimm_cycle.dimm_duration_ms / get_avg_update_time();
    if (num_increments > MAX_GRAY) num_increments = MAX_GRAY;
    
    // reset the cycle to the count down our number of increments
    dimm_cycle.current_increment = num_increments; 
    // track total ms need to update box
    dimm_cycle.dimm_start_ms = current_ms;

    // setup the boxes for their new target color
    for (int i=0; i<NUM_QUARKS; i++) {
      // current color always jumps back to full color
      quark_list[i].current_color = color_sequence[quark_list[i].color_index];
      
      // set the new target color(s)
      quark_list[i].target_color = white;
      quark_list[i].initial_color = quark_list[i].current_color;
      quark_list[i].increment_r = (quark_list[i].target_color.r - quark_list[i].current_color.r) / num_increments;
      quark_list[i].increment_g = (quark_list[i].target_color.g - quark_list[i].current_color.g) / num_increments;
      quark_list[i].increment_b = (quark_list[i].target_color.b - quark_list[i].current_color.b) / num_increments;
    }    
}

// make sure we don't go below 0 or above MAX_GRAY (will flicker and funk around with the lights)
int Nucleon::adjust_color(int initial, int target, float color_factor) {
  int current_increment = dimm_cycle.current_increment;

  // current_increment is count down, so we revers the count here to get a percent done
  double percent_done = ((num_increments - current_increment) * color_factor) / (num_increments * 1.0);
  if (percent_done > 1.00) percent_done = 1.00;
  if (percent_done < 0.00) percent_done = 0.00;
  int num_values = target - initial;        // can be negative for dimm down values
  int change = num_values * percent_done;
  return initial + change;
}

// print a color status line for a single box
void Nucleon::quark_color_status(PrintTimer *pt, char *msg, int quark) {

  if (!pt->get_do_print()) return;
  
  Serial.print(F("    dimm_quark() "));
  Serial.print(msg);
  Serial.print(F(" final update: "));
  Serial.print(F(" QuarkID="));
  Serial.print(quark_list[quark].quark_id);
  Serial.print(F(", target_color_name="));
//  if (!dimm_cycle.to_color_complete) {
    Serial.print(color_sequence_names[quark_list[quark].color_index]);
//  } else {
//    Serial.print(F("WHITE"));
//  }
  Serial.print(F(", color_index="));
  Serial.print(quark_list[quark].color_index);
  Serial.print(F(", initial_color=("));
  Serial.print(quark_list[quark].initial_color.r);
  Serial.print(F(","));
  Serial.print(quark_list[quark].initial_color.g);
  Serial.print(F(","));
  Serial.print(quark_list[quark].initial_color.b);
  Serial.print(F("), target_color=("));
  Serial.print(quark_list[quark].target_color.r);
  Serial.print(F(","));
  Serial.print(quark_list[quark].target_color.g);
  Serial.print(F(","));
  Serial.print(quark_list[quark].target_color.b);
  Serial.print(F("), current_color=("));
  Serial.print(quark_list[quark].current_color.r);
  Serial.print(F(","));
  Serial.print(quark_list[quark].current_color.g);
  Serial.print(F(","));
  Serial.print(quark_list[quark].current_color.b);
  Serial.print(F(")"));
  Serial.println(F(""));  
}

// adjust the color of this box and it's parameters
void Nucleon::dimm_quark(PrintTimer *pt, unsigned int quark) {

  float adjust_factor = 1.0;

  if (dimm_cycle.current_increment > dimm_cycle.num_increments/2) { 
    if (color_sequence_names[quark_list[quark].color_index] == COLOR_FACTOR_COLOR) {
      adjust_factor = COLOR_FACTOR;
    }
  }
  
  quark_list[quark].current_color.r =  
      adjust_color( quark_list[quark].initial_color.r,
                    quark_list[quark].target_color.r, adjust_factor);
  quark_list[quark].current_color.g = 
      adjust_color(quark_list[quark].initial_color.g, 
                    quark_list[quark].target_color.g, adjust_factor);
  quark_list[quark].current_color.b = 
      adjust_color(quark_list[quark].initial_color.b, 
                    quark_list[quark].target_color.b,  adjust_factor);
                    
  // make sure we reach our target on the last increment
  if (dimm_cycle.current_increment <= 0 ) {
    status(pt, "dimm_quark() INCREMENT 0 REACHED");
    quark_color_status(pt, "BEFORE", quark);
    quark_list[quark].current_color = quark_list[quark].target_color;
    quark_color_status(pt, "AFTER", quark);
  }
}



#endif
