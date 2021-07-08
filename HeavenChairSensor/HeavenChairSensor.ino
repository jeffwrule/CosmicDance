/*
 * This code takes as input two different sensors 
 * a set of distance sensors hooked together (mounted around the edge of the seat)
 * and a set of pressure sensors mounted on the bottom of the seat.
 * 
 * When either set of sensors is activated, the seat sends out a high signal on the seat_pin (to trigger a remote switch)
 * 
 */ 
// #include "analogWrite.h"

#define distance_pin 36
#define distance_led 33
#define pressure_pin 32
#define pressure_led 13
#define seat_pin 23
#define seat_led 22
#define NUM_READS 25
#define PRINT_EVERY_MS 1000

#define PRESSURE_CUTOFF 2500
#define PRESSURE_OFF 0

#define DISTANCE_CUTOFF 0.5
#define DISTANCE_OFF 1
#define DISTANCE_ON 0

#define SEAT_OCCUPIED_PWM 255
#define SEAT_EMPTY 0
#define SHUTOFF_DELAY 200
#define OFF LOW
#define ON HIGH

int     distance_reads[NUM_READS];      // last NUM_READS distance readings
int     pressure_reads[NUM_READS];      // last NUM_READS pressure readings
int     max_pressure_read=0;           // during each print cycle
int     read_index = 0;                 // current pointer into the sensor list of values
long    index_loops;                    // number of times we have looped around the size of our index

long    cur_millis=0;               // current time, gathered at the beginning of each loop
long    last_millis=0;              // list time we checked millis, what was it, for wrap around
long    last_print_ms=0;            // list time we sent out a status message

long    last_occupied_ms=0;         // last time either pressure or distance tested on
int     seat_state=OFF;             // track led state so we don't write to it so often
int     distance_state=OFF;         // distance no triggered
int     pressure_state=OFF;         // presure not triggered
boolean in_delay=false;     // Are we currently in a shutoff delay

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    Serial.println("Begin Setup HeavenChairSensor Jul-06-2021");

    pinMode(distance_pin, OUTPUT);               /* active low  */
    pinMode(pressure_pin, OUTPUT);               /* active high */
    pinMode(seat_pin, OUTPUT);                  /* active high */

    digitalWrite(distance_pin, HIGH);           /* active low  */
    digitalWrite(pressure_pin, LOW);            /* active high */
    digitalWrite(seat_pin, LOW);                /* active high */    
    
    pinMode(distance_pin, INPUT);               /* active low  */
    pinMode(pressure_pin, INPUT);               /* active high */
    pinMode(seat_pin, OUTPUT);                  /* active high */
    
    pinMode(seat_led, OUTPUT);                  /* indicator light for seat occupied state */
    pinMode(pressure_led, OUTPUT);              /* indicator light for presure_led state */
    pinMode(distance_led, OUTPUT);              /* indicator light for distance_led state */
    
    for (int i=0; i<NUM_READS; i++) {           // set of reads for each pin, to get average value */
        pressure_reads[i] = PRESSURE_OFF;
        distance_reads[i] = DISTANCE_OFF;
    }

    Serial.print("seat_pin/ah=");
    Serial.print(seat_pin);
    Serial.print(", pressure_pin/al=");
    Serial.print(pressure_pin);
    Serial.print(", distance_pin:/ah=");
    Serial.print(distance_pin);
    Serial.print(", seat_led=");
    Serial.print(seat_led);
    Serial.print(", pressure_led=");
    Serial.print(pressure_led);
    Serial.print(", distance_led=");
    Serial.print(distance_led);
    Serial.println("");
    Serial.print("DISTANCE_CUTOFF=");
    Serial.print(DISTANCE_CUTOFF);
    Serial.print(", PRESSURE_CUTOFF=");
    Serial.print(PRESSURE_CUTOFF);
    Serial.println("");
    Serial.println("End Setup");

}


void loop() {

    double pressure_sum;
    double distance_sum;
    double pressure_avg;
    double distance_avg;

    pinMode(distance_pin, INPUT);               /* active low  */
    pinMode(pressure_pin, INPUT);               /* active high */
    pinMode(seat_pin, OUTPUT);                  /* active high */

    last_millis = cur_millis;
    cur_millis = millis();
    if (cur_millis < last_millis) {
        last_millis=0;          // deal with ms wrap aournd for long running times
        last_occupied_ms = 0;   // this needs to be reset to a low value as well
    }

    // increment the index before we read
    read_index++;
    if ( read_index == NUM_READS ) {
        read_index=0;
        index_loops++;
    }

    // put your main code here, to run repeatedly:
    pressure_reads[read_index] = analogRead(pressure_pin);
    distance_reads[read_index] = digitalRead(distance_pin);

    pressure_sum=0;
    distance_sum=0;
    // Serial.print("    distance Values: ");
    for (int i=0; i<NUM_READS; i++) {
        pressure_sum = pressure_sum + pressure_reads[i];
        distance_sum = distance_sum + distance_reads[i];
        // Serial.print(", ");
        // Serial.print(distance_reads[i]);
    }
    // Serial.println("");
    
    distance_avg = distance_sum / NUM_READS;
    pressure_avg = pressure_sum / NUM_READS;

    if (pressure_avg > max_pressure_read) {
        max_pressure_read = pressure_avg;
    }

    /////// DEBUG DEBUG DEBUG
    // overriding the distance average and always turning it off
    // distance_avg = DISTANCE_OFF;

    if (cur_millis - last_print_ms > PRINT_EVERY_MS) {
        last_print_ms = cur_millis;
        Serial.print("Seat state: ");
        Serial.print(seat_state);
        
        Serial.print("\n    distance state: ");
        Serial.print(distance_state);
        Serial.print(", distance avg: ");
        Serial.print(distance_avg);
        Serial.print(", distance sum: ");
        Serial.print(distance_sum);

        
                
        Serial.print("\n    pressure state: ");
        Serial.print(pressure_state);
        Serial.print(", pressure Avg: ");
        Serial.print(pressure_avg);
        Serial.print(", pressure_sum=");
        Serial.print(pressure_sum);
        Serial.print(", max_presure_read=");
        Serial.print(max_pressure_read);
        
        Serial.print("\n    index_loops=");
        Serial.print(index_loops);
        Serial.print(", last_occupied_ms: ");
        Serial.print(last_occupied_ms);
        Serial.print(", in_delay: ");
        Serial.print(in_delay);
        Serial.println("");  
        }


    /*
     * Read and set Pressure States.
     */
    if (pressure_avg >= PRESSURE_CUTOFF) {
        if (pressure_state != ON) {
            Serial.println("\nPressure ON ~~~~~~~~~~");      
            pressure_state = ON;
            digitalWrite(pressure_led, pressure_state);        
        }
        last_occupied_ms=cur_millis;
    } else if (pressure_state != OFF) {
        Serial.println("\nPressure OFF %%%%%%%%%%");      
        pressure_state = OFF;
        max_pressure_read=0; 

        digitalWrite(pressure_led, pressure_state); 
    }


    /*
     * Read and set distance flags
     */
    if (distance_avg <= DISTANCE_CUTOFF) {
        if (distance_state != ON) {
            Serial.println("\nDistance ON %%%%%%%%%%");      
            distance_state = ON;
            digitalWrite(distance_led, distance_state); 
        }
        last_occupied_ms=cur_millis;
    } else {
        if (distance_state != OFF) {
            Serial.println("\nDistance OFF ~~~~~~~~~~");      
            distance_state = OFF;
            digitalWrite(distance_led, distance_state);  
        }
    }



    if (cur_millis - last_occupied_ms < SHUTOFF_DELAY) {
        if (in_delay != true) {
            in_delay=true;
            Serial.println("\n>>>>>>>>>> Starting Shutoff Delay");
        }
    } else {
        if (in_delay != false) {
            in_delay = false;
            Serial.println("\n<<<<<<<<<< Ending Shutoff Delay");
        }
    }

    if (distance_state == ON || pressure_state == ON) {
            if (seat_state != ON ) {
                Serial.println("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
                Serial.println("~~~~~~~~~~ SEAT ON ~~~~~~~~~~\n");      
                seat_state = ON;
                // analogWrite(seat_pin, SEAT_OCCUPIED_PWM, 100, 8);
                digitalWrite(seat_pin, HIGH);
                digitalWrite(seat_led, seat_state);
            }    
            last_occupied_ms = cur_millis;
    } else if (!in_delay) {    
            if (seat_state != OFF) {
                Serial.println("\n%%%%%%%%%% SEAT OFF %%%%%%%%%%");      
                Serial.println("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
                seat_state = OFF;
                // analogWrite(seat_pin, 0);
                digitalWrite(seat_pin, LOW);
                digitalWrite(seat_led, seat_state);
            }
        }



}
