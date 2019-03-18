/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
#define MOTION_PIN 8
#define INDICATOR_LED 2
// the setup function runs once when you press reset or power the board
int last_motion_read=0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(250000);
  pinMode(INDICATOR_LED, OUTPUT);
  pinMode(MOTION_PIN, INPUT);
  last_motion_read=0;
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
}

unsigned long start_millis;

// the loop function runs over and over again forever
void loop() {
  int motion_value;
  unsigned long cur_millis;
  
  motion_value = digitalRead(MOTION_PIN); 
  if (last_motion_read != motion_value) {
    start_millis = millis();
    Serial.println(F("state changed, resetting counter!"));
    digitalWrite(INDICATOR_LED, motion_value);   // turn the LED on (HIGH is the voltage level)
  }

  cur_millis=millis();
  last_motion_read = motion_value;


       
  Serial.print("pin D2=");
  Serial.print(motion_value);
  Serial.print(", Current Value=");
  Serial.print(motion_value);
  Serial.print(F(", Seconds in state="));
  Serial.println((cur_millis - start_millis) / 1000.0);
  
}
