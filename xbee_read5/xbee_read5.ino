/*
 * read a pack in api mode and 
 * turn on/off the pumpkin when bytes '1'/'0' arrive
 */
#include <XBee.h>
#include <SoftwareSerial.h>

#define RxD 4
#define TxD 5

SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();

uint8_t data = 0;

const unsigned char roll_call = 'R';           // call for dancers in attendance 
const unsigned char present = 'P';        // I am here response
const unsigned char solo = 'S';           // dance a solo
const unsigned char ensembl = 'E';        // everyone dance
const unsigned char finished = 'F';       // peice has been completed

/* works with tx jumper=4, rx jumper=5 */
void setup() {
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(13, OUTPUT);

  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
}

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

void loop() {

  xbee.readPacket(100);  // wait for a package for 100ms
  if (xbee.getResponse().isAvailable()) {
    // got something
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
      // get the one byte data
      xbee.getResponse().getRx16Response(rx16);
      data = rx16.getData(0);
      // this is a present response
      if (data == 'P') {
        // who is present, get thier 16bit address
        uint16_t dancer_addr = rx16.getRemoteAddress16();
        // only 10 dancers expected.... 1..10
        if (dancer_addr < 1 || dancer_addr > 10 ) {
           // fast error flash
           flashLed(13,10,50);
        } else {
          // who did we find, flash your id number of times.
          flashLed(13, dancer_addr, 250);
        }
      }
    }
  }
}


