#include <arduino.h>
#include "config.h"

#include "PrintTimer.h"
#include "Fob.h"
#include "Xbee.h"
#include "SparkFunMP3.h"
#include "DigitalOnOffDevice.h"
#include "Director.h"
#include "Dancer.h"

PrintTimer *print_timer;  // time to keep things moving
Fob *my_fob;              // pointer to our fob device;
GenericXbee *my_xbee;     // pointer to our xbee interface (null interface is a no-op)
GenericMP3  *my_mp3;      // pointer to my MP3 device

#define NUM_DEVICES 2
GenericDevice *device_list[NUM_DEVICES];
DigitalOnOffDevice *device1;
DigitalOnOffDevice *device2;

Director *my_director;
Dancer *my_dancer;

void setup() {

  Serial.begin(SERIAL_SPEED);          // setup the interal serial port for debug messages
  Serial.println(F("---------- Start setup ----------"));
  Serial.println(F("Sketch=Dancer.ino"));

  print_timer = new PrintTimer(PRINT_AFTER_SECONDS);

  // fob is getting an invalid reading sometimes, lets add a delay before we start to 
  // help stop the erronous readings sometimes..
  delay(500);
  my_fob = new Fob(FOB_PIN);                          // setup the fob
  
  my_xbee = new Xbee(XBEE_READ_PIN, XBEE_WRITE_PIN);  // seetup our xbee card
  
  my_mp3 = new SparkFunMP3(    
    NORMAL_VOLUME, // normal_volume standard play volume
    BEEP_VOLUME, // beep_volume     volume to play the bee at
    SD_SEL, // sd_sel          sd_select pin (use the Arduino Constant SD_SEL)
    SPI_FULL_SPEED // spi_speed       speed to run spi interface at typeical (SPI_FULL_SPEED)
    );

  device1 = new DigitalOnOffDevice(PRIMARY_DEVICE_PIN, "Primary Device");
  device_list[0] = device1;
  device2 = new DigitalOnOffDevice(SECONDARY_DEVICE_PIN, "Secondary Device");
  device_list[1] = device2;

  my_director = new Director(my_xbee, my_fob);

  my_dancer = new Dancer(
    print_timer, //  PrintTimer *pt                    time used by this dancer
    my_fob,             //  Fob *fob                          the fob used by this Dancer
    my_director,        //  Director *director                the associated director structure
    my_mp3,             //  GenericMP3 *mp3                   mp3 device (used to play musice)
    device_list,        //  GenericDevice *d_list             list of connected devices (pins)
    NUM_DEVICES,        //  unsigned int num_devices          number of devices in the list
    SOLO_TRACK_NAME,    //  const char *solo_track_name       Name of the solo track on the microSD card
    ENSEMBL_TRACK_NAME, //  const char *ensembl_track_name    Name of the ensemble track on the microSD card
    PRE_DELAY_SECNDS,   //  unsigned int dance_delay_seconds  number of seconds to delay between director/fob start and music start
    FOB_DELAY_SECONDS   //  unsigned int fob_delay_seconds    number of seconds to delay between plays when in fob mode
    ); 

  // just let it settle for a bit
  delay(3000);  // delay a bit to let things settle down
  
  // signal that we are done
  my_mp3->do_beep(200, 50);
  my_mp3->do_beep(200, 50);
  my_mp3->do_beep(200, 50);

  Serial.print(F("STOP_BEFORE_FOB_DELAY="));
  Serial.println(bool_tostr(STOP_BEFORE_FOB_DELAY));
    
  Serial.println(F("---------- End setup ----------"));
}

// start and stop dancers, keep the show moving along....
void loop() {

    //////////////////////////////////////
    // update pins and status
    //////////////////////////////////////
    print_timer->update();
    my_fob->update();
    my_director->update();
    my_dancer->update();

    //////////////////////////////////////
    // print status values
    //////////////////////////////////////
    boolean do_print = print_timer->get_do_print();
    if (do_print) {     Serial.println(F("-------- loop(): status block starting --------")); }
    print_timer->status();
    my_fob->status(do_print);
    my_xbee->status(do_print);
    my_mp3->status(do_print);
    for (int i=0; i<NUM_DEVICES; i++) {
      device_list[i]->status(do_print);
    }
    my_director->status(do_print);
    my_dancer->status(do_print);


    //////////////////////////////////////
    // do our dance moves
    //////////////////////////////////////
    my_dancer->dance();
    for (int i=0; i<NUM_DEVICES; i++) {
      device_list[i]->dance();
    }
  
}
  
