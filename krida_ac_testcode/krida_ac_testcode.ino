
unsigned char AC_LOAD = 7;    // Output to Opto Triac pin
unsigned int zero_cross_pin = 2;  //  zero cross interrupt pin here
volatile unsigned char dimming = 0;    // Dimming level (0-100)
unsigned char i;


void setup() {
  // put your setup code here, to run once:
  pinMode(AC_LOAD, OUTPUT); // Set AC Load pin as output
  // attachInterrupt(1, zero_crosss_int, RISING);
  attachInterrupt(digitalPinToInterrupt(zero_cross_pin), zero_crosss_int, RISING);

  Serial.begin(250000);

}

void zero_crosss_int()  // function to be fired at the zero crossing to dim the light
{
  // Firing angle calculation : 1 full 50Hz wave =1/50=20ms 
  // Every zerocrossing : (50Hz)-> 10ms (1/2 Cycle) For 60Hz (1/2 Cycle) => 8.33ms 
  // 10ms=10000us
  
  int dimtime = (65*dimming);    // For 60Hz =>65  , 50Hz => 100 
  delayMicroseconds(dimtime);    // Off cycle
  digitalWrite(AC_LOAD, HIGH);   // triac firing
  delayMicroseconds(8);         // triac On propogation delay (for 60Hz use 8.33) (for 50Hz us 10)
  digitalWrite(AC_LOAD, LOW);    // triac Off
}



void loop() {

          Serial.println("Dimming up");
          for (i=0;i<=100;i++)
          {
            dimming=i;
            // -- delay(200);
            Serial.println(i);
          }
          Serial.println("Dimming up complete");

          delay(1000);
          
          Serial.println("Dimming down");
          for (i=100; i>=0 && i<=100; i--)
          {
            dimming=i;
            // -- delay(200);
            Serial.println(i);
          }              
          Serial.println("Dimming down complete");

          delay(1000);

}
