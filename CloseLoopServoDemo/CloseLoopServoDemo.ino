
#define pulsePin 7
#define dirPin 6
#define pendingPin 3
#define alarmPin 4
#define enablePin 9

#define oneRev 40000
#define pulseMicroSecondDelay 40  // must be much longer if you use fewer steps..  

//#define oneRev 400
// #define pulseMicroSecondDelay 150  // must be much longer if you use fewer steps.. 


#define checkServoReadyInterval 100L 

unsigned long microDelay=pulseMicroSecondDelay;
boolean servoReady=false;
boolean servoAlarm=false;
unsigned long stepsToGo;
unsigned long loopCounter=0;
unsigned long printInterval=50000;
boolean doPrint=true;
long unsigned numInterrupts=0;
long unsigned numReadyCheck=0;

// delay any number of uSeconds w/i a reasonable accuracy rate
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

void alarmWather()
{
  
}

void stepWatcher()
{
  delayMicroseconds(5);
  servoReady=digitalRead(pendingPin) == 0;
  //servoReady=true;
  numInterrupts += 1;
  // if we are getting servo ready problems increase the delay
  //  if (! servoReady) {
  //    microDelay *= 1.1;
  //  }
  //Serial.print("ServoReady:");
  //Serial.println(servoReady);
}

// disable the servo
void disableServo() {
  digitalWrite(enablePin, HIGH);
  Serial.println("Servo Disabled");
  delay(2000);
  servoReady=false;
}

// enable the servo
void enableServo()
{
  digitalWrite(enablePin, LOW);
  Serial.println("Servo Enabled");
  delay(1000);
  servoReady=true;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);          // setup the interal serial port for debug messages
  Serial.println("Start setup");

  pinMode(enablePin, OUTPUT);
  disableServo();
  
  pinMode(pulsePin, OUTPUT);
  digitalWrite(pulsePin, LOW);

  pinMode(dirPin, OUTPUT);
  digitalWrite(dirPin, LOW);
  
  pinMode(pendingPin, INPUT_PULLUP);
  pinMode(alarmPin, INPUT_PULLUP);

  enableServo();

  attachInterrupt(0, stepWatcher, RISING);

  stepsToGo=oneRev;

  Serial.println("End Setup");
}

void loop() {

  
  if (doPrint && (loopCounter % printInterval) == 0) {
    Serial.print("servoReady:");
    Serial.print(servoReady);
    Serial.print(", numInterrupts:");
    Serial.print(numInterrupts);
    Serial.print(", numReadyCheck:");
    Serial.print(numReadyCheck);
    Serial.print(", stepsToGo:");
    Serial.print(stepsToGo);
    Serial.print(", loopCounter:");
    Serial.print(loopCounter);
    Serial.print(", microDelay:");
    Serial.println(microDelay);
  }

  // just check every now and then 
  if (!servoReady && (loopCounter % checkServoReadyInterval) == 0) {
      servoReady = digitalRead(pendingPin) == 0;
      numReadyCheck += 1;
      Serial.print("Checkready:");
      Serial.println(servoReady);     
  }

  loopCounter += 1;

  
//  // did we error out?
//  if (digitalRead(alarmPin) == 0) {
//    Serial.println("Alarm!!");
//    disableServo();
//    enableServo();
//    return; // restart loop, this will esentially break until a reset
//  }

  // step the motor
  if (servoReady == true && stepsToGo > 0) {
    //servoReady=false;
    stepsToGo -= 1;
    // Serial.println("pulse:ServoReady=false");
    digitalWrite(pulsePin, HIGH);
    delayMicroseconds(pulseMicroSecondDelay);
    digitalWrite(pulsePin, LOW);
    delayMicroseconds(pulseMicroSecondDelay);
  }

  if (servoReady == true && stepsToGo == 0) { 
    digitalWrite(dirPin, digitalRead(dirPin) ^ 1); 
    stepsToGo=oneRev;    
    Serial.println("Wait 2 seconds");
    delay(pulseMicroSecondDelay);
  }
}
