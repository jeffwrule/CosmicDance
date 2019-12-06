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
      Serial.println(F("Motor Constructor Starting"));
      pinMode(LED_BUILTIN, OUTPUT);
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      active=false;
    }


};

void Motor::start() { 
      Serial.println(F("%%%% MOTOR STARTED %%%")); 
      #ifdef DISABLE_MOTOR
        Serial.println(F("    %%% Motor Disabled %%%"));
      #else=
        digitalWrite(pin, HIGH);
      #endif 
      digitalWrite(LED_BUILTIN, HIGH); active=true; 
      }

void Motor::stop()  { 
  Serial.println(F("%%%% MOTOR STOPPED %%%")); 
  digitalWrite(pin, LOW);   
  digitalWrite(LED_BUILTIN, LOW); 
  active=false; }

boolean Motor::is_running() { return active; }

void Motor::display() {
  Serial.print(F("Motor::display() Motor: pin="));
  Serial.print(pin);
  Serial.print(F(", active="));
  Serial.print(active);
  Serial.println(F(""));
}

#endif
