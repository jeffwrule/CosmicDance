#ifndef DIGITALONOFFDEVICE_H
#define DIGITALONOFFDEVICE_H

#include </Users/jrule/Documents/Arduino/Dancer/GenericDevice.h>

// a device that is enabled/disable via a pin
class DigitalOnOffDevice: public GenericDevice {

  public:
    void status(boolean do_print);        // print the status for this device
    void start();         // start the device
    void stop();          // stop the device
    void dance();         // continiously modify a running devices state (if necessary)


    DigitalOnOffDevice(unsigned int output_pin, String device_name) : output_pin(output_pin), device_name(device_name) {
        pinMode(output_pin, OUTPUT);
        digitalWrite(output_pin, LOW);
        is_enabled=false;

        Serial.print(F("DigitalOnOffDevice::CONSTRUCTOR device_name="));
        Serial.print(device_name);
        Serial.print(F(", output_pin="));
        Serial.println(output_pin);
    }

  private:
    boolean is_enabled;
    String device_name;
    unsigned int output_pin;
}; 

// print the status for this device
void DigitalOnOffDevice::status(boolean do_print) {
  if (! do_print ) { return; }
  Serial.print(F("DigitalOnOffDevice::status() device_name="));
  Serial.print(device_name);
  Serial.print(F(", is_enabled="));
  Serial.println(bool_tostr(is_enabled));
}

// start the device
void DigitalOnOffDevice::start() {
  if (!is_enabled) {
    Serial.print(F("Starting DigitalOnOffDevice device_name="));
    Serial.println(device_name);
    digitalWrite(output_pin, HIGH);
    is_enabled=true;
  }
}

// stop the device
void DigitalOnOffDevice::stop() {
  if (is_enabled) {
    Serial.print(F("Stopping DigitalOnOffDevice device_name="));
    Serial.println(device_name);
    digitalWrite(output_pin, LOW);
    is_enabled=false;
  }
}

// continiously modify a running devices state (if necessary)
void DigitalOnOffDevice::dance() {
  return; 
}


#endif
