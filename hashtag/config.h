#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG
#define DO_LOG

// dimm units values, changing NUM_DIMM_LEVELS will drastically change the ON,LOW,HIGH and UNLIT values 
// note: our variable can only trach 255 dimm_leves, this will break if you try something bigger b/c of variable overflow
#define NUM_DIMM_LEVELS 200
#define DIMM_LEVEL_OFF NUM_DIMM_LEVELS
#define DIMM_LEVEL_ON 0
#define DIMM_LEVEL_LOW 160
#define DIMM_LEVEL_HIGH 60
#define DIMM_LEVEL_UNLIT 170

#define NUM_DIMMERS 6

#endif
