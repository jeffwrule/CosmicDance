#ifndef CONFIG_H
#define CONFIG_H

#include <arduino.h>
#include <inttypes.h>

// takes interrupt from dimmerboard everytime AC mains switches polarity
#define ZERO_CROSS_PIN 2    
// pin the gets interrupt when positions of the clock are passed  
// pin attached to on off switch
#define ON_OFF_PIN 4
// pin attached to on off switch
#define MOTOR_PIN 12

/*
 * really the number of indexes to support 0-4 are for pins 
 *    0-4   (5 pins) 
 *    A0-A3 (4 pins) are using 5,6,7,8 
 *    
 *    so we need an arry of 9 positions to support
 */
#define INTERRUPT_DEV_NUM_DEVICES 9
#define NUM_POSITION_PINS 4
uint8_t position_pin_list[NUM_POSITION_PINS] = { A0, A1, A2, A3 };
uint8_t position_index_list[NUM_POSITION_PINS] = { 5, 6, 7, 8 };


#define INTERRUPT_DEV_ON 0
#define INTERRUPT_DEV_OFF 1


// dimm units values, changing NUM_DIMM_LEVELS will drastically change the ON,LOW,HIGH and UNLIT values 
// note: our variable can only trach 255 dimm_leves, this will break if you try something bigger b/c of variable overflow
#define NUM_DIMM_LEVELS 200
#define DIMM_LEVEL_OFF NUM_DIMM_LEVELS
#define DIMM_LEVEL_UNLIT 170
#define DIMM_LEVEL_LOWEST 150
#define DIMM_LEVEL_LOW 140
#define DIMM_LEVEL_MEDIUM 120
#define DIMM_LEVEL_HIGH 60
#define DIMM_LEVEL_ON 10

// dim directions
#define DIMM_UP -1
#define DIMM_DOWN 1
#define DIMM_STOP 0

// time to delay between dimming levels 
#define DIMM_CYCLE_ULTRA_SHORT_MS 2000
#define DIMM_CYCLE_SHORT_MS 4000
#define DIMM_CYCLE_NORMAL_MS 6000
#define DIMM_CYCLE_LONG_MS 8000

// time to wait when starting the motor before accepting new postion interrupts 
// debounces the position sensor on restart
#define MOTOR_START_INTERRUPT_IGNORE_MS 2000

bool      cur_ms_wrap=false;        // did cur_ms wrap around, we can use this in sub-routines to fix up ther local 'last' variables
uint32_t  cur_ms;                   // a global snapshot of cur_ms at the start of each loop
uint32_t  last_ms=0;                // the previous value of our call to ms, used to check for wrap around
uint32_t  print_every_ms=10000;     // loggin should print messages after this many milliseconds

#define IS_OSCELLATING true
#define NOT_OSCELLATING false

// on of two arrays tracking the light level for the individual blubs
#define NUM_LIGHT_LEVELS 1
#define LIGHTPREP_LEVEL0 0
#define LIGHTPREP_LEVEL1 1

// how mnay physical dimmers are we driving
#define NUM_DIMMERS 1

enum WaitFor { duration, position, on_off };   // duration will wait a given time to next step, position will wait for position_interrupt
                                               // on_off is used in things like stopped mode when we enter the next step when the button is pused
// index of strings into the WaitFor above, so you can print a status name
static const char *WAIT_FOR_STRING[] = {
    "duration", "position", "on_off" };

// defines the stat stored state for a ligh bulb (dimmer) w/i a given step
typedef struct LightPrep {
    uint32_t  target_level;         // this lights target level
    uint32_t  dimm_min;             // low end to dimm to
    uint32_t  dimm_max;             // high end to dimm to 
    int8_t    dimm_direction_init;  // use one of the constants DIMM_UP, DIMM_DOWN, DIMM_STOP
    bool      is_oscellating;       // are we cycling up and down on this light
};

// list of steps to execute, these will be applied to the current state machine in (Playbook) to effect 
// the active ligth dimmers and behaviors
typedef struct Step {
    uint8_t   step_id;                                // the step number for this step
    uint8_t   next_step_id;                           // which step to pay next
    WaitFor   waiting_for;                            // what kind of event triggers the next step
    uint32_t  timed_step_duration_ms;                 // for a timed step, how long to hold before the next stepd
    bool      motor_on;                               // motor state during this step 
    uint32_t  dimm_length_ms;                         // how long should this dimmer take to complete it's dimm up/down
    const uint8_t num_skip_interrupts;                // how many interrupts to skip before stopping
    const LightPrep dimmer_levels[NUM_LIGHT_LEVELS];  // the setups for each light
    const LightPrep *dimmers[NUM_DIMMERS];            // dimmers[i] level will be applight to lights[i];
};

// forward references for global routines, routines defined in Playbook_steps.h
void display_dimmer_states();
void display_step(Step *s_ptr);
void display_all_steps();


#endif
