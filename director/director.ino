#include <XBee.h>
#include <SoftwareSerial.h>

/*
 * transmit and recieve pins for the xbee.
 * Note the pins jumpers on the xbee shield are configured just the opposite
 * Tx=5, Rx=10
 */
const unsigned int RxD = 5;
const unsigned int TxD = 10;
const unsigned int delay_seconds = 20;    // number of seconds to wait to restart cycle

/*
This example is for Series 1 XBee
Sends a TX16 or TX64 request with the value of analogRead(pin5) and checks the status response for success
Note: In my testing it took about 15 seconds for the XBee to start reporting success, so I've added a startup delay
*/

// director commands  
const unsigned char solo = 's';         // dance a solo
const unsigned char ensembl = 'e';      // everyone dance
const unsigned char halt = 'h';         // everyone stop what they are doing
const unsigned char whatchadoing = 'w';  // are you dancing or not?

// dancer responses 
const unsigned char finished = 'f';       // peice has been completed
const unsigned char acknowledge = 'a';    // I head you!
const unsigned char dancing = 'd';        // dancing

// when passed as expect, no response is waited for
const unsigned char empty_response = '\0'; // no response expected marker

const unsigned int all_dancers = 0xFFFF;      // this is the address we use when we want to send to all dancers

unsigned long start;             // the milliseconds when the program started
unsigned long last_response;      // millis at time of last response

// order to queue the dancers in, each dancer has a network card with an integer id in the 'MY' field 1-N
// The network cards will be called in this order.
// if a dancer does not respond back, the next dancer will be queued
unsigned int dance_order[]  = { 1, 7, all_dancers };  // order to queue the dancers
unsigned int num_dancers = sizeof(dance_order) / sizeof(unsigned int);

unsigned int current_dancer_position;      // which dancer is in the array is dancing
boolean is_dancing = false;       // is someone currently dancing
unsigned char dance_type;         // which dance type to performe (solo or ensembl) 


// 1 byte to hold transmit messages
uint8_t payload[] = { 0 };
// 1 byte to hold receive messages
unsigned char answer;

// create an Xbee object to communite with the XBee card
XBee xbee = XBee();             

// 16-bit transmit request
Tx16Request tx = Tx16Request(0xFFFE, payload, sizeof(payload));
// 16-bit recieve frame
Rx16Response rx16 = Rx16Response();
// 16-bit status response
TxStatusResponse txStatus = TxStatusResponse();

// software serial port
SoftwareSerial xbeeSerial(RxD, TxD); // RX, TX

// switch to the next dancer 
void queue_next_dance() {
  current_dancer_position = current_dancer_position + 1;
  if (current_dancer_position >= num_dancers ) { 
    current_dancer_position = 0;
  }
  if (now_dancing() == all_dancers) {
    dance_type = ensembl;
  } else {
    dance_type = solo;
  }
}
      
// return network address of the current dancer
uint16_t now_dancing() {
    return dance_order[current_dancer_position];
}

// read responses until we get the one we are looking for
// or there are no reply with in the given timeout
boolean get_response(char expected, unsigned int timeout) {
  // read until we get what we want or noting left to read
  // Serial.print("get_repsonse starting: expect=");
  // Serial.print(expected);
  // Serial.print(" timeout=");
  // Serial.println(timeout);
  while (true) {
    xbee.readPacket(timeout);
    if (xbee.getResponse().isAvailable()) {
      // got something
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
        xbee.getResponse().getRx16Response(rx16);
        answer = rx16.getData(0);
        Serial.print("get_response:Got something: ");
        Serial.println((char) answer);
        // we are hear to just remove any values from the RX buffer
        if (expected == empty_response) {
          Serial.print("Clearing unwanted value: ");
          Serial.println((char)answer);
          } 
        else if ( answer == expected) {
          Serial.println("Got expected");
          return true;
          } 
        else {
          Serial.print("Got unexpected: ");
          Serial.println((char)answer);           
        }
      }
    } else {
      // Serial.println("get_repsonse:Read timeout");
      // no data available after timeout seconds, so sorry la
      return false;
    }
  }
}

// read all the current packets that are available...
void flush_buffer() {
  Serial.println("Flush start...");
  get_response(empty_response, 200);
  Serial.println("Flush end...");
}

/*
 * send a command out and wait for the specified response, if not '\0'
 *   timeout is .5 seconds waiting for response
 */
boolean send_command(uint16_t addr, char command, char expect) {
  
  Serial.print("send_command: Starting command=");
  Serial.print(command);
  Serial.print(" who=");
  Serial.print(addr);
  Serial.print(" expect=");
  Serial.println(expect);
  
  // before sending a command clear the input buffer of any old junk 
  flush_buffer();
  // send the command out
  payload[0] = command;
  tx.setAddress16 (addr);
  xbee.send(tx);
  
  if (expect == empty_response) { 
    return true; 
  }
  // wait for the response 
  return get_response(expect, 4000);  // wait up to 0.5 seconds for a response  
}

void setup() {
  start = millis();            // when the program first started
  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  
  // setup the soft serial port for xbee reads and writes
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  
  current_dancer_position = -1;
  is_dancing = false;
  
  // just let it settle for a bit
  delay(10000);  // delay 15 seconds to let things settle down
  send_command(all_dancers, halt, empty_response);
  Serial.println("End setup");
}

// start and stop dancers, keep the show moving along....
void loop() {
    // protect against millis rollover
    if (start > millis()) {start = millis(); last_response = start; }
    
  /*
   * start the next dancer 
   */
  if (is_dancing == false ) {
    queue_next_dance();
    Serial.print("Starting dancer: ");
    Serial.print(now_dancing());
    Serial.print(", Dance Type: ");
    Serial.print((char) dance_type);
    Serial.println();
    
    if (send_command(now_dancing(), dance_type, acknowledge)) {
      is_dancing = true;
      last_response = millis();
    }
    return;
  }
    
  // waiting for this dance to end....
  if (is_dancing == true ) {
    unsigned long now = millis();
    
    // check to see if we have finished....
    if (get_response(finished, 200)) {
      Serial.println("Dancer finished!");
      is_dancing = false;
      // wait 3 minutes between sessions...
      if (now_dancing() == all_dancers) {
        Serial.print("Ensembl just finished, waiting ");
        Serial.print(delay_seconds);
        Serial.println(" seconds to restart...");
        delay(1000 * delay_seconds);
      }
      return;
    }
    // check in every 10 seconds to see if our dancer is still performing
    if ( now - last_response > 10000) {
      Serial.println("last_response timeout, whatchadoing time...");
      if (send_command(now_dancing(), whatchadoing, dancing) == false) {
        Serial.println("Not dancing, halt and next...");
        send_command(now_dancing(), halt, empty_response);
        is_dancing = false;
        return;
      }
      // we got a response from someone, we are still dancing....
      last_response = now;
      return;
    }
  }
}

