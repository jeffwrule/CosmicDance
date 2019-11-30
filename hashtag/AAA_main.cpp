#include "config.h"
#include "InterruptDev.h"
#include "ZeroCrossDimmer.h"
#include "Light.h"
#include "Playbook.h"
#include "Motor.h"

const int16_t ZERO_CROSS_PIN=2;    // takes interrupt from dimmerboard everytime AC mains switches polarity
const uint8_t POSITION_PIN=3;      // pin the gets interrupt when positions of the clock are passed 
const uint8_t ON_OFF_PIN=4;        // pin attached to on off switch
const uint8_t MOTOR_PIN=13;        // pin attached to on off switch

// config info for this app
uint8_t pin_list[NUM_DIMMERS] = { 7,8,9,10,11,12 };     // current pin mappings for each of the dimmers
unsigned int delay_time = 20;                           // time to delay between dimming levels (will probably get co-opted into a dimmer class
uint32_t triac_lock_delay=5;                            // how long (micro seconds) does the triac on this board take to lock, typical 5-20us start with 5

Light  *lights[NUM_DIMMERS];       // the physical lights dimmer
ZeroCrossDimmer *zdimmer;         // our active dimmer code
InterruptDev *position_dev;     // interrupts when momentary switch is pushed 
InterruptDev *on_off_dev;       // interrupts when the on off button is pushed
Motor *motor;                   // the dcmotor to stop and start as described in the playbook
Playbook *play_list;            // this feeds the dimmer with steps and watches inputs for changes

void setup() {

  // dump the config values
  Serial.begin(250000);
  Serial.println(F("Hashtag: version: 2019-11-28 v1"));
  Serial.print(F("NUM_DIMMERS: "));
  Serial.println(NUM_DIMMERS);
  Serial.print(F("ZERO_CROSS_PIN: "));
  Serial.println(ZERO_CROSS_PIN);
  Serial.print(F("POSITION_PIN: "));
  Serial.println(POSITION_PIN);
  Serial.print(F("ON_OFF_PIN: "));
  Serial.println(ON_OFF_PIN);
  Serial.print(F("MOTOR_PIN: "));
  Serial.println(MOTOR_PIN);
  Serial.print(F("delay_time: "));
  Serial.println(delay_time);
  Serial.print(F("triac_lock_delay: "));
  Serial.println(triac_lock_delay);
  Serial.print(F("Light dimmers pin_list: "));
  for (uint8_t i=0; i<NUM_DIMMERS;i++) {
    Serial.print(pin_list[i]);
    i < NUM_DIMMERS - 1 ? Serial.print(F(" , ")) : Serial.println(F(""));
  }
  
  // allocate and setup the new lights
  for (uint8_t i=0; i<NUM_DIMMERS; i++) {
    lights[i] = new Light(pin_list[i], DIMM_LEVEL_UNLIT + i);
    Serial.print(F("Light: "));
    Serial.print(i);
    Serial.print(F(" "));
    lights[i]->display();
    Serial.println(F(""));
  }
 
  
  zdimmer = new ZeroCrossDimmer(lights, NUM_DIMMERS, ZERO_CROSS_PIN,  NUM_DIMM_LEVELS, triac_lock_delay);
  zdimmer->display();
  position_dev = new InterruptDev( POSITION_PIN, 500, true, RISING);
  position_dev->display();
  on_off_dev = new InterruptDev(ON_OFF_PIN, 500, true, RISING);
  on_off_dev->display();
  motor = new Motor(MOTOR_PIN);
  motor->display();
  play_list = new Playbook(lights, NUM_DIMMERS, position_dev, on_off_dev, motor);
  play_list->display();

  Serial.println(F("End Setup"));

}

void loop() {

  play_list->run();

}
