
int delay_after_dance_seconds = 450;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");
  delay(3000);
  Serial.println("End setup");
}


void delay_seconds( int seconds_to_delay) {
  unsigned long num_passed = 0;
  unsigned long start_millis;
  unsigned long cur_millis;

  start_millis = millis();
  
  while (seconds_to_delay > (num_passed / 1000)) {
    cur_millis = millis();
    
    // when the millis wrapps around
    if (cur_millis < start_millis) {
      start_millis = cur_millis;
    }
    
    num_passed = cur_millis - start_millis;
  }
}
void loop() {
  // put your main code here, to run repeatedly:

  Serial.println("Delaying 5 seconds");
  delay_seconds(5);
  Serial.println("Delaying 10 seconds");
  delay_seconds(10);
  Serial.println("Delay 60 seconds");
  delay_seconds(60);
}
