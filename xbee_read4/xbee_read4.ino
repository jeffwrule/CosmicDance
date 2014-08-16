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

uint8_t option = 0;
uint8_t data = 0;

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
      xbee.getResponse().getRx16Response(rx16);
      option = rx16.getOption();
      data = rx16.getData(0);
      unsigned int flashes = data - '0';
      if (flashes >= 0 && flashes <= 9) 
        flashLed(13, flashes, 250);
    }
  }
}


