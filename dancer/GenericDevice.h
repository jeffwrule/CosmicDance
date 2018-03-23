#ifndef GENERICDEVICE_H
#define GENERICDEVICE_H

class GenericDevice {

  public:
    virtual void status(boolean do_print) =0;         // print the status for this device
    virtual void start() =0;          // start the device
    virtual void stop() =0;           // stop the device
    virtual void dance() = 0;         // continiously modify a running devices state (if necessary)
};

#endif
