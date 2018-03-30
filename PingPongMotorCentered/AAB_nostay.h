#ifndef AAB_NOSTAY_H
#define AAB_NOSTAY_H

#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/PrintTimer.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/Limits.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/DanceMove.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/Dancer.h>

// include the limit switch and motor type for this build
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/HBridgeMotor.h>
#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/DigitalLimitSwitch.h>

#include </Users/jrule/Documents/Arduino/pingPongMotorCentered/AAB_nostay_config.h>

// control the print speed
PrintTimer *print_timer;

GenericMotor *my_motor1;
Dancer *my_dancer1;
GenericLimitSwitch *m1_top_limit_switch;
GenericLimitSwitch *m1_bottom_limit_switch;
GenericLimitSwitch *m1_center_limit_switch;


// just one motor
#define NUM_DANCERS 1
Dancer* dancers[NUM_DANCERS];

// 2 moves up and down full extended; the repeat
#define D1_NUM_DANCE_MOVES 3
DanceMove *d1_dance_moves[D1_NUM_DANCE_MOVES];

// down = r, up=l
#define NOSTAY_SPEED_DOWN 60    
#define NOSTAY_SPEED_UP 100

void setup() {

  int i;

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("\nStart setup"));
  Serial.println(F("PingPongMotorCentered.ino"));
  Serial.println(F("NOSTAY CONFIG"));

  pinMode(HBRIDGE_MOTOR_STDBY, OUTPUT);
  digitalWrite(HBRIDGE_MOTOR_STDBY, LOW);

  pinMode(DANCER_INPUT_PIN, INPUT);

  print_timer = new PrintTimer(PRINT_AFTER_SECONDS);
  print_timer->init_values();
  print_timer->status();
  
  // NOSTAY (small book shelf)
  my_motor1 = new HBridgeMotor(    
    "bookshelf",     // p_motor_name               name of this motor (for status messages)
    255,                   // m_speed_left               max speed when going left
    255,                   // m_speed_right              max_speed when going right
    M1_HGRIDGE_PWM_PIN,    // p_speed                    pwm speed pin
    HBRIDGE_MOTOR_STDBY,   // p_standby                  stand by pin (high=enabled, low=disabled)
    M1_HGRIDGE_LEFT_PIN,   // p_left                     pin for left direction 
    M1_HGRIDGE_RIGHT_PIN,  // p_right                    pin for right direction
    25,                   // p_motor_start_speed        start() uses this as the first pmw speed when starting, must be *LESS* then m_speed_left, m_speed_right
    10,                    // p_speed_increment          increment the pmw by this ammount when starting the motor
    50,                    // p_speed_up_increment_delay   delay to add between increments to make speedup smoother
    50,                    // p_slow_down_increment_delay  delay to add between increments to make slowdown smoother
    200                    // p_reverse_delay_millis     wait this many milliseconds when reversing direction 
    );

  // NOSTAY (small book shelf)
  m1_top_limit_switch = new DigitalLimitSwitch(
    "top_limit_L", // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_LEFT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'l',  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NC"  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    );
  m1_bottom_limit_switch = new DigitalLimitSwitch(
    "bottom_limit_R", // p_digital_pin            digital pin that limit switch is attached to (can be analog but we read it with digitalRead), will be set to INPUT_PULLUP, tie to ground
    M1_RIGHT_LIMIT_PIN,    // p_limit_name             name for this limit switch
    'r',  // p_limit_position         l,r,c where are we physically located (NOTE a center switch can be l or r to signal that center is same as l)
    "NC"  // p_normally_open_closed   is either String "NC" normally closed or "NO" normally open, most are NC 
    ); 
  m1_center_limit_switch = m1_top_limit_switch; // the top limit is the center/home


  // NOSTAY (small book shelf), down first
  i=0;
  d1_dance_moves[i++] = new DanceMove(
        "down1",
        2,    // move_delay_seconds     seconds to delay before we start moving the motor
        'r',  // move_direction         motor direction l,r (l=down, r=up typically)
        NOSTAY_SPEED_DOWN,  // move_speed             pwm speed to run the motor at, 0-255
        60*5,  // move_duration_seconds  5 minutes, we just want to hit the limit to stop
        DELAY_TYPE_MOVE
        ); 

  // NOSTAY (small book shelf), up second
  d1_dance_moves[i++] = new DanceMove(
      "up2",
      0,    // move_delay_seconds     seconds to delay before we start moving the motor
      'l',  // move_direction         motor direction l,r (l=down, r=up typically)
      NOSTAY_SPEED_UP,  // move_speed             pwm speed to run the motor at, 0-255
      60*5,  // move_duration_seconds  5 minutes, we just want to hit the limit switch
      DELAY_TYPE_MOVE
      ); 
  // NOSTAY (small book shelf), up second
  d1_dance_moves[i++] = new DanceMove(
      "justdelay3",
      60*60,    // move_delay_seconds     seconds to delay before we start moving the motor
      'l',  // move_direction         motor direction l,r (l=down, r=up typically)
      0,  // move_speed             pwm speed to run the motor at, 0-255
      0,  // move_duration_seconds  5 minutes, we just want to hit the limit switch
      DELAY_TYPE_MOVE
      ); 
      
  if (i != D1_NUM_DANCE_MOVES) {
    Serial.print(F("ERROR: D1_NUM_DANCE_MOVES != num of initialized steps, D1_NUM_DANCE_MOVES="));
    Serial.print(D1_NUM_DANCE_MOVES);
    Serial.print(F(", i="));
    Serial.println(i);
  }
    
  // NOSTAY (small book shelf)
  my_dancer1 = new Dancer(
    "smallbookself",            // p_dancer_name  name of this dancer, for status messages
    DANCER_INPUT_PIN,           // p_dancer_pin   input from primary dancer remote_is_dancing, INPUT_PULLUP, can be Analong pin will be read with digitalRead
    my_motor1,                  // p_motor        pointer to our motor structure
    print_timer,                // global timer for the dancer to use
    "nostay_limits",          // p_limits_name  The name of the Limits objects that will be created using the LimitSwitch objects
    m1_top_limit_switch,        // p_left_limit   left limit switch
    m1_bottom_limit_switch,     // p_right_limit  right limit switch
    m1_center_limit_switch,     // p_center_limit center limit switch (functions as home position), can be a pointer to left or right if home is left or right
    d1_dance_moves,             // p_my_dance_moves       list of dance moves to execute
    D1_NUM_DANCE_MOVES,         // p_num_dance_moves      number of dance moves
    0,                          // p_delay_dance_seconds  one time delay at beginning of dance
    0,                          // p_extend_dance_seconds number of seconds to extend the dance when done
    0,                        // p_extend_speed speed to run the PMW at when extending
    NOSTAY_SPEED_UP           // p_center_speed   speed to run motor at when centering
    );

    dancers[0] = my_dancer1;

    print_timer->print_now();  // make sure this call prints a status
     // make sure the calls to status prints 
    // my_dancer1->update();
    // my_dancer1->status();

    Serial.println(F("End Setup"));

}

#endif

