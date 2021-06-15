#include <SoftwareSerial.h>

#define IS_BRIEF True
//#define IS_CHATTY True 


const int signal_pin = A3;
const int rocker_pin = A0;                 // pin to turn the electronics on and off


// control the rocking chair
boolean rock_interval_active;            // are we actively pulling rocker down...
unsigned long rock_interval_started_ms;     // time when we last flipped the rocking on or off
unsigned long rock_interval_length_ms;      // time to wait for the next rock interval
#define  ROCK_ON_INTERVAL_MS 750            // rocking seems to work best when we are on slightly longer then off
#define  ROCK_OFF_INTERVAL_MS  500          // time to wait for rocker to pull back when magnets are off

boolean is_rocking;                      // we flip this on for random periods of time to create the illusions of a random cycle
unsigned long rocking_started_ms;           // time when we last stop/started rocking
unsigned long rocking_length_ms;            // how long will we rock on wait.

#define START_ROCKING_MIN 1              // min seconds to rock
#define START_ROCKING_MAX 6              // max seconds to rock
#define STOP_ROCKING_MIN 10              // min seconds to not-rock
#define STOP_ROCKING_MAX 20              // max seconds to rock


boolean is_dancing = false;               // are we in dance mode? 
long is_dancing_last_change_ms=0;         // fix any bounce on this line
#define NEW_SIGNAL_MIN_MS  500            // wait at least 250 ms before you accept a new input

long cur_millis=0;
long last_print_ms=0;
#define PRINT_EVERY_MS 3000


void setup() {

  Serial.begin(250000);             // setup the interal serial port for debug messages
  Serial.println("Start setup");
  Serial.println("Codebase: RemoteRockingChair Jun-2021");
  
  // setup the soft serial port for xbee reads and writes
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(rocker_pin, OUTPUT);
  digitalWrite(rocker_pin, LOW);
  pinMode(signal_pin, INPUT);
  
   is_rocking = false;
   rocking_started_ms = millis();
   rocking_length_ms = 3000;
   rock_interval_active = false;
   rock_interval_started_ms = millis();
   rock_interval_length_ms = ROCK_ON_INTERVAL_MS;
     
  is_dancing = false;
  
  Serial.println(F("End setup"));
}

/*
 * check the network to see if we any new directions 
 * from the director
 */
void check_signal_line() {
    boolean new_signal = false;
    
    new_signal = digitalRead(signal_pin) == 1;

//    Serial.print("new_signal: ");
//    Serial.print(new_signal ? "True" : "False");
//    Serial.println("");
    
    if (new_signal != is_dancing 
       && (cur_millis - is_dancing_last_change_ms > NEW_SIGNAL_MIN_MS)) {
        is_dancing_last_change_ms = cur_millis;
        is_dancing = new_signal;
        Serial.println("");
        Serial.print("~~~~~~~~~~~~~~~~~~~~~~ is_dancing changed to: ");
        Serial.println(is_dancing ? F("True") : F("False") );
        
        if (is_dancing == false) {
            digitalWrite(rocker_pin, LOW);
            digitalWrite(LED_BUILTIN, LOW);
            is_rocking=false;
        }
    }
    
}

void start_rocking() {
    // start rocking
    rocking_length_ms = random(START_ROCKING_MIN, START_ROCKING_MAX) * 1000;  // rock at least 10 seconds, but no more then 20
    is_rocking = true;
    Serial.print("    start rocking: ");
    Serial.println(rocking_length_ms);
}

void stop_rocking() {
  // stop rocking 
  digitalWrite(rocker_pin, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  rocking_length_ms = random(STOP_ROCKING_MIN, STOP_ROCKING_MAX) * 1000;  // stop rocking for 5 seconds, but no more then 10 seconds
  is_rocking = false;
  Serial.print("    stop rocking: ");
  Serial.println(rocking_length_ms);
}

// turn the change on and off at the pre-determined intervals
void rock_chair() {
  
  unsigned long cur_time = millis();
  unsigned long cur_diff;

    if (is_dancing == false) {
        return;
    }
    
    // the milliseconds can wrap around ... so we just have to reset things if they do.
    if (cur_time < rock_interval_started_ms) {
        rock_interval_started_ms = cur_time;
        return;
    }
  
    // rock the chair back and forth
    if (is_rocking) {
        cur_diff = cur_time - rock_interval_started_ms;
        // stop/start individual rocks
        if (cur_diff >= rock_interval_length_ms) {
            rock_interval_started_ms = cur_time;
            if (rock_interval_active) {
                digitalWrite(rocker_pin, LOW);
                digitalWrite(LED_BUILTIN, LOW);
                rock_interval_length_ms = ROCK_OFF_INTERVAL_MS;
                rock_interval_active = false;
                Serial.println("    rock interval stop");
            } else {
                digitalWrite(rocker_pin, HIGH);
                digitalWrite(LED_BUILTIN, HIGH);
                rock_interval_length_ms = ROCK_ON_INTERVAL_MS;
                rock_interval_active = true;
                Serial.println("    rock interval start");
            }
        }
    }
    
    // start/stop the chair at random intervals
    cur_diff  = cur_time - rocking_started_ms;
    //    Serial.print("cur_diff: ");
    //    Serial.println(cur_diff);
    if (cur_diff > rocking_length_ms) {
        rocking_started_ms = cur_time;
        if (is_rocking) {
            stop_rocking();
        } else {
            start_rocking();
        }
    }

}


// dump some info to the terminal every now and then
void status() {

    Serial.print("cur_millis: ");
    Serial.print(cur_millis);
    Serial.print(F(", is_dancing: "));
    Serial.print(is_dancing ? F("True") : F("False"));
    Serial.print(", is_rocking: ");
    Serial.print(is_rocking ? F("True") : F("False"));
    Serial.print(", rocker_interival_active: ");
    Serial.print(rock_interval_active ? F("True") : F("False"));
    Serial.println();
    
}

void update_millis() {
    long new_millis;
    new_millis = millis();

    // watch for wrap around
    if (new_millis < cur_millis) {
        cur_millis = 0;
    } else {
        cur_millis = new_millis;
    }
}

// start and stop dancers, keep the show moving along....
void loop() {  

    update_millis();

    check_signal_line();

    // lets dump a status message very so often
    if (cur_millis - last_print_ms > PRINT_EVERY_MS) {
        last_print_ms = cur_millis;
        status();
    }

    rock_chair();
  

}
