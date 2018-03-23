#ifndef XBEE_H
#define XBEE_H
#include <SoftwareSerial.h>
#include </Users/jrule/Documents/Arduino/Dancer/GenericXbee.h>

class Xbee: public GenericXbee {

  public:
    unsigned char xbee_read();
    int xbee_available();
    void xbee_write(unsigned char msg); 
    void status(boolean do_print);

  Xbee(unsigned int read_pin, unsigned int write_pin) : read_pin(read_pin), write_pin(write_pin) {
    pinMode(read_pin, INPUT);
    pinMode(write_pin, OUTPUT);
    xbeeSerial = new SoftwareSerial(read_pin, write_pin); // RX, TX  
    xbeeSerial->begin(9600);

    Serial.print(F("Xbee::CONSTRUCTOR speed=9600, read_pin="));
    Serial.print(read_pin);
    Serial.print(F(", write_pin="));
    Serial.println(write_pin);
  }

  private:
    unsigned int read_pin;      // pin for reading 
    unsigned int write_pin;     // pin for xmitting
    SoftwareSerial *xbeeSerial;
};

// read the data
unsigned char Xbee::xbee_read() {
  return xbeeSerial->read();
}

int Xbee::xbee_available() {
  return xbeeSerial->available();
}

void Xbee::xbee_write(unsigned char msg) {
  #ifdef IS_CHATTY
    Serial.print(F("Xbee::xbee_write msg="));
    Serial.println((char)msg);
  #endif
  xbeeSerial->write(msg);
}

void Xbee::status(boolean do_print) {
  if (! do_print) { return; }

  Serial.print(F("Xbee::status() xbee_type=Xbee, xbee_available="));
  Serial.println(xbee_available());
}

#endif
