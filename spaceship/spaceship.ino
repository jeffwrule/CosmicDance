void setup() {
 
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(2, INPUT);
  
}

int duration = 250;

void loop () {
  int switch_state = 0;
  
  switch_state = digitalRead(2);
  
  if (switch_state == LOW) {
    duration = 250;
    digitalWrite(3,HIGH);
    digitalWrite(4,LOW);
    digitalWrite(5,LOW);
  } else {
    digitalWrite(3,LOW);
    digitalWrite(4,HIGH);
    digitalWrite(5,LOW);
    delay(duration);
    digitalWrite(4,LOW);
    digitalWrite(5,HIGH);  
    delay(duration);
    duration = duration - 20;
    if (duration < 20) {duration = 20;}
  }
  
}
