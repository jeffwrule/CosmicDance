#include "config.h"
#include "InterruptDev.h"
#include "ZeroCrossDimmer.h"
#include "Light.h"
#include "Playbook.h"
#include "Motor.h"



// config info for this app
//uint8_t pin_list[NUM_DIMMERS] = { 6,7,8,9,10,11 };     // current pin mappings for each of the dimmers
uint8_t pin_list[NUM_DIMMERS] = { 6,7,8,  };     // current pin mappings for each of the dimmers
uint32_t triac_lock_delay=5;                            // how long (micro seconds) does the triac on this board take to lock, typical 5-20us start with 5

Light  *light_levels[NUM_LIGHT_LEVELS]; // how many physical light leves are we mapping to our dimmers
Light  *lights[NUM_DIMMERS];            // the physical lights dimmer
ZeroCrossDimmer *zdimmer;               // our active dimmer code
InterruptDev *position_dev_list[NUM_POSITION_PINS];   // list of position devices
InterruptDev *on_off_dev;               // interrupts when the on off button is pushed
Motor *motor;                           // the dcmotor to stop and start as described in the playbook
Playbook *play_list;                    // this feeds the dimmer with steps and watches inputs for changes

void setup() {

  // dump the config values
  Serial.begin(250000);
  Serial.println(F("Hashtag: version: 2019-11-28 v1"));
  Serial.print(F("NUM_DIMMERS: "));
  Serial.println(NUM_DIMMERS);
  Serial.print(F("ZERO_CROSS_PIN: "));
  Serial.println(ZERO_CROSS_PIN);
  Serial.print(F("ON_OFF_PIN: "));
  Serial.println(ON_OFF_PIN);
  Serial.print(F("MOTOR_PIN: "));
  Serial.println(MOTOR_PIN);
  Serial.print(F("triac_lock_delay: "));
  Serial.println(triac_lock_delay);
  Serial.print(F("Light dimmers pin_list: "));
  for (uint8_t i=0; i<NUM_DIMMERS;i++) {
    Serial.print(pin_list[i]);
    i < NUM_DIMMERS - 1 ? Serial.print(F(" , ")) : Serial.println(F(""));
  }
  // dump the raw dimmer on/off data
  display_dimmer_states();

  // update the active dimmers with pointers to their dimm levels
  for (uint8_t i=0; i<NUM_ACTIVE_STEPS; i++) {
    for (uint8_t j=0; j<NUM_DIMMERS; j++) {
        // set the dimmer pointers in dimmers[] = either dimmer_levels[ON | OFF];
       active_steps[i].dimmers[j] = active_steps[i].dimmer_levels + (active_dimmer_states[i][j]);
    }
  }
  // update the inactive dimmers with pointers to their dimm levels  
  for (uint8_t i=0; i<NUM_INACTIVE_STEPS; i++) {
    for (uint8_t j=0; j<NUM_DIMMERS; j++) {
        // set the dimmer pointers in dimmers[] = either dimmer_levels[ON | OFF];
       inactive_steps[i].dimmers[j] = inactive_steps[i].dimmer_levels + (inactive_dimmer_states[i][j]);
    }
  }
  
  display_all_steps();
  
  // allocate and setup the new lights
  for (uint8_t i=0; i<NUM_DIMMERS; i++) {
    lights[i] = new Light(pin_list[i], DIMM_LEVEL_UNLIT, DIMM_LEVEL_LOW, DIMM_UP, false );
    lights[i]->display();
  }
      

  cur_ms = millis();    // a lot of this code uses the global cur_ms, initialize it before we start
  
  zdimmer = new ZeroCrossDimmer(lights, NUM_DIMMERS, ZERO_CROSS_PIN,  NUM_DIMM_LEVELS, triac_lock_delay);
  zdimmer->display();
  for (uint8_t i=0; i<NUM_POSITION_PINS; i++) {
    position_dev_list[i] = new InterruptDev(position_index_list[i], position_pin_list[i], 500, true, FALLING);
    position_dev_list[i]->display();  
  }  
  on_off_dev = new InterruptDev(ON_OFF_PIN, ON_OFF_PIN, 500, true, FALLING);
  on_off_dev->display();
  motor = new Motor(MOTOR_PIN);
  motor->display();
  // play_list = new Playbook(lights, NUM_DIMMERS, position_dev_list, NUM_POSITION_PINS, on_off_dev, motor, zdimmer);
  play_list = new Playbook(lights, NUM_DIMMERS, position_dev_list, NUM_POSITION_PINS, on_off_dev, motor, zdimmer);
  play_list->display();

  Serial.println(F("End Setup"));

}

void loop() {

  static uint32_t last_print_ms=0;
  static uint32_t last_ms;

  cur_ms_wrap=false;
  last_ms = cur_ms;
  cur_ms = millis();
  if (cur_ms < last_ms) cur_ms_wrap = true;
  if (cur_ms_wrap) last_print_ms = cur_ms;

  if (cur_ms - last_print_ms >= print_every_ms) {
    last_print_ms = cur_ms;
    Serial.print("cur_ms: ");
    Serial.println(cur_ms);
  }
  
  play_list->run();

}
