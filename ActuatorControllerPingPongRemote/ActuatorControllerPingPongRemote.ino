
/* 
Controller takes two digital inputs from the Serial port - a speed value [0 - 100] and a "go" value - 'g' = 103.  
For debug purposes, the escape characters 101 and 102 are used to send non-drive commands.  

This controller assumes that the optimal PWM setting = K * speed.  where K is a constant.  
K is an EE-backed value that is adjusted continuously in response to Hall effect feedback from the actuator.  

Need an interlock process and an echo process.  


*/

#include "EEPROMAnything.h"
#include "commands.h"
#include "defines.h"
#include <avr/wdt.h>
#include <util/atomic.h>

#define VERSION 1.3
#define DIRECTION_PIN 8
#define PWM_PIN 9
#define ENCODER_PIN 3
#define DANCER_PIN A3
#define	PWM_PERIOD_HZ 10000	// Hz (1/seconds)
#define PWM_PERIOD_TICS (16000000 / PWM_PERIOD_HZ)	// timer tics per PWM Period
#define FB_PERIOD	100		// milliseconds
#define MAX_PWM		100		// dimensionless
#define MAX_SPEED	100		// encoder tics per second. 
#define WDT_SETTING	WDTO_2S

#define TIMEOUT 100

//Serial_ Serial = Serial;

const char debug = 0;
unsigned long func_time = 0;
unsigned long reboots;
int reboots_addy;
uint8_t wdt_on;
volatile unsigned long last_edge_time = 0;


float k_forward, k_reverse, p_gain, i_gain, d_gain;
float* active_k;

uint8_t nominal_speed, nominal_pwm;		// these are unsigned because we don't count direction of the encoder. 
uint8_t running, fb_enable, pos_enable;
volatile int current_position;
int  d_err, i_err, err;
unsigned long move_start_time, move_duration, last_fb_time, code_execute_time;


boolean moving_forward;
boolean is_position_move;
long global_position;   // current position
long start_from_position;    // where we started from
long move_to_position;  // target position for a new move

boolean is_dancing;
boolean is_home;
boolean is_reset;

unsigned long last_print_ms=0;

void setup()
{

  global_position=0;
  start_from_position=0;
  move_to_position=0;
  moving_forward=false;
  is_dancing=false;
  is_home=false;
  is_reset=false;
  
	int ee_addy;
	//wdt_enable(WDTO_4S);

	Serial.begin(115200);
	// Serial1.begin(9600);  // Turned off so it does not generate garbage

	Serial.print(F("Version "));
	Serial.println(VERSION);

	//Serial1.begin(9600);		// Serial1 are the 

	pinMode(DIRECTION_PIN, OUTPUT);
	pinMode(PWM_PIN, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(13, OUTPUT);
  pinMode(A3, INPUT_PULLUP);

	digitalWrite(7, 0);	// use pin 7 as gnd for LED test. 

	 // Timer1
	TCCR1A = _BV(WGM11);	// non-inverted, fast PWM with ICR1 as top
	TCCR1B = _BV(CS10) | _BV(WGM13) | _BV(WGM12);	
	ICR1 = PWM_PERIOD_TICS;	// set the period of PWM
	
	
	attachInterrupt( digitalPinToInterrupt(ENCODER_PIN), EncoderRisingISR, RISING);

	LoadParams();
	EEPROM_writeAnything(reboots_addy, ++reboots);

	if (wdt_on)
	{
		wdt_reset();
		wdt_enable(WDT_SETTING);
	}

}

void SetPWM(uint8_t duty)
{
	if ((duty == 0) || (duty > 100))	// only set the PWM if duty is in range : [1 - 100]. 
	{
		TCCR1A &= ~_BV(COM1A1);	// turn off the PWM. 
		digitalWrite(PWM_PIN, LOW);
	}
	else
	{
		OCR1A = (uint16_t)duty * (PWM_PERIOD_TICS / 100);
		TCCR1A |= _BV(COM1A1);	// turn on the PWM.  clock is I/O clock with no prescaler 1:1. 
	}
}

void ActuatorStop()
{
	running = 0;
	// Turn off the motor voltage.  
	SetPWM(0);

	// echo the command
  Serial.println("");
	Serial.println(CMD_STOP);
	//Serial1.println(CMD_STOP);
  PrintInfo();
}


void ActuatorGo()
{
	//Serial.println (CMD_GO);
	//Serial1.println(CMD_GO);
	running = (nominal_pwm != 0);
	err = i_err = 0;

	// turn on the motor.  
	SetPWM( nominal_pwm );
	current_position = 0;
  start_from_position = global_position;
	move_start_time = millis();
}

// Moves the actuator at a commanded power (PWM duty) no positional feedback control
void PowerMove(long duration, int pwm)
{
	// first we echo the command
	Serial.print(F("\n\nCommand\t\t\t"));
	Serial.println(CMD_PWR_MOV);
  Serial.print(F("duration\t\t"));
	Serial.println(duration);
  Serial.print(F("pwm\t\t\t"));
	Serial.println(pwm);

	fb_enable = 0;
  is_position_move=false;
	move_duration = duration;
	moving_forward = pwm > 0;
  start_from_position = global_position;

	digitalWrite(DIRECTION_PIN, (moving_forward == true));	// set the direction based on the sign of nominal_speed.
	nominal_pwm = abs(pwm);

  PrintInfo();

}

// Moves the actuator at a commanded speed with positional feedback control
void SpeedMove(long duration, int speed)
{
	nominal_speed = min(abs(speed), 100);
  Serial.print(F("\n\ncommand\t\t\t"));
	Serial.println(CMD_SPD_MOV);
  Serial.print(F("duration\t\t"));
	Serial.println(duration);
  Serial.print(F("speed\t\t\t"));   
	Serial.println(speed);

	fb_enable = 1;
  is_position_move=false;
	move_duration = duration;
  moving_forward = speed > 0;
  start_from_position = global_position;

	digitalWrite(DIRECTION_PIN, (moving_forward == true));	// set the direction based on the sign of nominal_speed.
	nominal_pwm = ((speed > 0) ? (nominal_speed * k_forward) : (nominal_speed * k_reverse));

  PrintInfo();
}

// move to a specifc encoder ticks position
void PositionMove(long _move_to_position, int speed) 
{
  move_to_position = _move_to_position;
  nominal_speed = min(abs(speed), 100);
  Serial.print(F("\n\nCommand\t\t\t"));
  Serial.println(CMD_POS_MOV);
  Serial.print(F("move_to_position\t"));
  Serial.println(move_to_position);
  Serial.print(F("speed\t\t\t"));
  Serial.println(speed);

  fb_enable = 0;
  is_position_move=true;
  start_from_position = global_position;
  moving_forward = move_to_position > global_position;

  digitalWrite(DIRECTION_PIN, (moving_forward == true));
  nominal_pwm = (((move_to_position > global_position)) ? (nominal_speed * k_forward) : (nominal_speed * k_reverse)); 

  PrintInfo();
}

void AutoSetK()
{
	unsigned long timer_start;
	static uint8_t state = 0;	// 0 = started, 1 = waiting 

	wdt_disable();		// disable the wdt so we can use the following blocking code. 

	// Preliminary Retract so we know we have room to extend (not part of calculations) 
	PowerMove(6000, -100);
	ActuatorGo();
	delay(6001);


	// Now we extednd at 75% duty and do calculations on how far we went. 
	PowerMove(6000, 75);	
	ActuatorGo();
	delay(6001);

	// At this point we'll have a valid value in the current_position variable. 
	SetParameter(&k_forward, ((float)(6 * 75) / current_position), F("k_forward"));
	
	// Now we do the same thing but retracting instead of extending, and for a shorter time so we don't bottom out. 
	PowerMove(5000, -75);
	ActuatorGo();
	delay(5001);

	// At this point we'll have a valid value in the current_position variable. 
	SetParameter(&k_reverse, ((float)(5 * 75) / current_position), F("k_reverse"));

	// And save to EEPROM.
	SaveParams();

	wdt_reset();
	if (wdt_on) wdt_enable(WDTO_2S);
}

void Run() // 
{
	static int last_position; 
  long _last_edge_time;
  long cur_mills = millis();
  
	// To start this sequence, call StartSequence(). - this function sets running = 1.  
	if (!running)	return;

  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    _last_edge_time = last_edge_time;
    global_position = moving_forward ? start_from_position + current_position : start_from_position - current_position;
  }
  global_position = global_position < 0 ? 0 : global_position;

  if (cur_mills - move_start_time > 100 && cur_mills - _last_edge_time > 100) {
    if (nominal_pwm < 40) {
      nominal_pwm = nominal_pwm + 1;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        last_edge_time = cur_mills;
      }
      SetPWM(nominal_pwm);
      Serial.print('+');
    } else {
      Serial.print("Reached maximum "); 
      Serial.println(moving_forward ? "extension" : "retraction");
      ActuatorStop();
      if (!moving_forward) global_position = 0;
    }
  }

  // tracks if we have made it to our target position
  if (is_position_move) {
    
    if (move_to_position == 0) {
      return; // just wait for the last_edge_timer to exceed it limit to know we are entirely home
    }
    
    if (moving_forward) {
      if (global_position >= move_to_position) {
        ActuatorStop();    
        return;
      }
    }
    
    if (!moving_forward) {
      if (global_position <= move_to_position) {
        ActuatorStop();
      }
    }
    
  } else { // we are moving based on seconds and speed alone
  	// Check to see if it's time to stop moving.   
  	if ((millis() - move_start_time) >= move_duration)
  	{
  		ActuatorStop();
  		return;
  	}
  }
  
	// Check to see if it's time to adjust the PWM according to feedback.
	if (  (fb_enable)  && ((millis() - last_fb_time) >= FB_PERIOD)  )	{

		if (debug == 't') code_execute_time = micros();

		last_fb_time = millis();
		UpdateFeedback();		// this takes up to .5 milliseconds (heavy calculations)

		if (debug == 't') Serial.println(micros() - code_execute_time);
	}

}

void UpdateFeedback()
{
	int new_err, correction;
	// we're already running, direction and enable are already set up.  

	// calculate error ( nominal position (t) - measured) 


	// negative error means we're lagging - so we need to catch up. 
	new_err = current_position - (((millis() - move_start_time) * nominal_speed)/1000); 

	d_err = new_err - err;
	err = new_err; 
	i_err += err; 

	//Serial.println((i_gain * i_err));

	correction = (p_gain * err) + (i_gain * i_err) + ( d_gain * d_err);

	SetPWM(min((nominal_pwm - correction), 100));
}

void PrintVersion()
{
	Serial.print(F("\nFirmware Version "));
	Serial.println(VERSION);
}

void PrintParams()
{
	Serial.print(F("\np_gain\t\t\t"));
	Serial.println(p_gain);
	Serial.print(F("i_gain\t\t\t"));
	Serial.println(i_gain);
	Serial.print(F("d_gain\t\t\t"));
	Serial.println(d_gain);
	Serial.print(F("k_forward\t\t"));
	Serial.println(k_forward);
	Serial.print(F("k_reverse\t\t"));
	Serial.println(k_reverse);

	Serial.print(F("N reboots\t\t"));
	Serial.println(reboots);
}

void PrintInfo() {
  
  Serial.print(F("\nfb_enable\t\t"));
  Serial.println(fb_enable);
  Serial.print(F("is_position_move\t"));
  Serial.println(is_position_move);
  Serial.print(F("move_duration\t\t"));
  Serial.println(move_duration);
  Serial.print(F("nominal_pwm\t\t"));
  Serial.println(nominal_pwm);

  Serial.print(F("global_position\t\t"));
  Serial.println(global_position);
  Serial.print(F("start_from_position\t"));
  Serial.println(start_from_position);
  Serial.print(F("move_to_position\t"));
  Serial.println(move_to_position);
  Serial.print(F("current_position\t"));
  Serial.println(current_position);
  Serial.print(F("is_dancing\t\t"));
  Serial.println(is_dancing);
  Serial.print(F("is_reset\t\t"));
  Serial.println(is_reset);  
  Serial.print(F("is_home\t\t\t"));
  Serial.println(is_home);  
  Serial.print(F("moving_up\t\t"));
  Serial.println(moving_forward);  
}

void PrintBrief()
{
  unsigned long cur_ms;
  
  if (Serial.availableForWrite() < 10) return;

  cur_ms = millis();
  if (last_print_ms > cur_ms) last_print_ms = 0;
  if (cur_ms - last_print_ms < PRINT_EVERY_MS) {
    return;
  }
  last_print_ms = cur_ms;
  
  Serial.print(F("\n\nnominal_pwm\t\t"));
  Serial.println(nominal_pwm);
  Serial.print(F("global_position\t\t"));
  Serial.println(global_position);
  Serial.print(F("current_position\t"));
  Serial.println(current_position);
  Serial.print(F("is_dancing\t\t"));
  Serial.println(is_dancing);
  Serial.print(F("is_home\t\t\t"));
  Serial.println(is_home);  
  Serial.print(F("moving_up\t\t"));
  Serial.println(moving_forward);   
}

void SaveParams()
{
	int ee_addy = 0;
	ee_addy += EEPROM_writeAnything(ee_addy, wdt_on);
	ee_addy += EEPROM_writeAnything(ee_addy, p_gain);
	ee_addy += EEPROM_writeAnything(ee_addy, i_gain);
	ee_addy += EEPROM_writeAnything(ee_addy, d_gain);
	ee_addy += EEPROM_writeAnything(ee_addy, k_forward);
	ee_addy += EEPROM_writeAnything(ee_addy, k_reverse);

	Serial.println(F("\nParameters saved."));
}
int LoadParams()
{
	int ee_addy = 0;
	ee_addy += EEPROM_readAnything(ee_addy, wdt_on);
	ee_addy += EEPROM_readAnything(ee_addy, p_gain);
	ee_addy += EEPROM_readAnything(ee_addy, i_gain);
	ee_addy += EEPROM_readAnything(ee_addy, d_gain);
	ee_addy += EEPROM_readAnything(ee_addy, k_forward);
	ee_addy += EEPROM_readAnything(ee_addy, k_reverse);
	reboots_addy = ee_addy; 
	ee_addy += EEPROM_readAnything(ee_addy, reboots);

	if (isnan(p_gain) || isnan(i_gain) || isnan(d_gain) || isnan(k_forward) || isnan(k_reverse) || 
		(p_gain < 0.0) || (p_gain > 5.0) || 
		(i_gain < 0.0) || (i_gain > 0.5) || 
		(d_gain < 0.0) || (d_gain > 5.0) || 
		(k_forward < 0.0) || (k_forward > 2.0) || 
		(k_reverse < 0.0) || (k_reverse > 2.0))
	{
		p_gain = 2.0;
		i_gain = 0.2;
		d_gain = 2.0;
		k_forward = 0.72;
		k_reverse = 0.80;

		Serial.println(F("\nLoading Default Parameters"));
		SaveParams();
	}
	else	Serial.println(F("\nParameters Loaded."));

	return ee_addy;
}
void SetParameter(float* var, float new_value, const __FlashStringHelper* name)
{
	
	Serial.print(name);
	Serial.print(F(" set to "));
	Serial.println(new_value);
	*var = new_value;

}

void loop()
{
	ParseUSBSerial();
  Dance();

	if (running) Run();

  PrintBrief();
    
	if (wdt_on) wdt_reset();	// resetting the WDT prevents the USB bootloader from working, so we put it behind 
								// a conditional that we control with the serial port. 

}

// control the acuator based on the dancing pin (active low)
// when first started send acuator to position 0
// after that bounce between POSITION_START and POSITION_STOP when is_dancing=true
// if not dancing, the goto POSIION_HOME
void Dance() {

  int _is_dancing1;
  int _is_dancing2;
  int _is_dancing3;

  // never been home
  if (!is_home) {
    Serial.println(F("Sending acuator to POSITION_HOME"));
    PositionMove(POSITION_HOME, 40);
    ActuatorGo();
    is_home=true;
  }

  // is the trip home started but not complete?
  if (is_home && !is_reset) {
    if (running > 0) {
      return;
    }
    Serial.println(F("Scuplture HOME reset"));
    is_reset = true;
  }

  _is_dancing1 = digitalRead(DANCER_PIN) == 1;
  delay(2);
  _is_dancing2 = digitalRead(DANCER_PIN) == 1;
  delay(2);
  _is_dancing3 = digitalRead(DANCER_PIN) == 1;

  // make sure we are getting consistent and steady reads
  if (_is_dancing1 != _is_dancing2 && _is_dancing2 != _is_dancing3 ) return;

  if (_is_dancing1 != is_dancing) {
    if (_is_dancing1) {
      Serial.println(F("\n\n=====================\nNew Dance Starting\n"));
      PositionMove(POSITION_BOTTOM, PWM_SPEED);
      ActuatorGo();
    } else {
      Serial.println(F("\n\n~~~~~~~~~~~~~~~~~~~~\nDance Ending\n"));
      PositionMove(POSITION_CENTER, PWM_SPEED);
      ActuatorGo();
    }
    is_dancing = _is_dancing1;
    delay(20);
  }
    
  if (!is_dancing && !running && global_position != POSITION_CENTER) {
    Serial.println(F("\nNot dancing going to center"));
    PositionMove(POSITION_CENTER, PWM_SPEED);
    ActuatorGo();
  }

  if (is_dancing) {
    if (!running) {
      if (global_position == POSITION_BOTTOM) {
        Serial.println(F("\nSending Actuator to Top"));
        PositionMove(POSITION_TOP, PWM_SPEED);
      } else {
        Serial.println(F("\nSending Actuator to Bottom"));
        PositionMove(POSITION_BOTTOM, PWM_SPEED);
      }
      ActuatorGo();
    }
  }  
}

void ParseUSBSerial() {
	char c;
	int var1, var2;

	if (!Serial.available()) return;

	c = Serial.read();
	switch (c) {

	case CMD_AUTO_K:
		AutoSetK();
		break;

	case CMD_GO:
		ActuatorGo();
		Serial1.print(CMD_GO);
		break;

	case CMD_STOP:
		ActuatorStop();
		Serial1.print(CMD_STOP);	// echo the stop command
		break;

	case CMD_SET_P:
		SetParameter(&p_gain, Serial.parseFloat(), F("p_gain"));
		break;

	case CMD_SET_I:
		SetParameter(&i_gain, Serial.parseFloat(), F("i_gain"));
		break;

	case CMD_SET_D:
		SetParameter(&d_gain, Serial.parseFloat(), F("d_gain"));
		break;

	case CMD_SET_K_FORWARD:
		SetParameter(&k_forward, Serial.parseFloat(), F("k_forward"));
		break;

	case CMD_SET_K_REVERSE:
		SetParameter(&k_reverse, Serial.parseFloat(), F("k_reverse"));
		break;

	case CMD_SAVE:
		SaveParams();
		break;

  case CMD_PRINT:
    PrintParams();
    break;

  case CMD_INFO:
    PrintInfo();
    break;

	case CMD_LOAD:
		LoadParams();
		break;

	case CMD_PWR_MOV:	// m for move ( little m sets pwm directly )
		var1 = Serial.parseInt();
		var2 = Serial.parseInt();
		PowerMove( var1 , var2 );
		break;

	case CMD_SPD_MOV:	// M for move ( big M sets the speed for feedback control )
		var1 = Serial.parseInt();
		var2 = Serial.parseInt();
		SpeedMove(var1, var2);
		break;

  case CMD_POS_MOV: // t for move to 
    var1 = Serial.parseInt();
    var2 = Serial.parseInt();
    PositionMove(var1, var2);
    break;

	case CMD_VERSION:
		PrintVersion();
		break;

	case CMD_WDT_ON:
		WDTOn();
		break;

	case CMD_WDT_OFF:
		WDTOff();
		break;

	case '\n':	// ignore newline characters
		break;

	default:
		Serial.println(F("invalid command"));
		break;

	}

}

void WDTOn()
{
	Serial1.print(CMD_WDT_ON);
	wdt_on = 1;
	wdt_reset();
	wdt_enable(WDT_SETTING);
	EEPROM_writeAnything(0, wdt_on);
	Serial.println(F("\n Watchdog enabled"));
}

void WDTOff()
{
	Serial1.print(CMD_WDT_OFF);
	wdt_on = 0;
	wdt_disable();
	EEPROM_writeAnything(0, wdt_on);
	Serial.println(F("\n Watchdog disabled"));

}

void EncoderRisingISR()
{
	// static unsigned long last_edge_time = 0;

  long this_edge_time=millis();
	if ((this_edge_time - last_edge_time) > 3)	// only count it if it has been at least 3ms since the last edge. 
	{
		last_edge_time = millis();
		current_position++;
	}

}
