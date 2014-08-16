
/* works with tx jumper=0, rx jumper=1 */
void setup() {
   Serial.begin(9600);
   pinMode(13, OUTPUT);
}

void loop() {

    if (Serial.available() > 0) {
      char x = Serial.read();
      if (x == '1') {
        digitalWrite(13, HIGH);
      }
      else if (x == '0') {
        digitalWrite(13, LOW);
      }  
    }  
}

