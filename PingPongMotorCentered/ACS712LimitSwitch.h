#ifndef ACS712LIMITSWITCH_H
#define ACS712LIMITSWITCH_H

///////// ACS Limit Switch   
/// base on code from here: http://www.microcontroller-project.com/acs712-current-sensor-with-arduino.html
class ACS712LimitSwitch: public GenericLimitSwitch {
  public:
  
    void update()=0;
    void status()=0;
    String get_limit_type();
    boolean get_is_at_limit();
    char get_limit_position();
    String get_switch_name();

    ACS712LimitSwitch(int p_analog_pin, String p_switch_name, char p_limit_position, int p_amperage) :
      analog_pin(p_analog_pin), 
      switch_name(p_switch_name),
      limit_position(p_limit_position),
      amperage(p_amperage)
      {
        if (amperage == 30) {
          acs_factor=acs_30a_factor;
        } else if (amperage == 20) {
          acs_factor=acs_20a_factor;
        } else {
          acs_factor=acs_5a_factor;
        }
        pinMode(analog_pin, INPUT);
        update();
        // print the init and set once values
        Serial.print(F("CONSTRUCTOR::ACS712LimitSwitch switch_name="));
        Serial.print(switch_name);
        Serial.print(F(", p_analog_pin="));
        Serial.print(p_analog_pin);
        Serial.print(F(", p_limit_position: "));
        Serial.print(p_limit_position);
        Serial.print(F(", p_amperage "));
        Serial.print(p_amperage);
        Serial.print(F(", acs_factor "));
        Serial.println(acs_factor, 3);
      }

  private:

    const String limit_type = "acs712";   // limit class
    boolean is_at_limit=false;            // have we reached out limit?
    char limit_position;                  // position where this limit is installed r,l,c
    String  switch_name;                  // give this limit a name (for status etc..)

    const float acs_off = 0.10;           // acs cutoff value values betwen -acs_off to acs_off, power is off
    const  float acs_30a_factor = 0.066;   // 30Amp chip factor
    const   float acs_20a_factor = 0.100;  // 20Amp chip factor
    const  float acs_5a_factor = 0.185;    // 5Amp chip factor
    
    int     analog_pin;           // analog pin hooked up to ACS breakout input
    int     amperage;             // amperage rating for ACS, so we can assign the correct acs_factor;
    float   acs_factor;           // constant factor to put into ACS sensor routine
    float   last_acs_value_f=0.0; // previous acs read value
    float   acs_value_f=0.0;      // current acs formula output
};

String ACS712LimitSwitch::get_limit_type() {return limit_type; }
boolean ACS712LimitSwitch::get_is_at_limit() {return is_at_limit; }
char ACS712LimitSwitch::get_limit_position() {return limit_position; }
String ACS712LimitSwitch::get_switch_name() {return switch_name; };

// read the ACS712 HAL censor to see if we have power (at limit or not)
// map the result into SWITCH_OPEN (when button not pushed) the motor is running, SWITCH_CLOSED the motor is off
void ACS712LimitSwitch::update() {
  unsigned int x=0;

  float acs_value=0.0,
        samples=0.0,
        avg_acs=0.0,
        acs_value_f=0.0;

  // sample this value a bunch of times to get an average
  for (int x=0; x < 150; x++) {
    acs_value = analogRead(analog_pin);
    // Serial.print("individual read=");
    // Serial.println(AcsValue);
    samples = samples + acs_value;
    delay(1);
  }
  avg_acs=samples/150.0;
  #ifdef IS_CHATTY
    Serial.print("samples=");
    Serial.println(samples);
    Serial.print("avg_acs=");
    Serial.println(avg_acs);
    Serial.print("Scale up=");
    Serial.println(avg_acs * (5.0/1024.0));
  #endif
  // this is for a 30 AMP sensor (this comment may not be true any more)
  acs_value_f=((avg_acs * (5.0 / 1024.0)) - 2.5)/acs_factor;
  last_acs_value_f = acs_value_f;
  #ifdef IS_CHATTY
    Serial.println(AcsValueF);
    delay(10);
  #endif
  // if AcsValueF is near 0 then the power is off/limit switch was triggered
  if ( acs_value_f >= (acs_off * -1.0) and acs_value_f <= acs_off ) {
    is_at_limit=true;
  } else {
    is_at_limit=false;
  }
}

// print our status
void ACS712LimitSwitch::status() {
  if (! do_print) {
    return;
  }
  Serial.print(F("ACS712LimitSwitch:: switch_name="));
  Serial.print(switch_name);
  Serial.print(F(", limit_position: "));
  Serial.print(limit_position);
  Serial.print(F(", last_acs_value_f "));
  Serial.print(last_acs_value_f, 5);
  Serial.print(F(", acs_value_f "));
  Serial.print(acs_value_f, 5);
  Serial.print(F(", is_at_limit: "));
  Serial.println(bool_tostr(is_at_limit));
}


#endif
