const byte ledPin = 13 ;

const byte interruptPin = 2 ;

volatile byte state = LOW ;
volatile unsigned long switches = 0;

void setup ( ) {

   pinMode ( ledPin , OUTPUT ) ;

   pinMode ( interruptPin , INPUT_PULLUP) ;

   attachInterrupt ( digitalPinToInterrupt ( interruptPin ), glow, RISING ) ;

   Serial.begin(250000);

} 

void loop () {

  unsigned long static next_write = 0;
  unsigned long cur_millis = millis();

  digitalWrite ( ledPin, state ) ;

  if (cur_millis > next_write) {
    Serial.print(switches); 
    Serial.print(" , ");
    Serial.println(state);
    next_write = cur_millis + 1000;
  }

}

void glow ( ) 

{

   state = !state ;
   switches++;

}
