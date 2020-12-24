/*

  A sketch that displays the values from the AT30TS750 temperature sensor
  using the I2C and SPI Interaction Shield Display Board and the I2C Display
  http://www.rheingoldheavy.com

  AT30TS750 Datasheet:  https://media.digikey.com/pdf/Data%20Sheets/Atmel%20PDFs/AT30TS750.pdf
  IS31FL3728 Datasheet: http://www.issi.com/WW/pdf/31FL3728.pdf
*/

#include "I2C.h"   // Include the I2C library

// AT30TS750A Temperature Sensor Constants
const byte   AT30TS750_I2C      = 0x48;  // I2C Address
const byte   AT30TS750_R_TEMP   = 0x00;  // Register Address: Temperature Value
const byte   AT30TS750_R_CONFIG = 0x01;  // Register Address: Temperature sensor configuration

// IS31FL3728 LED Driver Constants
const byte IS31FL3728_I2C   = 0x60; // I2C Address. Driver Address Pin is Tied Low.
const byte TOP_R            = 0x01; // top red LED in RGB die
const byte TOP_G            = 0x02; // top grn LED in RGB die
const byte TOP_B            = 0x03; // top blu LED in RGB die
const byte DIG_D            = 0x04; // extra marking LEDs in display
const byte DIG_3            = 0x05; // Number 3 display in 0 1 2 3 order
const byte DIG_2            = 0x06; // Number 2 display in 0 1 2 3 order
const byte DIG_1            = 0x07; // Number 1 display in 0 1 2 3 order
const byte DIG_0            = 0x08; // Number 0 display in 0 1 2 3 order
const byte IS31_R_CONFIG    = 0x00; // IS31FL3728 Configuration Register
const byte IS31_R_BRIGHT    = 0x0D; // IS31FL3728 Brightness / Audio Gain Register
const byte IS31_R_UPDATE    = 0x0C; // OpCode used to latch display update
const byte DUMMY_DATA       = 0xCC; // OpCode needs to be sent dummy value
const byte DEGREE_SYM       = 0xC6; // Degree Symbol Bit Pattern
const byte numPatterns[20]  = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x0E, 0xFE, 0xDE};

// Speaker and IS31FL3728 Shutdown Pins
int  pinSpeaker  = A2;
int  pinShutdown = A3;

void setup()
{

  Serial.begin      (9600);

  // Initialize Speaker and Shutdown Pin
  pinMode      (pinSpeaker,  OUTPUT);
  pinMode      (pinShutdown, OUTPUT);
  digitalWrite (pinShutdown, HIGH);   // IS31 Shutdown Pin Active Low

  I2c.begin    ();       // Initialize the I2C library
  I2c.pullup   (0);      // Disable the internal pullup resistors
  I2c.setSpeed (1);      // Enable 100kHz I2C Bus Speed
  I2c.timeOut  (25);     // Set a 25ms timeout before the bus resets

  I2c.write(AT30TS750_I2C, AT30TS750_R_CONFIG, 0x00); // 9 bit Resolution
  I2c.write(IS31FL3728_I2C, IS31_R_CONFIG, 0x00);     // Normal Operation, Audio Disabled, 8x8 Matrix Mode

  // Blank the display and create the degree symbol in the DIGIT 3 space
  for (int i = 1; i < 9; i++) {
    I2c.write(IS31FL3728_I2C, i, 0x00);
  }
  I2c.write(IS31FL3728_I2C, DIG_3, DEGREE_SYM);     // Create degree symbol by setting individual segments
  I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);

  Serial.begin(250000);
}

void loop()
{
 
  // byte   tempLSByte = 0;
  byte    tempMSByte = 0;
  int     tempFarenheight=0;
 
  I2c.read(AT30TS750_I2C, AT30TS750_R_TEMP, 2);
 
  tempMSByte = I2c.receive();
  // tempLSByte = I2c.receive() >> 4;

  tempFarenheight = tempMSByte / (5.0/9.0) + 32; 

  setDigit (DIG_0, tempFarenheight / 100);
  setDigit (DIG_1, tempFarenheight / 10 % 10);
  setDigit (DIG_2, tempFarenheight % 10);
  // setDigit (DIG_3, tempFarenheight * 0.625);

 
  if (tempFarenheight >= 83)
  {
    I2c.write(IS31FL3728_I2C, TOP_R, 0xFF);     // Set Red
    I2c.write(IS31FL3728_I2C, TOP_G, 0x00);     // Clear Green
    I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
    // tone(pinSpeaker, 440);
  }
 
  if (tempFarenheight >= 78 && tempFarenheight < 83)
  {
    I2c.write(IS31FL3728_I2C, TOP_R, 0xFF);     // Set Red
    I2c.write(IS31FL3728_I2C, TOP_G, 0xFF);     // Set Green
    I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
    // noTone(pinSpeaker);
  }
 
  if (tempFarenheight <  78)
  {
    I2c.write(IS31FL3728_I2C, TOP_G, 0xFF);     // Set Green
    I2c.write(IS31FL3728_I2C, TOP_R, 0x00);     // Clear Red
    I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
    // noTone(pinSpeaker);
  }

  Serial.print(tempFarenheight);
  Serial.print('\n');
  delay(1000);
 
}


void setDigit(int digitPosition, int digitValue)
{
  byte displayPattern = numPatterns[digitValue];

  // If the digit is the ones place of the temperature value, turn on the decimal point
  // if (digitPosition == DIG_2)
  // {
  //  displayPattern = displayPattern + 1;
  // }

  I2c.write(IS31FL3728_I2C, digitPosition, displayPattern);
  I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);

}
