
#include <SoftwareSerial.h>
#define RxD 4
#define TxD 5

SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX

/* works with tx jumper=4, rx jumper=5 */
void setup() {
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  xbeeSerial.begin(9600);
}

void loop() {

//    xbeeSerial.listen();
    
    if (xbeeSerial.available() > 0) {
      char x = xbeeSerial.read();
      if (x == '1') {
        digitalWrite(13, HIGH);
      }
      else if (x == '0') {
        digitalWrite(13, LOW);
      }  
    }  
}

