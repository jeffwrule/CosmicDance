//#define RxD 4
//#define TxD 5

/* works with tx jumper=4, rx jumper=5 */
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // this is the xbee
}

void loop() {

//    xbeeSerial.listen();
    
    if (Serial1.available() > 0) {
      Serial.write(Serial1.read());
    } 
    if (Serial.available() > 0) {
      Serial1.write(Serial.read());
    } 
}
