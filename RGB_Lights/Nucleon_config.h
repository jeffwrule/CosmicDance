#ifndef NUCLEON_CONFIG_H
#define NUCLEIO_CONFIG_H

#define NUM_QUARKS 3
#define NUM_FIBONOCCI 9
#define NUM_COLORS = 3;

// the current state of an individual light box
struct quark { 
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

quark quark_list[NUM_QUARKS];


#endif
