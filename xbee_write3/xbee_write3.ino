/* 
 * this is a demo script that will try and write to the
 * xbee API
 */

#include <SoftwareSerial.h>
#include <XBee.h>

#define RxD 4
#define TxD 5

const unsigned char call = 'C';           // call for dancers in attendance 
const unsigned char present = 'P';        // I am here response
const unsigned char solo = 'S';           // dance a solo
const unsigned char ensembl = 'E';        // everyone dance
const unsigned char finished = 'F';       // peice has been completed

SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX

XBee xbee = XBee();              // create an Xbee object to communite with the XBee card
uint8_t payload[] = { 0 };

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0xFFFF, payload, sizeof(payload));
// our response packet
TxStatusResponse txStatus = TxStatusResponse();


/* works with tx jumper=4, rx jumper=5 */
void setup() {
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
}

void loop() {
  payload[0] = call;
  xbee.send(tx);
  delay(20); 
}

