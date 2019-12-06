#ifndef PLAYBOOK_STEPS_H
#define PLAYBOOK_STEPS_H

#include "config.h"

// this array is used to initialize the dimmers structure  
#define NUM_ACTIVE_STEPS 6
uint8_t active_dimmer_states[NUM_ACTIVE_STEPS][NUM_DIMMERS] = {
  {LIGHTPREP_ON, LIGHTPREP_OFF, LIGHTPREP_OFF, },
  {LIGHTPREP_ON, LIGHTPREP_OFF, LIGHTPREP_OFF, },
  {LIGHTPREP_OFF, LIGHTPREP_ON, LIGHTPREP_OFF, },
  {LIGHTPREP_OFF, LIGHTPREP_ON, LIGHTPREP_OFF, },
  {LIGHTPREP_OFF, LIGHTPREP_OFF, LIGHTPREP_ON, },
  {LIGHTPREP_OFF, LIGHTPREP_OFF, LIGHTPREP_ON, },
};

Step active_steps[NUM_ACTIVE_STEPS] = { 
      { 0, 1, position, 0, true, DIMM_CYCLE_SHORT_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_HIGH, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_MEDIUM, DIMM_LEVEL_LOW, DIMM_LEVEL_HIGH, DIMM_UP, IS_OSCELLATING },
              },
        },
      { 1, 2, duration, 40000, false, DIMM_CYCLE_SHORT_MS, 0,
            {
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_HIGH, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_HIGH, DIMM_LEVEL_LOW, DIMM_LEVEL_HIGH, DIMM_UP, IS_OSCELLATING },
              },
        },
      { 2, 3, position, 0, true, DIMM_CYCLE_SHORT_MS, 0, 
            {
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_HIGH, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_MEDIUM, DIMM_LEVEL_LOW, DIMM_LEVEL_HIGH, DIMM_UP, IS_OSCELLATING },
              },
        },
      { 3, 4, duration, 40000, false, DIMM_CYCLE_SHORT_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_HIGH, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_HIGH, DIMM_LEVEL_LOW, DIMM_LEVEL_HIGH, DIMM_UP, IS_OSCELLATING },
              },
        },
      { 4, 5, position, 0, false, DIMM_CYCLE_SHORT_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_HIGH, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_MEDIUM, DIMM_LEVEL_LOW, DIMM_LEVEL_HIGH, DIMM_UP, IS_OSCELLATING },
              },
        },
      { 5, 0, duration, 40000, false, DIMM_CYCLE_SHORT_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_HIGH, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_HIGH, DIMM_LEVEL_LOW, DIMM_LEVEL_HIGH, DIMM_UP, IS_OSCELLATING },
              },
        },
      };


// steps to execute when sculpture is in off state
#define NUM_INACTIVE_STEPS 5

uint8_t inactive_dimmer_states[NUM_INACTIVE_STEPS][NUM_DIMMERS] = {
  {LIGHTPREP_ON, LIGHTPREP_ON, LIGHTPREP_ON, },
  {LIGHTPREP_ON, LIGHTPREP_OFF, LIGHTPREP_OFF, },
  {LIGHTPREP_OFF, LIGHTPREP_ON, LIGHTPREP_OFF, },
  {LIGHTPREP_OFF, LIGHTPREP_OFF, LIGHTPREP_ON, },
  {LIGHTPREP_ON, LIGHTPREP_ON, LIGHTPREP_ON, },
} ;

Step inactive_steps[NUM_INACTIVE_STEPS] = { 
      { 0, 1, position, 0, true, DIMM_CYCLE_LONG_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_LOW, DIMM_LEVEL_OFF, DIMM_LEVEL_LOW, DIMM_UP, IS_OSCELLATING },
              }
        },
    { 1, 2,  on_off, 0, false, DIMM_CYCLE_LONG_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_LOW, DIMM_LEVEL_OFF, DIMM_LEVEL_LOW, DIMM_UP, IS_OSCELLATING },
              },
      },
    { 2, 3,  on_off, 0, false, DIMM_CYCLE_LONG_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_LOW, DIMM_LEVEL_OFF, DIMM_LEVEL_LOW, DIMM_UP, IS_OSCELLATING },
              },
      },
    { 3, 4,  on_off, 0, false, DIMM_CYCLE_LONG_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_LOW, DIMM_LEVEL_OFF, DIMM_LEVEL_LOW, DIMM_UP, IS_OSCELLATING },
              },
      },
    { 4, 1,  on_off, 0, false, DIMM_CYCLE_LONG_MS, 0,
            { 
              { DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_LEVEL_OFF, DIMM_DOWN, NOT_OSCELLATING },
              { DIMM_LEVEL_LOW, DIMM_LEVEL_OFF, DIMM_LEVEL_LOW, DIMM_UP, IS_OSCELLATING },
              },
      },};

void display_dimmer_states() {
  
  Serial.println(F("  display_dimmer_states(): "));
  
  for (uint8_t i=0; i<NUM_ACTIVE_STEPS; i++) {
    Serial.print(F("    active_dimmer_states["));
    Serial.print(i); 
    Serial.print(F("]: "));
    for (uint8_t j=0; j<NUM_DIMMERS; j++) {
      Serial.print(active_dimmer_states[i][j]);
      Serial.print(F(", "));
    }
    Serial.println(F(""));
  }

  for (uint8_t i=0; i<NUM_INACTIVE_STEPS; i++) {
    Serial.print(F("    inactive_dimmer_states["));
    Serial.print(i); 
    Serial.print(F("]: "));
    for (uint8_t j=0; j<NUM_DIMMERS; j++) {
      Serial.print(inactive_dimmer_states[i][j]);
      Serial.print(F(", "));
    }
    Serial.println(F(""));
  }
  Serial.println(F(""));
   
}

void display_step(Step *s_ptr) {
  Serial.print(F("    Playbook::display_step(Step *s_ptr) Active Step: step_id="));
  Serial.print(s_ptr->step_id);
  Serial.print(F(", next_step_id="));
  Serial.print(s_ptr->next_step_id); 
  Serial.print(F(", waiting_for="));
  Serial.println(WAIT_FOR_STRING[s_ptr->waiting_for]);
  Serial.print(F("               s_ptr->timed_step_duration_ms="));
  Serial.print(s_ptr->timed_step_duration_ms);
  Serial.print(F(", s_ptr->motor_on="));
  Serial.print(s_ptr->motor_on);
  Serial.print(F(", s_ptr->dimm_length_ms="));
  Serial.println(s_ptr->dimm_length_ms);
  Serial.println(F("      Available Light Levels:"));       
    for (uint8_t i=0; i<NUM_LIGHT_LEVELS; i++) {
    Serial.print(F("        s_ptr->dimmer_levels["));
    Serial.print(i);
    Serial.print(F("]: target_level="));
    Serial.print(s_ptr->dimmer_levels[i].target_level);
    Serial.print(F(": dimm_min="));
    Serial.print(s_ptr->dimmer_levels[i].dimm_min);
    Serial.print(F(": dimm_max="));
    Serial.print(s_ptr->dimmer_levels[i].dimm_max);
    Serial.print(F(": dimm_direction_init="));
    Serial.print(s_ptr->dimmer_levels[i].dimm_direction_init);
    Serial.print(F(": is_oscellating="));
    Serial.println(s_ptr->dimmer_levels[i].is_oscellating);
  } 
  Serial.println(F("      Dimmer settings:"));       
  for (uint8_t i=0; i<NUM_DIMMERS; i++) {
    Serial.print(F("        s_ptr->dimmers["));
    Serial.print(i);
    Serial.print(F("]: target_level="));
    Serial.print((s_ptr->dimmers[i])->target_level);
    Serial.print(F(": dimm_min="));
    Serial.print(s_ptr->dimmers[i]->target_level);
    Serial.print(F(": dimm_min="));
    Serial.print(s_ptr->dimmers[i]->dimm_min);
    Serial.print(F(": dimm_max="));
    Serial.print(s_ptr->dimmers[i]->dimm_max);
    Serial.print(F(": dimm_direction_init="));
    Serial.print(s_ptr->dimmers[i]->dimm_direction_init);
    Serial.print(F(": is_oscellating="));
    Serial.print(s_ptr->dimmers[i]->is_oscellating);
    Serial.println(F(""));
  }  
}

void display_all_steps() {

  Serial.println(F(" display_all_steps(): "));


  Serial.println(F("  Playbook active Steps: "));
  for (uint8_t i=0; i<NUM_ACTIVE_STEPS; i++) {
    display_step(active_steps + i);
  }

  
  Serial.println(F("  Playbook Inactive Steps: "));
  for (uint8_t i=0; i<NUM_INACTIVE_STEPS; i++) {
    display_step(inactive_steps + i);
  }

}

#endif
