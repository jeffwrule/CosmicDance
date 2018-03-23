#ifndef PWMMOTOR_H
#define PWMMOTOR_H

#include </Users/jrule/Documents/Arduino/TimedMotor/GenericMotor.h>

class PWMMotor: public GenericMotor {
    public:
      void stop();
      void start();
      void run();
      boolean get_is_stopped();
      void status(boolean do_print, unsigned long current_millis);
      char *get_motor_name();
      void set_start_speed(unsigned int new_start_speed);
      void set_target_speed(unsigned int new_target_speed);
      unsigned short get_target_speed();
      unsigned short get_max_speed();
      virtual int get_subclass();

  // const char *motor_name           // the name of this motor
  // unsigned short pwm_pin           // the pin to control this motor
  // unsigned int  motor_start_speed  // what speed to start the motor at
  // unsigned int  speed_increment    // speed up/down increments when adjusting motor speed.
  // increment_delay_millis           // how long to wait between speed changes 
  PWMMotor (
        const char *motor_name,       // the name of this motor
        unsigned short pwm_pin,
        unsigned int  motor_start_speed,
        unsigned int  speed_increment,
        unsigned int  increment_delay_millis
        ) : 
    motor_name(motor_name),
    pwm_pin(pwm_pin),
    motor_start_speed(motor_start_speed),
    speed_increment(speed_increment), 
    increment_delay_millis(increment_delay_millis)
    {
      analogWrite(pwm_pin, 0);
      pinMode(pwm_pin, OUTPUT);
      analogWrite(pwm_pin, 0);

      is_stopped = true;
      speed = 0;
      last_speed_change_millis = 0;

      Serial.print(F("PWMMotor::CONSTRUCTOR motor_name="));
      Serial.print(motor_name);
      Serial.print(F(", pwm_pin="));
      Serial.print(pwm_pin);
      Serial.print(F(", motor_start_speed="));
      Serial.print(motor_start_speed);
      Serial.print(F(", speed_increment="));
      Serial.print(speed_increment);
      Serial.print(F(", increment_delay_millis="));
      Serial.println(increment_delay_millis);
    }

  private:
    unsigned short pwm_pin;
    const char *motor_name;
    boolean is_stopped;
    unsigned int speed;               // the current motor speed
    unsigned int target_speed;        // the speed we want to get to
    unsigned int motor_start_speed;   // initial speed to start the motor at
    unsigned int  speed_increment;    // speed rate to increment the speed
    unsigned int last_speed_change_millis;  // last time we incremented the speed
    unsigned long increment_delay_millis; // when did we last change speed

    const unsigned short max_speed=255;
};

int PWMMotor::get_subclass() {
  return SUB_CLASS_PWMMOTOR;
}

// stop this motor quickly
void PWMMotor::stop() {

  Serial.print(F("PWMMotor::stop motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", speed="));
  Serial.print(speed);
  Serial.println(F(", new_target_speed=0"));

  target_speed=0;
  unsigned int old_increment_delay_millis = increment_delay_millis;
  increment_delay_millis = 0;
  while (speed != 0 ) { run(); }  
  increment_delay_millis = old_increment_delay_millis;
}

// get this motor going
void PWMMotor::start() {

    Serial.print(F("PWMMotor::start motor_name="));
    Serial.print(motor_name);
    Serial.print(F(", speed="));
    Serial.print(speed); 
    Serial.print(F(", target_speed="));
    Serial.println(target_speed);

    is_stopped = false;         // let the run routine start moving the motor

    digitalWrite(pwm_pin, speed);     // make sure the motor is started here at the current speed

}

// keep increasing or decreasing the motor speed until we reach our desired speed
void PWMMotor::run() {
  
  if ( !is_stopped ) {
    unsigned long current_millis = millis();

    // have to wait until the delay has passed to update the speed
    if (current_millis - last_speed_change_millis < increment_delay_millis) { return; } 

    // make sure our pins are active if we are going to changing speed
    if (speed != target_speed) {
      last_speed_change_millis = current_millis;
    }
    
    // some bounds checking on target speed
    if (target_speed > 0 && target_speed < motor_start_speed) { target_speed = motor_start_speed; } // can't have a target (other then 0, below start speed 
    if (target_speed > max_speed) {target_speed = max_speed; }                          // can't have a target speed greater then max_speed

    // increment speed
    if (speed < target_speed) {
      speed += speed_increment;
      if (speed < motor_start_speed) { speed = motor_start_speed; }   // always start at a minimum here
      if (speed > target_speed) { speed = target_speed; } // don't go above max
      analogWrite(pwm_pin, speed);
      Serial.print(F("HBridgeMotor::run motor_name="));
      Serial.print(motor_name);
      Serial.print(F(" speed INCREASED to: "));
      Serial.println(speed);
    }

    // decrement speed
    if (speed > target_speed) {
      speed -= speed_increment;
      if (speed < motor_start_speed) { speed = 0; }
      analogWrite(pwm_pin, speed);
      Serial.print(F("HBridgeMotor::run motor_name="));
      Serial.print(motor_name);
      Serial.print(F("speed DECREASED to: "));
      Serial.println(speed);
    }
  }  
}

boolean PWMMotor::get_is_stopped() {
  return is_stopped;
}

void PWMMotor::status(boolean do_print, unsigned long current_millis) {
  if ( !do_print ) { return; }

  Serial.print(F("PWMMotor::status() motor_name="));
  Serial.print(motor_name);
  Serial.print(F(", is_stopped="));
  Serial.print(bool_tostr(is_stopped));
  Serial.print(F(", speed="));
  Serial.print(speed);
  Serial.print(F(", motor_start_speed="));
  Serial.print(motor_start_speed);
  Serial.print(F(", target_speed="));
  Serial.print(target_speed);
  Serial.print(F(", last_speed_change_millis="));
  Serial.print(last_speed_change_millis);
  Serial.print(F(", last_speed_change_millis_diff="));
  Serial.println(current_millis - last_speed_change_millis);
}

char* PWMMotor::get_motor_name() {
  return motor_name;
}

void PWMMotor::set_start_speed(unsigned int new_start_speed) {
  Serial.print(F("PWMMotor::set_start_speed() motor_start_speed="));
  Serial.print(motor_start_speed);
  Serial.print(F(", new_start_speed="));
  Serial.println(new_start_speed);
  motor_start_speed = new_start_speed;  
}

void PWMMotor::set_target_speed(unsigned int new_target_speed) {
  Serial.print(F("PWMMotor::set_target_speed() target_speed="));
  Serial.print(target_speed);
  Serial.print(F(", new_target_speed="));
  Serial.println(new_target_speed);
  target_speed = new_target_speed;
}

unsigned short PWMMotor::get_target_speed() {
  return target_speed;
}

unsigned short PWMMotor::get_max_speed() {
  return max_speed;
}

#endif
