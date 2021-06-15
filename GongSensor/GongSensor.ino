   /*     
    Simple Interrupt example 2
    by: Jordan McConnell
    SparkFun Electronics
    created on 10/29/11
    */
    
int ledPin = 13; // LED is attached to digital pin 13
int interruptPin = 2; // where our interrupt is going to be coming from.
volatile long hits = 0; // variable to be updated by the interrupt
volatile long false_interrupts = 0; // the number of times interrupt is called to soon

//variables to keep track of the timing of recent interrupts
volatile unsigned long button_time = 0;  
volatile unsigned long last_button_time = 0; 
volatile boolean           led_state=false;
volatile boolean           last_led_state=false;
    
    
void setup() {                
  //enable interrupt 0 which uses pin 2
  //jump to the increment function on falling edge
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), increment, FALLING);
  Serial.begin(115200);  //turn on serial communication
  Serial.println("Setup Complete");
}
    
void loop() {

  digitalWrite(ledPin, led_state);

  if (led_state != last_led_state) {
    last_led_state = led_state;
    digitalWrite(ledPin, led_state);
    Serial.print("New LED State: ");
    Serial.print(led_state);
    Serial.print(", hits: ");
    Serial.print(hits);
    Serial.print(", false_interrupts: ");
    Serial.println(false_interrupts);
  }
  
}
    
// Interrupt service routine for interrupt 0
void increment() {
  button_time = millis();
  //check to see if increment() was called in the last 250 milliseconds
  if (button_time - last_button_time > 250)
  {
    led_state = !led_state;
    hits++;
    last_button_time = button_time;
  } else {
    false_interrupts++;
  }
}
