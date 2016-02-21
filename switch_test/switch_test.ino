
int limit_switch = 0;
int line_sensor = 0;

void setup() {
  // put your setup code here, to run once:
pinMode(7, INPUT);
Serial.begin(9600); 

}

void loop() {
  // put your main code here, to run repeatedly:

  limit_switch = digitalRead(7);   // read the input pin
  digitalWrite(13, limit_switch-1);
  
  line_sensor = analogRead(0);
  Serial.print("Line Sensor Value: ");
  Serial.println(line_sensor);
  
}
