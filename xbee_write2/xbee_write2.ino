
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

//  if (xbeeSerial.available()) {
      xbeeSerial.println("Hello World!");
//  }  
  delay(20); 
}

