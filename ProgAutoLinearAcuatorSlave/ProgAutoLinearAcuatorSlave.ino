/* 
 *  Linear Acuator Driver (Progressive Automations)
 *  
 *  PA-04-HS w/ digital pulse counter
 *  
 *  Using IBT_2 motor driver interface.
 *  
 *  Under Zero load it produces 1865 to fully extend.
 *  
 */

#define SERIAL_SPEED 9600
#define DISPLAY_EVERY 2000
//#define DISPLAY_EVERY 1
#define DO_PRINT 1
unsigned long int display_counter=0;

int RPWM = 4;
int LPWM = 5;

int RENABLE = 6;
int LENABLE = 7;

#define PWM_SPEED 100;

#define BUTTON_ON 0
#define BUTTON_OFF 1

#define ONOFF_PIN  11


int r_pwm;
int button;
int button_cur;

int l_pwm;


#define SIGNAL1_PIN 2

volatile long unsigned int signal1_cnt=0;

#define HOME_WAIT_ITTERATIONS 1000
boolean go_home=true;           // set to true by setup, to get us to return home
unsigned long go_home_ticks=0;    // the number of interrupt ticks the last time through the main loop 
unsigned int itter_at_home=0;   // counts the number of times loop was called when itter was not moving

/* stop after this many interrupts */
#define L_MAX_INTERRUPTS 1000
#define L_MAX_INTERRUPTS 500

char* bool_tostr(bool input_bool) {
  if (input_bool) {
    return("true");
  } else {
    return("false");
  }
}

void display(int do_print=0) {

  if ((do_print == 0) && (display_counter % DISPLAY_EVERY != 0)) return;
  
  Serial.print(F("button_cur="));
  Serial.print(bool_tostr(button_cur==BUTTON_ON));
  Serial.print(F(", button="));
  Serial.print(bool_tostr(button==BUTTON_ON));
  Serial.print(F(", l_pwm="));
  Serial.print(l_pwm);
  Serial.print(F(", r_pwm="));
  Serial.print(r_pwm);

  Serial.print(F(", signal1_cnt="));
  Serial.print(signal1_cnt);

  Serial.print(F(", go_home="));
  Serial.println(bool_tostr(go_home == 1));


}

void signal1() {
  signal1_cnt++;
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(SERIAL_SPEED);
  Serial.println(F("\nStart setup"));
  Serial.println(F("ProgAutoLinearAcuator.ino"));

  Serial.println(F("Setting PWM pins low"));
  for (int i = 4; i <= 5; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
    delay(75);
  }
  r_pwm=0;
  l_pwm=0;

  Serial.println(F("Setting BUTTON pins INPUT_PULLUP"));
  for (int i = 11; i <= 12; i++) {
    pinMode(i, INPUT_PULLUP);
    delay(75);
  }
  button_cur = BUTTON_OFF;
  button = BUTTON_OFF;

  Serial.println(F("Setting ENABLE HIGH"));
  for (int i = 6; i <= 7; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
    delay(75);
  }

  attachInterrupt(digitalPinToInterrupt(SIGNAL1_PIN), signal1, RISING);
  go_home = true;
  go_home_ticks=0;

  display(DO_PRINT);

  Serial.println(F("Setup Complete"));

}



void update_pwm(int cur) {
  if (cur != button) {
    Serial.println(F("Button Switch"));
    display(DO_PRINT);
    if (cur == BUTTON_ON ) {
      l_pwm = 255;
      r_pwm = 0;
    } else {
      l_pwm = 0;
      r_pwm = 255;
    }
    button = cur;
    signal1_cnt=0;
    display(DO_PRINT);
  }
}

void loop() {

  display_counter++;

  button_cur = digitalRead(ONOFF_PIN);

  // first power, go back to home position 
  if (go_home == true) {
    if (r_pwm == 0) Serial.println(F("Going Home..."));
    button_cur = BUTTON_OFF;    // force a return home (right)
    button = BUTTON_OFF;
    r_pwm = 255;                // override the speed to get there
    if (go_home_ticks != signal1_cnt) {
      go_home_ticks=signal1_cnt;
      itter_at_home=0;
    } else {
      itter_at_home++;
      if (itter_at_home >= HOME_WAIT_ITTERATIONS) {
        Serial.println(F("Reached Home.."));
        go_home=false;
      }
    }
  } else {
    update_pwm(button_cur);
  }

  if (signal1_cnt >= L_MAX_INTERRUPTS && l_pwm > 0) {
    Serial.println(F("Extension LImit Reached..."));
    l_pwm = 0; 
    analogWrite(LPWM, l_pwm);
    display(DO_PRINT);

  }

  display();

  analogWrite(RPWM, r_pwm);
  analogWrite(LPWM, l_pwm);

}
