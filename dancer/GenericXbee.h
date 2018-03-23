#ifndef GENERICXBEE_H
#define GENERICXBEE_H

class GenericXbee {
  public:
    virtual unsigned char xbee_read() =0;
    virtual int xbee_available() =0;
    virtual void xbee_write(unsigned char msg) =0;
    // virtual String class_type_name() =0;
    virtual void status(boolean do_print) =0;
};

#endif
