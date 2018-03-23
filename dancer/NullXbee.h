#ifndef NULLXBEE_H
#define NULLXBEE_H

class NullXbee: public GenericXbee {

  public:
    unsigned char xbee_read();
    int xbee_available();
    void xbee_write(unsigned char msg); 
    void status(boolean do_print);

  NullXbee(unsigned char empty_request) : empty_request(empty_request) {
    Serial.print(F("NullXbee::CONSTRUCTOR no xbee card configured, empty_request="));
    Serial.println(empty_request, HEX);
  }

  private:
    unsigned char empty_request; 
};

// read the data
unsigned char Xbee::xbee_read() {
  return empty_request;
}

int Xbee::xbee_available() {
    return 0;
}

void Xbee::xbee_write(unsigned char msg) {
}

void Xbee::status(boolean do_print) {
  if (! do_print) { return; }

  Serial.print(F("Xbee::status() xbee_type=NullXbee, xbee_available="));
  Serial.println(xbee_availabe());
}

#endif
