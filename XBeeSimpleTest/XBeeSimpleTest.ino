//Arduino UNO XBee Test Code

int led = 13;
int temp = 0;

void setup() {
  Serial.begin(9600); 
  pinMode(led, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    temp = Serial.read();
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
  }
}
