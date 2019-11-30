/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogInput
*/

int sensorPin = A0;    // select the input pin for the potentiometer
int digitalPin = A5;
int ledPin = 13;      // select the pin for the LED
float sensorValue = 0;  // variable to store the value coming from the sensor
float digitalValue = 0;
int const read_trigger_value=130; // value above which we had the sensor triggered.

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(sensorPin, INPUT);
  pinMode(digitalPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(250000);
}

void loop() {
  // static variables 
  unsigned long static i=0;
  unsigned long static next_print=0;
  unsigned long static next_read=0;   // what millis to wait for next read
  unsigned long static const wait_next=3000;  // 3000 milliseconds, 3 seconds
  boolean static is_triggered=false;
  // local variables
  unsigned long cur_millis = millis(); // current milliseconds


  if (cur_millis >= next_print ) {
      next_print = cur_millis + 1000;
      Serial.print(cur_millis);
      Serial.print(" , ");
      Serial.print(next_read);
      Serial.print(" , ");
      Serial.print(next_print);
      Serial.print(" , ");
      Serial.print(is_triggered);
      Serial.print(" Sensor Pin read ");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(sensorValue);
      Serial.print(" , ");
      Serial.println(digitalValue);
  }

  // read the value from the sensor and take an average reading...
  if ((is_triggered == false && cur_millis >= next_read)) {
    i++;
    sensorValue=0;
    digitalValue=0;
    for (int i=0; i<5; i++) {
      sensorValue = sensorValue + analogRead(sensorPin);
      digitalValue = digitalValue + analogRead(digitalPin);
    }
    sensorValue = sensorValue / 5;
    digitalValue = digitalValue / 5;
    // if (digitalValue >= 1.5 ) digitalValue = 1;
    if (sensorValue >= read_trigger_value ) {
        Serial.println("Sensor Triggered...");
        digitalWrite(ledPin, HIGH);
        next_read = cur_millis + wait_next;
        is_triggered = true;
     } else {
      next_read = cur_millis + 10;   // wait 100 milliseconds between reads
     }
  } else {
    if ( cur_millis >= next_read) {
       digitalWrite(ledPin, LOW);
       is_triggered=false;  
       Serial.println("Read Restored");    
    } 
  }

}
