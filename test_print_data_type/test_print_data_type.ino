#include <SPI.h>
#include <SdFat.h>


// serial print variable type
void types(String a) { Serial.println("it's a String"); }
void types(int a) { Serial.println("it's an int"); }
void types(char *a) { Serial.println("it's a char*"); }
void types(float a) { Serial.println("it's a float"); }
void types(bool a) { Serial.println("it's a bool"); }
void types(long a) { Serial.println("it's a long"); }
void types(unsigned long a) { Serial.println("it's an unsigned long"); }

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  String a;
  types(SPI_FULL_SPEED);



}

void loop() {
  // put your main code here, to run repeatedly:


}
