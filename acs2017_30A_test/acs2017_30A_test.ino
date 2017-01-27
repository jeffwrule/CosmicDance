void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned int x=0;

  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x=0; x < 150; x++) {
    AcsValue = analogRead(A1);
//    Serial.print("individual read=");
//    Serial.println(AcsValue);
    Samples = Samples + AcsValue;
    delay(1);
  }
  AvgAcs=Samples/150.0;
//  Serial.print("Samples=");
//  Serial.println(Samples);
//  Serial.print("AvgAcs=");
//  Serial.println(AvgAcs);
//  Serial.print("Scale up=");
//  Serial.println(AvgAcs * (5.0/1024.0));
  AcsValueF=((AvgAcs * (5.0 / 1024.0)) - 2.5)/0.066;
//
//  Serial.println(AcsValueF);
//  delay(10);
}
