void setup() {
  // put your setup code here, to run once:
  Serial.begin(19200);          // setup the interal serial port for debug messages

}

long i=1;
void loop() {
  // put your main code here, to run repeatedly:

  if ((i % 1000) == 0) {
  Serial.print("Num Loooooops: ");
  Serial.println(i);
  }
  i=i+1;
}
