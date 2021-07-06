// Load Wi-Fi library
#include <WiFi.h>
#include "HTTPClient.h"

#define CONNECT_TIMEOUT 1000
    
int ledPin = LED_BUILTIN; // LED is attached to digital pin 13
int GPIO_pin = 13; // where our interrupt is going to be coming from.
volatile long hits = 0; // variable to be updated by the interrupt
volatile long false_interrupts = 0; // the number of times interrupt is called to soon
volatile long emi_interrupts = 0;  // trigger when no valid read has happend, but digitalRead fails to see high value
volatile int  last_analog_read = 0;  // check the value on the interrupt pin

//variables to keep track of the timing of recent interrupts
volatile unsigned long last_button_ms = 0; 
volatile boolean gong_activated = false;
#define STARTUP_DELAY 25000
volatile long startup_time=0;

boolean is_dancing=false;
boolean last_is_dancing=false;
boolean fob_on=false;
int dance_requests=0;
int failed_dance_requests=0;

// Replace with your network credentials
const char *ssid     = "SDZone-immersion";
const char *password = "ballast.scallop.teahouse";


#define STATUS_CHECK_DELAY 2000
long cur_millis=0;
long last_status_ms=0;

#define PRINT_INTERVAL 1000
long last_print_ms=0;

    
    
void setup() {                
  Serial.begin(115200);  //turn on serial communication
  Serial.println(F("Setup Begin: GongSensor July--05-2021"));
  //enable interrupt 0 which uses pin 2
  //jump to the increment function on falling edge
  pinMode(ledPin, OUTPUT);


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

  IPAddress IP = WiFi.softAPIP();
  Serial.print("  IP address: ");
  Serial.println(IP);
  startup_time=millis();
  Serial.println("Setup Complete");
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

    HTTPClient http;   

    if (millis() < cur_millis) {
        /* we wrapped around, reset all saved time stamps to a low value */
        last_status_ms = 0;
        last_button_ms = 0;
    }
    cur_millis=millis();
    
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
    Serial.print(", dance_requests: ");
    Serial.print(dance_requests);
    Serial.print(", failed_dance_requestas: ");
    Serial.print(failed_dance_requests);
    Serial.print(", hits: ");
    Serial.print(hits);
    Serial.print(", last_analog_read: ");
    Serial.print(last_analog_read);    
    Serial.print(", emi_interrupts: ");
    Serial.print(emi_interrupts);    
    Serial.print(", false_interrupts: ");
    Serial.println(false_interrupts);
    
}
    
void loop() {

    static boolean startup_delay_notified=false;

    pinMode(GPIO_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(GPIO_pin), increment, FALLING);

    
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
    }

    get_status();
  
    print_status();  
  
}
    
// Interrupt service routine for interrupt 0
void increment() {
  long button_ms = millis();
  if (button_ms - startup_time < STARTUP_DELAY) {
    Serial.println("Skipping early interrupts until STARTUP_DELAY has passed");
    return;
  }
  //check to see if increment() was called in the last 250 milliseconds
  if (button_ms - last_button_ms > 250)
  {
    last_analog_read=analogRead(GPIO_pin);
    if (last_analog_read <= 100) {
        Serial.print("Last Analog Read=");
        Serial.println(last_analog_read);
        gong_activated = true;
        hits++;        
        last_button_ms = button_ms;
    } else {
        emi_interrupts += 1;
    }
  } else {
    false_interrupts++;
  }
}
