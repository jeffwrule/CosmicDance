/*
 * Example using non-blocking mode to move until a switch is triggered.
 *
 * Copyright (C)2015-2017 Laurentiu Badea
 *
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */
#include <Arduino.h>

// only print every so often as it screws up timing
#define PRINT_EVERY 10000L
// uncomment the next line to use a simple switch between DANCER_PIN and ground, not normal mode.
//#define DANCER_IS_PULLUP    ' 
unsigned long loop_count=0;


// this pin should connect to Ground when want to stop the motor
#define STOPPER_PIN 4
// PIN to drive mosfet for stop table
#define TOP_MOTOR_PIN 10

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200L
#define RPM 2
// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 128L

#define DIR_PIN 8
#define STEP_PIN 9
#define ENABLE_PIN 13 // optional (just delete ENABLE from everywhere if not used)

#define DOWN_DIR ExtendedBasicStepperDriver::DOWN_DIRECTION
#define UP_DIR ExtendedBasicStepperDriver::UP_DIRECTION

#include "BasicStepperDriver.h" // generic
class ExtendedBasicStepperDriver : public BasicStepperDriver {
  
  public:
    ExtendedBasicStepperDriver(short steps, short dir_pin, short step_pin, short enable_pin) : 
      BasicStepperDriver(steps, dir_pin, step_pin, enable_pin)
    {}

  enum Direction {UP_DIRECTION, DOWN_DIRECTION};

  // the number of steps to our goal absolute value
  long getStepsRemaining() {
    return(steps_remaining);
  }
  // the number of steps taken to our goal absolute value
  long getStepCount() {
    return(step_count);
  }
  // the current direction we are traveling.
  Direction getDirection() {
    return(dir_state == HIGH ? UP_DIRECTION : DOWN_DIRECTION);
  }
  
};

//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, ENABLE);
ExtendedBasicStepperDriver stepper(MOTOR_STEPS, DIR_PIN, STEP_PIN, ENABLE_PIN);

int last_stop_read;

// total number of steps to the top (we are going to go small at first, the bigger)
#define REVOLUTIONS_UP 2L
// same as up + 1 to make sure it comes all the way down.  The sensor switch will stop it.
#define REVOLUSTIONS_DOWN (REVOLUTIONS_UP + 1)
#define STEPS_UP (MOTOR_STEPS * MICROSTEPS * REVOLUTIONS_UP)
#define STEPS_DOWN (MOTOR_STEPS * MICROSTEPS * REVOLUSTIONS_DOWN * -1)
#define DANCER_PIN 3  // input from other arduino, are we dancing?

// wait time when starting from home so the switch is off
#define HOME_WAIT 1000 

class Dancer {
  
protected:

  int const dancer_pin;               // pin to read for dancer
  int const spinning_pin;             // top motor pin  
  int const stopper_pin;              // stopper pin
  boolean is_dancing;                 // true of dancing pin is HIGH
  boolean at_stopper;                 // true if stop_pin is low. 
  boolean is_spinning;                // top is spinning
  String state_error="";          // we found a state error, only so far is stuck stopper

public: 

  Dancer(int spinning_pin, int dancer_pin, int stopper_pin) : 
      spinning_pin(spinning_pin), dancer_pin(dancer_pin), stopper_pin(stopper_pin)
    {
      pinMode(spinning_pin, OUTPUT);
      #ifdef DANCER_IS_PULLUP
        pinMode(dancer_pin, INPUT_PULLUP);
      #else
        pinMode(dancer_pin, INPUT);
      #endif
      pinMode(stopper_pin, INPUT_PULLUP);
      stopTopMotor();
    } // constructor

  void setStateError(String str) { state_error = str; };
  boolean hasError() { return state_error.length() > 0; }
  inline boolean isDancing() { return is_dancing; }
  inline boolean isHome(ExtendedBasicStepperDriver *stepper) 
    { return at_stopper && stepper->getStepsRemaining() == 0 && is_dancing == 0; }
  inline boolean atStopper() { return at_stopper; }
  inline boolean isSpinning() { return is_spinning; }

  // get the dancer state
  void checkDancer() {
    #ifdef DANCER_IS_PULLUP
      is_dancing = digitalRead(dancer_pin) == LOW;
    #else
      is_dancing = digitalRead(dancer_pin) == HIGH;
    #endif
    at_stopper = digitalRead(stopper_pin) == LOW;
  }

  void print_status(ExtendedBasicStepperDriver *stepper) {
    Serial.print("DANCER: is_dancing: ");
    Serial.print(is_dancing);
    Serial.print(", at_stopper: ");
    Serial.print(at_stopper);
    Serial.print(", is_spinning: ");
    Serial.print(is_spinning);
    Serial.print(", is_home: ");
    Serial.print(isHome(stepper)); 
    Serial.print(", direction: ");
    Serial.print(stepper->getDirection() == 0 ? "UP" : "DOWN" );       
    Serial.print(", steps_remaining: ");
    Serial.println(stepper->getStepsRemaining()); 
    if (hasError()) {
      Serial.print("State_error: ");
      Serial.println(state_error);       
    }
  }

  void startTopMotor() {
    Serial.println("Starting top motor");
    pinMode(spinning_pin, OUTPUT);
    digitalWrite(spinning_pin, HIGH);
    is_spinning = true;
  }

  void stopTopMotor() {
    Serial.println("Stopping top motor");
    pinMode(spinning_pin, OUTPUT);
    digitalWrite(spinning_pin, LOW);
    is_spinning = false;
  }
  
};

Dancer dancer(TOP_MOTOR_PIN, DANCER_PIN, STOPPER_PIN);

void setup() {
    Serial.begin(115200);

    Serial.println("START SETUP");
    
    // Configure stopper pin to read HIGH unless grounded
    stepper.begin(RPM, MICROSTEPS);
    
    // display up direction
    stepper.startMove(STEPS_UP);
    Serial.print("up direction: ");
    Serial.println(stepper.getDirection());
    stepper.stop();
    
    // make sure we are moving down not up when we first start, to reset to home
    stepper.startMove(STEPS_DOWN);
    // display the down direction 
    Serial.print("down direction: ");
    Serial.println(stepper.getDirection());
    
    // turn off the top motor in case it was on at start
    dancer.stopTopMotor();
    #ifdef DANCER_IS_PULLUP
      Serial.println("Dancer is INPUT_PULLUP pin");
    #else
      Serial.println("Dancer is normal INPUT pin");
    #endif

    Serial.println("END SETUP");
}


void loop() {
    
    dancer.checkDancer();  // check our environment
    if ((loop_count % PRINT_EVERY) == 0) { dancer.print_status(&stepper); }
    if (dancer.hasError()) {
      if (stepper.getStepsRemaining() > 0 ) {stepper.stop();}
      if (dancer.isSpinning()) { dancer.stopTopMotor();}
      loop_count++;
      return;
    }    
    // first, check if stopper was hit
    if (dancer.atStopper()) {
        // print every so often when stopped
        if ((loop_count % PRINT_EVERY) == 0) { Serial.println("STOPPER REACHED"); }
        // home switch triggered.  better stop!
        if (stepper.getDirection() == DOWN_DIR && stepper.getStepsRemaining() != 0) {
          stepper.stop(); // this sets steps to go to 0 so we don't redo this over and over
          Serial.println("STOPPING MOTOR!");
          Serial.print("AFTER STOP: steps to go: ");
          Serial.print(stepper.getStepsRemaining());
          Serial.print(", isHome()?: ");
          Serial.println(dancer.isHome(&stepper));
          dancer.print_status(&stepper);
          // stepper.disable();  // may drop the piece if there is a small distance to go down...
        }

        // are we all done?, stop spinning top if still spinning
        if (dancer.isHome(&stepper) && dancer.isSpinning()) {
          dancer.stopTopMotor();
        }

        // if we have started up and are 20% and stopper is still set, then we have a problem
        if (stepper.getDirection() == UP_DIR && stepper.getStepCount() > (STEPS_UP * 0.5)) {
          Serial.println("Error Detected: stopping motor, need reset");
          dancer.setStateError("Seems like stopper is stuck, waiting for reset");
          stepper.stop();
          dancer.stopTopMotor();
          return;
        }

    }

    // motor control loop - send pulse and return how long to wait until next pulse
    unsigned wait_time_micros = stepper.nextAction();

    // 0 wait time indicates the motor has stopped
    if (wait_time_micros <= 0) {
      if ((loop_count % PRINT_EVERY) == 0) {Serial.println("Motor Stopped, what next?...");}
      // we are still dancing, need to reverse...
      if (dancer.isDancing()) {
        Serial.println("isDancing, lets go...");
        if (dancer.atStopper() || stepper.getDirection() == DOWN_DIR) {
          Serial.println("Bottom, so go up!");
          stepper.startMove(STEPS_UP);
          dancer.startTopMotor(); 
          Serial.print("UP STARTING STATS: ");
          dancer.print_status(&stepper);
        } else { // not at bottom, stopped and still dancing, down!
          Serial.println("Top, lets to down!");
          stepper.startMove(STEPS_DOWN);
          dancer.startTopMotor();
          Serial.print("DOWN STARTING STATS: ");
          dancer.print_status(&stepper);       
          }
      } else {
        if ((loop_count % PRINT_EVERY) == 0) {Serial.println("Not Dancing....");}
        if (!dancer.isHome(&stepper)) {
          stepper.startMove(STEPS_DOWN);
          if (!dancer.isSpinning()) {
            dancer.startTopMotor();
          }
        }
      }
    }

    // (optional) execute other code if we have enough time
    if (wait_time_micros > 100){  
      if (!dancer.isDancing()) {  // we are moving, but no longer dancing
        if (stepper.getDirection() == UP_DIR) { // go home we are done
          if ((loop_count % PRINT_EVERY) == 0) {Serial.println("Going Home, dance is done!");}
          stepper.stop();
          stepper.startMove(STEPS_DOWN);
        }           
      }
    }
    // keep track of how manytimes we passed through (mostly used to mod printer statements)
    loop_count ++;
}
