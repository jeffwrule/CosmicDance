#ifndef VARYSPEEDDEVICE_H
#define VARYSPEEDDEVICE_H

/// setup()
    Serial.println(F("Using Varying Speed Setup"));

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


// in stop_all() dance, now needs to be here

    analogWrite(VARY_PIN, VARY_OFF);
    vary_current_speed = VARY_OFF;
// stop_all end



#endif
