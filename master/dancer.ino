#include <XBee.h>

XBee xbee = XBee();

unsigned long start = millis();
/*
 * Dancer commands
 */
const char marco = 'M';       // I am here (from dancer)
const char polo = 'P';        // I heard you (from director)
const char da_capo = 'D';     // play from beginning to end
const char fine = 'F';        // the song has ended
const int director = 0x0000;    // the coordinator node address is 0x0000
const int  statusLed = 13;

uint8_t payload[] = {0};            // all the messages are a single character
unsigned long last_shoutout = 0;  // last time we checked in with director

Tx16Request tx = Tx16Request(director, payload, sizeof(payload));
TxStatusResponse txStatus = TxStatusResponse();

void setup() {
  pinMode(statusLed, OUTPUT);
  Serial.begin(9600);
  xbee.setSerial(Serial);
  delay(15000);            // wait 15 seconds for xbee to come online
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

// tell the director we are out here
void sendMessage() {
      
    xbee.send(tx);  // marco!
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
             	flashLed(statusLed, 10, 50);
           } else {
            	// the remote XBee did not receive our packet. is it powered on?
             	flashLed(statusLed, 3, 500);
           }
        }      
    } else if (xbee.getResponse().isError()) {
      flashLed(statusLed, 4, 500);
    } else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      flashLed(statusLed, 5, 550);
    }    
    
}

void loop() {
  
    // give a soutout every 10 seconds
    if (last_shoutout == 0 || millis() - last_shoutout > 10000) {
      payload[0] = marco;
      sendMessage();
      last_shoutout = millis();
    }

}

  
    

