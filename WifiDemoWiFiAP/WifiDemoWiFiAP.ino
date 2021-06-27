/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>
#include "HTTPClient.h"
#define CONNECT_TIMEOUT 100

// Replace with your network credentials
const char* ssid     = "SDZone-immersion";
const char* password = "ballast.scallop.teahouse";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAPConfig (IPAddress(192,168,4,1), IPAddress(0,0,0,0), IPAddress(255,255,255,0));
  WiFi.softAP(ssid, password, 1, false, 8);


  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
}

void loop() {

    Serial.printf("Stations connected to soft-AP = %d\n", WiFi.softAPgetStationNum());
    
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

 
  delay(10000);
}
