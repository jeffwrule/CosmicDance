#ifndef MOTOR_H
#define MOTOR_H

class Motor {
  private:

    uint8_t pin;
    boolean active;

  public:

    void start();
    void stop();
    boolean is_running();           // turn boolean to see if motor is or is not running
    void    display();         // dump the details about this motor

    Motor(uint8_t pin) : pin(pin) {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      active=false;
    }


};

void Motor::start() { digitalWrite(pin, HIGH);  active=true; }

void Motor::stop()  { digitalWrite(pin, LOW);   active=false; }

boolean Motor::is_running() { return active; }

void Motor::display() {
  Serial.print(F("Motor: pin="));
  Serial.print(pin);
  Serial.print(F(", active="));
  Serial.print(active);
  Serial.println(F(""));
}

#endif
