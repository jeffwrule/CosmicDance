#ifndef NUCLEON_CONFIG_H
#define NUCLEIO_CONFIG_H

#define NUM_QUARKS 3

#define INIT_DELAY_MS 8000

// values for a single color
struct color {
    long r;
    long g;
    long b;
    };

color red = { MAX_GRAY, 0, 0 };
color yellow = { MAX_GRAY, MAX_GRAY, 0 };
color green = { 0, MAX_GRAY, 0 };
color cyan = { 0, MAX_GRAY, MAX_GRAY };
color blue = { 0, 0, MAX_GRAY };
color magenta { MAX_GRAY, 0, MAX_GRAY };

color white = { MAX_GRAY, MAX_GRAY, MAX_GRAY };


// the current state of an individual quark
struct quark { 
    // set once when initialized
    unsigned int quark_id;             // sculpture number (abribrarily assigned)
    unsigned int channel_r;           // channel(pin) for red LEDs
    unsigned int channel_g;           // channel(pin) for green LEDs
    unsigned int channel_b;           // channel(pin) for blue LEDs
    // values that 
    // change for each round
    unsigned int color_index;         // index into the color list names for this box
    color current_color;              // current color level
    color target_color;               // color we are headed for
    color initial_color;              // color when we started this dimm cycle
    long increment_r;                 // increment for the red channel
    long increment_g;                 // incrmeent for the green channel
    long increment_b;                 // increment for the blue channel
    long increment_factor;            // percent increment factor
    };

// values to control how quckly we are dimming from 0 to MAX_GRAY and back again
struct dimm_cycle_t { 
        boolean  to_white_complete;         // did we finish dimming to white? (this is always first)
        boolean  to_color_complete;         // did we finish returning to a new color? (this completes the cycle)
        boolean  inital_delay;              // one long delay at begenning of cycle;
        unsigned long dimm_duration_ms;     // total duration of the dimm down/up (half down half up)
        unsigned long dimm_start_ms;        // when did we start the current dimm
        unsigned long current_increment;    // number of increments for this run
        unsigned long ms_per_step;          // how long to wait between increments (for longer dimms)     
        unsigned long ms_last_step;         // when did we execute our last color change
        unsigned long num_increments;        // increment number when half done
};

// average time per increment
struct avg_update_time_t { 
    unsigned long count;      // total number of changes
    unsigned long total_ms;   // total milliseconds spent sending data to lights.
    };

#endif
