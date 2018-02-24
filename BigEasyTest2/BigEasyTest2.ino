#define RPMS                50.0
#define GO_PIN                  11

const unsigned long STEPS_PER_REV         = 200;
const unsigned long MICROSTEPS_PER_STEP    =  0;
const unsigned long miscroseconds_per_seconds = 1000000;
const unsigned long seconds_per_minute = 60;
const uint32_t microseconds_per_microstep = 15000000;

uint32_t LastStepTime = 0;
uint32_t CurrentTime = 0;

#define stp 2
#define dir 3
#define MS1 4
#define MS2 5
#define MS3 6
#define EN  7

void setup() { 

  Serial.begin(9600);
  Serial.print("microseconds_per_microstep: ");
  Serial.println(microseconds_per_microstep);

  pinMode(stp, OUTPUT);
  pinMode(dir, OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(EN, OUTPUT);

  digitalWrite(stp, LOW); 
  digitalWrite(dir, LOW);
  
  digitalWrite(MS1, LOW); // 1/8 step HIGH,HIGH,LOW
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
  
  digitalWrite(EN, LOW);  // enable the motor to run
                 
  pinMode(GO_PIN,INPUT_PULLUP);
}

void loop() {
  if (true)
  {
    CurrentTime = micros();
    if (CurrentTime < LastStepTime) { LastStepTime = 0; }
    Serial.print("LastStepTime: ");
    Serial.print(LastStepTime);
    Serial.print(", Diff: ");
    Serial.println(CurrentTime - LastStepTime);
    if ((CurrentTime - LastStepTime) > microseconds_per_microstep)
    {
      Serial.print("Stepping ");
      LastStepTime = CurrentTime;
      digitalWrite(stp, HIGH);
      delayMicroseconds((microseconds_per_microstep * 0.9)/2);
      digitalWrite(stp, LOW); 
      delayMicroseconds((microseconds_per_microstep * 0.9)/2);
    }
  }
}
