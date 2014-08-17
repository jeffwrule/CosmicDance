
#include <SoftwareSerial.h>
//#define RxD 4
//#define TxD 5

#define RxD 11
#define TxD 12

SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX

/* works with tx jumper=4, rx jumper=5 */
void setup() {
  Serial.begin(9600);
  
  // set up the softserial
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  xbeeSerial.begin(9600);
}

void loop() {

//    xbeeSerial.listen();
    
    if (xbeeSerial.available() > 0) {
      Serial.write(xbeeSerial.read());
    }  
}

