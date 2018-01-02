#define pulseMicroSecondDelay 20000  // microseconds delay starting value
#define stepDown 100  // decrement this much for each test

// current delay
unsigned long delayMicroSeconds = pulseMicroSecondDelay;

// delay any number of micro seconds

void myUSecondDelay(long usDelay)
{
  long small_delay;
  long large_delay;

  if (usDelay > 16000) {
      small_delay = usDelay % 1000;
      large_delay = usDelay / 1000;
      delay(large_delay);
      delayMicroseconds(small_delay);
  } else {
    delayMicroseconds(usDelay);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Setup complete");
}

unsigned long before;   // ms before delay
unsigned long after;    // ms after dealy
unsigned long diff;     // ms difference between before and after

void loop() {
    //collect the timings.
    before = micros();
    // delayMicroseconds(delayMicroSeconds);
    myUSecondDelay(delayMicroSeconds);
    after = micros();
    diff = after - before;
    
    // print some diagnostics, after we have collected our stats
    Serial.print("Delay:");
    Serial.print(delayMicroSeconds);
    Serial.print(", ms before:");
    Serial.print(before);
    Serial.print(", ms after:");
    Serial.print(after);
    Serial.print(", ms diff:");
    Serial.print(diff);
    Serial.print(", percent of expected: ");
    Serial.println((diff/(delayMicroSeconds*1.0)) * 100.0, 4);
    

    // decrement the delay, or loop around to the top when we get <= our step value
    if (delayMicroSeconds <= stepDown ) {delayMicroSeconds = pulseMicroSecondDelay; } else { delayMicroSeconds -= stepDown; }

}
