/**
 * Copyright (c) 2009 Andrew Rapp. All rights reserved.
 *
 * This file is part of XBee-Arduino.
 *
 * XBee-Arduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * XBee-Arduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with XBee-Arduino.  If not, see <http://www.gnu.org/licenses/>.
 */
 

#include <XBee.h>


/*
This example is for Series 1 XBee
Sends a TX16 or TX64 request with the value of analogRead(pin5) and checks the status response for success
Note: In my testing it took about 15 seconds for the XBee to start reporting success, so I've added a startup delay
*/

const unsigned int maxDancers = 10;
const unsigned char call = 'C';           // call for dancers in attendance 
const unsigned char present = 'P';        // I am here response
const unsigned char solo = 'S';           // dance a solo
const unsigned char ensembl = 'E';        // everyone dance
const unsigned char finished = 'F';       // peice has been completed

int statusLED = 13;              // our output for now
unsigned long last_marco = 0;    // the last time we sent out a marco call
unsigned long start;             // the milliseconds when the program started
unsigned int num_dancers = 0;    // number of dancers that responded to marco call
uint16_t dancers_present[maxDancers];    // array of responding dancers
// order to queue the dancers in, each dancer has a network card with an integer id in the 'MY' field 1-N
// The network cards will be called in this order.
// any dancer that has not chekced in during a curtin call
unsigned int dance_order[maxDancers]  = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };  // order to quey the dancers

XBee xbee = XBee();              // create an Xbee object to communite with the XBee card


// 1 byte to hold transmit and receive messages
uint8_t payload[] = { 0 };

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0xFFFE, payload, sizeof(payload));

// our response packet
TxStatusResponse txStatus = TxStatusResponse();

// Flash the pumpkin the given number of times with specified delay
void flashLed(int times, int wait) {
    for (int i = 0; i < times; i++) {
      digitalWrite(statusLED, HIGH);
      delay(wait);
      digitalWrite(statusLED, LOW);      
      //if (i + 1 < times) {
        delay(wait);
      //}
    }
}

void roll_call() {
  uint16_t dancer;
//  for (int dancer = 0x0001; dancer < 0x0009; dancer += 1)
//    tx.  
}


void setup() {
  start = millis();
  pinMode(statusLED, OUTPUT);
  Serial.begin(9600);
  xbee.setSerial(Serial);
  
  delay(15000);  // delay 15 seconds to let things settle down
  // roll_call()
}

void loop() {
  
    if (millis() - last_marco > 15000) {
      payload[0] = call;
      xbee.send(tx);
      flashLed(3, 500);
    }
   
  
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
        // got a response!

        // should be a znet tx status            	
    	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
            	// success.  time to celebrate
             	flashLed(4, 500);
           } else {
            	// the remote XBee did not receive our packet. is it powered on?
             	flashLed(5, 500);
           }
        }      
    } else if (xbee.getResponse().isError()) {
      flashLed(6, 500);
    } else {
      // remote XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      flashLed(7, 500);
    }
    
    delay(1000);
}
