#include <XBee.h>
#include <SoftwareSerial.h>

const unsigned int all_dancers = 0xFFFF;      // this is the address we use when we want to send to all dancers

// order to queue the dancers in, each dancer has a network card with an integer id in the 'MY' field 1-N
// The network cards will be called in this order.
// if a dancer does not respond back, the next dancer will be queued
unsigned int dance_order[]  = {1, 3, 5, 9, all_dancers};  // order to queue the dancers
unsigned int num_dancers = sizeof(dance_order) / sizeof(unsigned int);

/*
 * transmit and recieve pins for the xbee.
 * Note the pins jumpers on the xbee shield are configured just the opposite
 * Tx=5, Rx=10
 */
const unsigned int RxD = 5;
const unsigned int TxD = 10;

const int m_solo_delay_seconds = 5;        // number of seconds to delay for a solo in mini mode
const int m_ensembl_delay_seconds = 20;    // number of seconds to wait between ensemble play in mini mode
const int l_solo_delay_seconds = 5;        // number of seconds to delay for a solo in long mode
const int l_ensembl_delay_seconds = 150;    // number of seconds to wait between ensemble play long mode

/*
This example is for Series 1 XBee
Sends a TX16 or TX64 request with the value of analogRead(pin5) and checks the status response for success
Note: In my testing it took about 15 seconds for the XBee to start reporting success, so I've added a startup delay
*/

// director commands  
const unsigned char solo = 's';            // dance a solo
const unsigned char ensembl = 'e';         // everyone dance
const unsigned char halt = 'h';            // everyone stop what they are doing
const unsigned char whatchadoing = 'w';    // are you dancing or not?
const unsigned char mini_or_long = 'l';    // mini or long; change this to play short tracks 1/2 or long trancs 10/20  

// dancer responses 
const unsigned char finished = 'f';       // peice has been completed
const unsigned char acknowledge = 'a';    // I head you!
const unsigned char dancing = 'd';        // dancing

// when passed as expect, no response is waited for
const unsigned char empty_response = '\0'; // no response expected marker


unsigned long start;             // the milliseconds when the program started
unsigned long last_response;      // millis at time of last response

unsigned int current_dancer_position;      // which dancer is in the array is dancing
boolean is_dancing = false;                // is someone currently dancing
unsigned char dance_type;                  // which dance type to performe (solo or ensembl) 
unsigned int delay_after_dance_seconds;    // number of seconds to delay after this diance
unsigned int solo_delay_seconds;           // number of seconds a solo waits for next dance
unsigned int ensembl_delay_seconds;        // number of seconds an ensembl waits for next dance

// 1 byte to hold transmit messages
uint8_t payload[] = { 0 };
// 1 byte to hold receive messages
unsigned char answer;

byte  whatchadoing_misses;

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
boolean get_response(char expected, unsigned int timeout, char expected_alt='\0') {
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
        else if ( answer == expected || answer == expected_alt) {
          Serial.print("Got expected: ");
          Serial.println((char)answer);
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
      return NULL;
    }
  }
}

// read all the current packets that are available...
void flush_buffer() {
  Serial.println("Flush start...");
  get_response(empty_response, 400);
  Serial.println("Flush end...");
}

/*
 * send a command out and wait for the specified response, if not '\0'
 *   timeout is .5 seconds waiting for response
 */
boolean send_command(uint16_t addr, char command, char expect) {
  
  Serial.println("send_command starting...");
  
  // before sending a command clear the input buffer of any old junk 
  flush_buffer();
  
  // what is the command we are about to send.
  Serial.print("send_command: Starting command=");
  Serial.print(command);
  Serial.print(" who=");
  Serial.print(addr);
  Serial.print(" expect=");
  Serial.println(expect);
  
  // send the command out
  payload[0] = command;
  tx.setAddress16 (addr);
  xbee.send(tx);
  
  if (expect == empty_response) { 
    return true; 
  }
  // wait for the response 
  return get_response(expect, 4000);  // wait up to 4 seconds for a response  
}

void delay_next_dance() {
  
  if (delay_after_dance_seconds > 0) {
    if (dance_type == solo) {
      Serial.print("Solo");
    } else {
      Serial.print("Ensembl");
    }
    Serial.print(" dance just finished, waiting ");
    Serial.print(delay_after_dance_seconds);
    Serial.println(" seconds to restart...");
    delay(1000 * delay_after_dance_seconds); 
    delay_after_dance_seconds = 0;
  } 
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
  whatchadoing_misses = 0;
  delay_after_dance_seconds = 0;
  
  if (mini_or_long == 'm') {
    solo_delay_seconds = m_solo_delay_seconds;
    ensembl_delay_seconds = m_ensembl_delay_seconds;
  } else {
    solo_delay_seconds = l_solo_delay_seconds;
    ensembl_delay_seconds = l_ensembl_delay_seconds;    
  }
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
    // delay the next dance if we have not done that yet...
    delay_next_dance();
    
    queue_next_dance();
    Serial.print("Starting dancer: ");
    Serial.print(now_dancing());
    Serial.print(", Dance Type: ");
    Serial.print((char) dance_type);
    Serial.println();
    
    send_command(now_dancing(), mini_or_long, empty_response);
    delay(1000); // sleep for 1 second
    if (send_command(now_dancing(), dance_type, empty_response)) {
      is_dancing = true;
      last_response = millis();
      whatchadoing_misses = 0;
      switch(dance_type) {
        case ensembl: delay_after_dance_seconds = ensembl_delay_seconds; break;
        case solo: delay_after_dance_seconds = solo_delay_seconds; break;
      }
    }
    return;
  }
    
  // waiting for this dance to end....
  if (is_dancing == true ) {
    unsigned long now = millis();
    
    // check to see if we have finished....
    if (get_response(finished, 200, dancing)) {
      if (answer == finished) {
        Serial.println("Dancer finished!");
        is_dancing = false;
        return;
      }
      if (answer == dancing) {
        Serial.println("Updated last_repsonse, to now");
        last_response = now;
        whatchadoing_misses = 0;       
      }
    }
    // poke the dancer(s) after 15 seconds if it has been longer then that since anyone checked in...
    if ( now - last_response >= 40000) {
      Serial.print("15 seconds have passed, whatchadoing time, number of misses: ");
      Serial.println(whatchadoing_misses);
      if (send_command(now_dancing(), whatchadoing, dancing) == false) {
        whatchadoing_misses++;
        if (whatchadoing_misses > 1) {
          Serial.println("Too many misses, looks like noboday dancing, halt and next...");
          send_command(now_dancing(), halt, empty_response);
          is_dancing = false;
        }
        else {
          Serial.print("No response, but letting it go for now num_misses: ");
          Serial.println(whatchadoing_misses);
        }
        return;
      } 
      // we got a response from someone, we are still dancing....
      last_response = now;
      whatchadoing_misses = 0;
      return;
    }
  }
}

