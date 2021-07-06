// Load Wi-Fi library


int GPIO_pin = 13; // where our interrupt is going to be coming from.
volatile int hits=0;
volatile int last_analog_read=0;
    
    
void setup() {                
  Serial.begin(115200);  //turn on serial communication
  //enable interrupt 0 which uses pin 2
  //jump to the increment function on falling edge
  // digitalWrite(GPIO_pin, HIGH);
  pinMode(GPIO_pin, INPUT);
  Serial.println("Setup Complete");
}





    
void loop() {
    pinMode(GPIO_pin, INPUT);
    attachInterrupt(digitalPinToInterrupt(GPIO_pin), increment, FALLING);

//  delay(10);
//  Serial.print(", d_now=");
//  Serial.print(digitalRead(GPIO_pin));
//  Serial.print(", a_now=");
//  Serial.print(analogRead(GPIO_pin));
  Serial.print("Last analog read=");
  Serial.print(last_analog_read);
  Serial.print(", hits=");
  Serial.println(hits);


}

void increment() {
    hits += 1;
    last_analog_read=analogRead(GPIO_pin);
}
