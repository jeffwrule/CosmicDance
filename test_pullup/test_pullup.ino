void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);          // setup the interal serial port for debug messages
  pinMode(A0, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:

  delay(30);
  int input_value = digitalRead(A0);

  Serial.print("A0=");
  Serial.println(input_value);

}
