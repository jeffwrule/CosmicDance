
/* works with tx jumper=0, rx jumper=1 */
void setup() {
   Serial.begin(9600);
}

void loop() {

  Serial.println("Hello World!");
  delay(200); 
}

