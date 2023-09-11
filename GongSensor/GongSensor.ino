// Load Wi-Fi library
#include <WiFi.h>
#include "HTTPClient.h"

#define CONNECT_TIMEOUT 1000
    
int ledPin = LED_BUILTIN; // LED is attached to digital pin 13
int GPIO_pin = 33; // where our interrupt is going to be coming from.
volatile long hits = 0; // variable to be updated by the interrupt
volatile long false_interrupts = 0; // the number of times interrupt is called to soon
volatile long emi_interrupts = 0;  // trigger when no valid read has happend, but digitalRead fails to see high value
volatile int  last_analog_read = 0;  // check the value on the interrupt pin

long    gongs_seen=0;                // how many recent gongs
long    last_gong=0;             // first gong time in recent series

//variables to keep track of the timing of recent interrupts
volatile unsigned long last_button_ms = 0; 
volatile boolean gong_activated = false;
// #define STARTUP_DELAY 25000
#define STARTUP_DELAY 0
#define GONG_DELAY 2000
volatile long startup_time=0;

boolean is_dancing=false;
boolean last_is_dancing=false;
boolean fob_on=false;
int dance_requests=0;
int stop_requests=0;
int failed_dance_requests=0;
int failed_stop_requests=0;

// Replace with your network credentials
const char *ssid     = "SDZone-immersion";
const char *password = "ballast.scallop.teahouse";


#define STATUS_CHECK_DELAY 2000
long cur_millis=0;
long last_status_ms=0;

#define PRINT_INTERVAL 1000
long last_print_ms=0;

#define NUM_VALUES 20
#define GONG_CUTOFF 10
#define BUTTON_DELAY_MS 750
int  signal_values[NUM_VALUES];
long signal_sum=0;
int  signal_avg=0;
int  signal_idx=0;

long num_reads=0;
    
void setup() {                
  Serial.begin(115200);  //turn on serial communication
  Serial.println(F("Setup Begin: GongSensor July--05-2021"));
  //enable interrupt 0 which uses pin 2
  //jump to the increment function on falling edge
  pinMode(ledPin, OUTPUT);
  pinMode(GPIO_pin, INPUT);


  // Create a Wi-Fi network with ssid and password
  Serial.print("SSID: ");
  Serial.print(ssid);
  Serial.print(", Password: ");
  Serial.print(String(password).substring(0,7));
  Serial.print("xxxxxxxxxxxxxxx");
  Serial.println("");
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAPConfig (IPAddress(192,168,4,1), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
  WiFi.softAP(ssid, password, 1, false, 8);

  clear_values();

  IPAddress IP = WiFi.softAPIP();
  Serial.print("  IP address: ");
  Serial.println(IP);
  startup_time=millis();
  Serial.println("Setup Complete");
}

void clear_values() {

  /* This is an active low signal, so lets artififally make it appear off or high */
  /* the values stored here are from analogRead which returns a 16 bit value */
  for (int i=0; i<NUM_VALUES; i++) {
    signal_values[i]=0xffff;
  }
    
}

void error_blink() {
    int blink_delay=100;
    Serial.println("Error Blink...");
    for (int i=0; i<3; i++) {
        digitalWrite(LED_BUILTIN, 0);
        delay(blink_delay);
        digitalWrite(LED_BUILTIN, 1);
        delay(blink_delay); 
    }
}

void start_dance() {

    Serial.println("\n>>>>>>>>>>>>>>>>>>>> Starting New Dance");   

    // add a delay for effect between gong hit and sending the message
    Serial.print("Delay before start: ");
      Serial.print(GONG_DELAY);
    Serial.println("");
    delay(2000);

    HTTPClient http;   
    
    http.begin("http://192.168.4.22:5000/dance");
    http.setConnectTimeout(CONNECT_TIMEOUT);
    int httpResponseCode = http.POST("");
    
    if(httpResponseCode>0){
    
        String response = http.getString();   
        
        // Serial.println(httpResponseCode);
        // Serial.println(response); 

        if (httpResponseCode != 200) {
            Serial.print("http dance request failed, status=");
            Serial.println(response);
            Serial.print(", httpResponseCode=");
            Serial.println(httpResponseCode);
            failed_dance_requests += 1;
        } else {
            dance_requests += 1;        
        }

        
    }else{
    
        Serial.print("Error on sending dance PUT Request: ");
        Serial.println(httpResponseCode);
        error_blink();
    }
    
    http.end();
}

void stop_dance() {

    Serial.println("\n>>>>>>>>>>>>>>>>>>>> Stopping Dance");    

    HTTPClient http;   
    
    http.begin("http://192.168.4.22:5000/stop");
    http.setConnectTimeout(CONNECT_TIMEOUT);
    int httpResponseCode = http.POST("");
    
    if(httpResponseCode>0){
    
        String response = http.getString();   
        
        // Serial.println(httpResponseCode);
        // Serial.println(response); 

        if (httpResponseCode != 200) {
            Serial.print("http dance request failed, status=");
            Serial.println(response);
            Serial.print(", httpResponseCode=");
            Serial.println(httpResponseCode);
            failed_stop_requests += 1;
        } else {
            stop_requests += 1;        
        }

        
    }else{
    
        Serial.print("Error on sending stop PUT Request: ");
        Serial.println(httpResponseCode);
        error_blink();
    }
    
    http.end();
}

void get_status() {

    if (cur_millis - last_status_ms < STATUS_CHECK_DELAY) {
        return;
    }

    // Serial.println("Checking Status");
    
    last_status_ms = cur_millis;

    HTTPClient http;   
    
    http.begin("http://192.168.4.22:5000/status");
    http.setConnectTimeout(CONNECT_TIMEOUT);
    int httpResponseCode = http.POST("");
    
    if(httpResponseCode>0){
    
        String response = http.getString();   
    
        // Serial.println(httpResponseCode);
        // Serial.println(response);    

         if (response ==  "not_dancing") {
            is_dancing = false;
            fob_on = false;
         } else if (response == "is_dancing" ) {
            is_dancing = true;
            fob_on = false;
         } else if (response == "fob_on") {
            fob_on = true;
            is_dancing = true;
         } else {
            Serial.println("Unknown status: " + response);
         }
    
    }else{
        Serial.print("check_status(): Error on PUT/status Request: ");
        Serial.println(httpResponseCode);
        error_blink();
    }
    
    http.end();
}

void print_status() {

    if (cur_millis - last_print_ms < PRINT_INTERVAL) {
        return;
    }
    last_print_ms = cur_millis;
    Serial.print("is_dancing: ");
    Serial.print(is_dancing);
    Serial.print(", fob_on: ");
    Serial.print(fob_on);
//    Serial.print(", dance_requests: ");
//    Serial.print(dance_requests);
//    Serial.print(", failed_dance_requestas: ");
//    Serial.print(failed_dance_requests);
//    Serial.print(", stop_requests: ");
//    Serial.print(stop_requests);
//    Serial.print(", failed_stop_requestas: ");
//    Serial.print(failed_stop_requests);
    Serial.print(", hits: ");
    Serial.print(hits);    
    Serial.print(", num_reads: ");
    Serial.print(num_reads);
    Serial.print(", signal_avg: ");
    Serial.print(signal_avg);
    long uptime_sec=(cur_millis - startup_time) / 1000;
    if (uptime_sec == 0) { uptime_sec = 1; }
    Serial.print(", reads_per_second: ");
    Serial.print( num_reads / uptime_sec);
    Serial.print(", seconds_running: ");
    Serial.print( uptime_sec);

//    Serial.print(", last_analog_read: ");
//    Serial.print(last_analog_read);    
//    Serial.print(", emi_interrupts: ");
//    Serial.print(emi_interrupts);    
//    Serial.print(", false_interrupts: ");
//    Serial.print(false_interrupts);
    Serial.print("\n");
    
}

void read_next() {
    long button_ms = millis();
    boolean in_delay = false;
    
    in_delay = button_ms - last_button_ms <= BUTTON_DELAY_MS;

    
    signal_idx += 1;
    if (signal_idx >= NUM_VALUES) {
        signal_idx=0;
    }

    if (!in_delay) {
        num_reads++;
        signal_values[signal_idx] = analogRead(GPIO_pin);
        // Serial.println(signal_values[signal_idx]);
    }

    signal_sum = 0;
    for (int i=0; i<NUM_VALUES; i++) {
        signal_sum += signal_values[signal_idx];
    }

    signal_avg = (int)(signal_sum / NUM_VALUES);

//    if (signal_avg > 0) {
//        Serial.print("Average is: ");
//        Serial.print(signal_avg);
//        Serial.print(", Values: ");
//        for (int i=0; i<NUM_VALUES; i++) {
//            Serial.print(", ");
//            Serial.print(signal_values[i]);
//        }
//        Serial.print("\n");
//    }

    if (signal_avg <= GONG_CUTOFF) {

        if (!in_delay)
            hits++;
            gong_activated = true;
            last_button_ms = button_ms;
            clear_values();
            Serial.println("\n>>>>>>>>>>> GONG DETECTED >>>>>>>>>>>\n");
        }
    
}
    
void loop() {

    static boolean startup_delay_notified=false;


    pinMode(GPIO_pin, INPUT);
    // attachInterrupt(digitalPinToInterrupt(GPIO_pin), increment, FALLING);

    
//    Serial.print("a=");
//    Serial.println(last_analog_read);
     
    if (millis() < cur_millis) {
        Serial.print("Millis looped around, resetting ms trackers");
        last_print_ms = 0;
        last_status_ms = 0;
        last_button_ms=0;
    }

    cur_millis = millis();

    if (!startup_delay_notified) {
        if (cur_millis - startup_time >= STARTUP_DELAY) {
            startup_delay_notified=true;
            Serial.println("\n\nStartup delay complete, now acceptting gongs!\n");
        }
    }

    read_next();

    if (last_is_dancing != is_dancing) {
        Serial.print("\n<<<<<<<<<<<<<<<<<<<< Dancing Status Changed to: ");
        Serial.println(is_dancing);
        last_is_dancing = is_dancing;
    }
    digitalWrite(ledPin, !is_dancing);  // have to reverse it for intern LED on LOLIN Pro board

    if (gong_activated) {
        gong_activated=false;
        if (is_dancing || fob_on) {
            Serial.println("Dancer already active, skipping request");
        } else {
            start_dance(); 
        }

        // gong 5 times in 1.5 seconds to stop the device
        if (cur_millis - last_gong > 2000) {
            gongs_seen=0;
            Serial.println("\n\nNew Gong series started...\n");
        } 
        last_gong=cur_millis;
        gongs_seen += 1;
        Serial.print("Gongs Seen=");
        Serial.println(gongs_seen);
        if (gongs_seen >= 5) {
            stop_dance();
            // reset the gong counter
            gongs_seen=0;
            last_gong=0;
        }
    }

    get_status();
  
    print_status();  
  
}
    
