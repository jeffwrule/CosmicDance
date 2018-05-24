#ifndef NUCLEON_H
#define NUCLEON_H


class Nucleon {

  public:
  
    update(unsigned long current_ms);   // update the nucleon
    status(PrintTimer *pt);    // print the current dancer status
    

    Nucleon() : {

      int color1
      int color2
      int color3
      for(int i=0; i < NUM_QUARKS; i++) {

        quark_list[i].box_id = i;
        quark_list[i].color_index = i; 
        quark_list[i].channel_r = i % 3;
        quark_list[i].channel_g = (i+1) % 3 ;
        quark_list[i].channel_b = (i+2) % 3;
        quark_list[i].current_color = color_sequence[i];
        quark_list[i].target_color = white;
        quark_list[i].initial_color = quark_list[i].current_color;
  

  // init the total_ms variables
  avg_update_time.count=1;
  avg_update_time.total_ms=1;  

  Serial.println(F("Setup End"));
      }
    
    private:
      quark_list[NUM_QUARKS];

      unsigned int fibonocci_index = NUM_FIBONOCCI;
      unsigned int fibonicci_sequence[NUM_FIBONOCCI] = { 34000, 21000, 13000, 8000, 5000, 3000, 2000, 1000, 1000};
  // if both of these are true; we trigger a new cycle
  dimm_cycle.to_white_complete = true;
  dimm_cycle.to_color_complete = true;

}


// primary loop to control the lights.
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


#endif
