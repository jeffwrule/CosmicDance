// #include <Odometer.h>

class Odometer
{
  public:
    Odometer(int pinA, int pinB);
    long distance();
    void debug(boolean debug);
   
  private:
    void encodePinA();
    void encodePinB();
   
    int _pinA;
    int _pinB;
    boolean _A_set = false;
  boolean _B_set = false;
    boolean _debug;
    long _distance;
};

Odometer::Odometer(int pinA, int pinB)
{
  pinMode(pinA, INPUT);
  digitalWrite(pinA, HIGH);  // turn on pullup resistor
  _pinA = pinA;
 
  pinMode(pinB, INPUT);
  digitalWrite(pinB, HIGH);  // turn on pullup resistor
  _pinB = pinB;
 
  attachInterrupt(0, encodePinA, CHANGE);
  attachInterrupt(1, encodePinB, CHANGE);
 
  _distance = 0;
  _debug = false;
}

// Return Distance
long Odometer::distance(){
  return _distance;
}

// Interrupt on A changing state
void Odometer::encodePinA(){
  // Test transition
  _A_set = digitalRead(_pinA) == HIGH;
  // and adjust counter + if A leads B
  _distance += (_A_set != _B_set) ? +1 : -1;
 
  if (_debug) {Serial.println(_distance);}
}

// Interrupt on B changing state
void Odometer::encodePinB(){
  // Test transition
  _B_set = digitalRead(_pinB) == HIGH;
  // and adjust counter + if B follows A
  _distance += (_A_set == _B_set) ? +1 : -1;

  if (_debug) {Serial.println(_distance);}
}



Odometer MyOdometer(2, 3);

long prev_distance = 0;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  if (prev_distance != MyOdometer.distance()) {
    Serial.println(MyOdometer.distance());
    prev_distance = MyOdometer.distance();
  }

}
