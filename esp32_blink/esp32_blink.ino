#define ONBOARD_LED  2
 
void setup() {
  pinMode(ONBOARD_LED,OUTPUT);
}
 
void loop() {
  delay(2000);
  digitalWrite(ONBOARD_LED,HIGH);
  delay(2000);
  digitalWrite(ONBOARD_LED,LOW);
}
