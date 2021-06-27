// Load Wi-Fi library
#include <WiFi.h>
#include "HTTPClient.h"

#define CONNECT_TIMEOUT 1000
    
int ledPin = LED_BUILTIN; // LED is attached to digital pin 13
int GPIO_pin = 13; // where our interrupt is going to be coming from.
volatile long hits = 0; // variable to be updated by the interrupt
volatile long false_interrupts = 0; // the number of times interrupt is called to soon

//variables to keep track of the timing of recent interrupts
volatile unsigned long button_time = 0;  
volatile unsigned long last_button_time = 0; 
volatile boolean           led_state=false;
volatile boolean           last_led_state=false;

// Replace with your network credentials
const char* ssid     = "SDZone-immersion";
const char* password = "ballast.scallop.teahouse";
    
    
void setup() {                
  Serial.begin(115200);  //turn on serial communication
  Serial.println(F("Setup Begin: GongSensor Jun-2021"));
  //enable interrupt 0 which uses pin 2
  //jump to the increment function on falling edge
  pinMode(ledPin, OUTPUT);
  pinMode(GPIO_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GPIO_pin), increment, FALLING);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAPConfig (IPAddress(192,168,4,1), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
  WiFi.softAP(ssid, password, 1, false, 8);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  Serial.println("Setup Complete");
}

void start_dance() {

    HTTPClient http;   
    
    http.begin("http://192.168.4.22:5000/dance");
    http.setConnectTimeout(CONNECT_TIMEOUT);
    int httpResponseCode = http.POST("");
    
    if(httpResponseCode>0){
    
    String response = http.getString();   
    
    Serial.println(httpResponseCode);
    Serial.println(response);          
    
    }else{
    
    Serial.print("Error on sending PUT Request: ");
    Serial.println(httpResponseCode);
    
    }
    
    http.end();
}
    
void loop() {

  digitalWrite(ledPin, !led_state);

  if (led_state != last_led_state) {
    last_led_state = led_state;
    digitalWrite(ledPin, led_state);
    Serial.print("New LED State: ");
    Serial.print(led_state);
    Serial.print(", hits: ");
    Serial.print(hits);
    Serial.print(", false_interrupts: ");
    Serial.println(false_interrupts);
    Serial.println("Starting New Dance");
    start_dance();
  }
  
}
    
// Interrupt service routine for interrupt 0
void increment() {
  button_time = millis();
  //check to see if increment() was called in the last 250 milliseconds
  if (button_time - last_button_time > 250)
  {
    led_state = !led_state;
    hits++;
    last_button_time = button_time;
  } else {
    false_interrupts++;
  }
}
