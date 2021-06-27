#include "WiFi.h"
#include "HTTPClient.h"
 
const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPassword";
 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password); 
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
 
}
 
void loop() {
 
 if(WiFi.status()== WL_CONNECTED){
 
   HTTPClient http;   
 
   http.begin("http://jsonplaceholder.typicode.com/posts/1");
   http.addHeader("Content-Type", "text/plain");            
 
   int httpResponseCode = http.PUT("PUT sent from ESP32");   
 
   if(httpResponseCode>0){
 
    String response = http.getString();   
 
    Serial.println(httpResponseCode);
    Serial.println(response);          
 
   }else{
 
    Serial.print("Error on sending PUT Request: ");
    Serial.println(httpResponseCode);
 
   }
 
   http.end();
 
 }else{
    Serial.println("Error in WiFi connection");
 }
 
  delay(10000);
}
