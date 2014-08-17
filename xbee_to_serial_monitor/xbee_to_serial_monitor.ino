
#include <SoftwareSerial.h>
//#define RxD 4
//#define TxD 5

/* note the pins on the xbee shied are just the opposite
 * Tx=5, Rx=10
 */
#define RxD 05 
#define TxD 10

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
    if (Serial.available() > 0) {
      xbeeSerial.write(Serial.read());
    }
}

