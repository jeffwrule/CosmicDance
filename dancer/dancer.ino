
const int statusLED = 13;

// incomming commands
const unsigned char marco = 'M';    // marco 
const unsigned char queue = 'Q';    // start dancing
const unsigned char halt  = 'H';    // stop dancing

// outgoing commands
const unsigned char polo = 'P';     // polo
const unsigned char solo = 'S';     // performing solo
const unsigned char fine = 'F';     // finished dancing
const unsigned char resting = 'R';  // I am resting

const unsigned long SONG_LENGTH = 15000;  // simulate a 15 second song
const unsigned long CHECKIN_PERIOD = 3000 // check in every 3 seconds during playback

boolean performing = false;
unsigned long time_started = 0;  // when music was queueud
unsigned long last_checkin = 0;  // last checing during performance

void setup() {
  Serial.begin(9600);    	//initialize serial
  pinMode(13, OUTPUT);   	//set pin 13 as output
  delay(15000);  // delay 15 seconds to let things settle down
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
  
  while(Serial.available()){  //is there anything to read?
	char getData = Serial.read();  //if yes, read it

	if(getData == marco) {
          serial.write(polo);
          serial.flush();
          flashLed(statusLED, 3, 100);  	 
  	  digitalWrite(13, HIGH);
	}else if(getData == queue ) {
          // start music
          // start motor
          performing = true;
          time_started = millis();
          last_checkin  = millis();
          serial.write(solo);
          flashLed(statusLED, 4, 100);
       } else if (getData == halt) {
         // stop music
         // stop motor
         serial.write('F');
         flastLed(statusLED, 5, 100);
       }
  }
  
  // check in from time to time
  if (millis() - last_checkin > CHECKIN_PERIOD) {
    if (performing) {
      serial.write(solo);
    } else {
      serial.write(resting);
    }
    serial.flush();
  }
      
  // some house keeping during playback mode
  if (performing) {
    if (millis() - time_started > SONG_LENGTH) {
      // stop montor
      serial.write(fine);
      flash(statusLED, 5, 100);
    }
  }
   
}
