/*

  A sketch that performs color mixing with the RGB bar graph of the I2C Display v2
  board.

  http://www.rheingoldheavy.com/i2c-display-add-on-tutorial-02-larson-scanner/

*/

#include <I2C.h>

#define BLK 0x00
#define RED 0x04
#define GRN 0x02
#define BLU 0x01
#define YLW 0x06
#define MAG 0x05
#define CYA 0x03
#define WHT 0x07

const byte IS31FL3728_I2C  = 0x60; // IS31FL3728 I2C Address and Registers. Driver Address Pin is Tied Low.
const byte IS31_R_CONFIG   = 0x00; // Configuration Register
const byte IS31_R_BRIGHT   = 0x0D; // Brightness / Audio Gain Register
const byte IS31_R_UPDATE   = 0x0C; // OpCode used to latch display update
const byte DUMMY_DATA      = 0xCC; // OpCode needs to be sent dummy value

const byte TOP_R           = 0x01; // top RED LED in RGB die
const byte TOP_G           = 0x02; // top GRN LED in RGB die
const byte TOP_B           = 0x03; // top BLU LED in RGB die

byte valueTopR = 0;
byte valueTopG = 0;
byte valueTopB = 0;

const byte pinShutdown     = A3;

void setup() {

  pinMode      (pinShutdown, OUTPUT);   // Configure the shutdown pin as output
  digitalWrite (pinShutdown, HIGH);     // Enable the display

  I2c.begin    ();             // Initialize the I2C library
  I2c.pullup   (1);            // Enable the internal pullup resistors
  I2c.setSpeed (1);            // Enable 100kHz I2C Bus Speed
  I2c.timeOut  (250);          // Set a 250ms timeout before the bus resets

  I2c.write(IS31FL3728_I2C, IS31_R_CONFIG, 0x00);  // Configure Driver for Normal Operation, Audio Disabled, 8x8 Matrix Mode
  I2c.write(IS31FL3728_I2C, IS31_R_BRIGHT, 0x08);  // Configure brightness to lowest value
  
  // Write zeros to all the bits of each display register: 0x01 through 0x08
  for (int i = 1; i < 9; i++) {
    I2c.write(IS31FL3728_I2C, i, 0x00);
  }
  I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);

}

void loop() {

  setRGB(0, BLK);
  setRGB(1, RED);
  setRGB(2, GRN);
  setRGB(3, BLU);
  setRGB(4, YLW);
  setRGB(5, MAG);
  setRGB(6, CYA);
  setRGB(7, WHT);
}

void setRGB(uint8_t pixelPosition, uint8_t pixelColor)
{
  // Determine the desired state of each color
  byte rValue = (pixelColor & 0x04) >> 2; 
  byte gValue = (pixelColor & 0x02) >> 1;
  byte bValue = (pixelColor & 0x01);
  
  // Sets or clears bits depending on the state of the color in that position
  if (rValue == 0) valueTopR &= ~(0x01 << pixelPosition);
  if (gValue == 0) valueTopG &= ~(0x01 << pixelPosition);
  if (bValue == 0) valueTopB &= ~(0x01 << pixelPosition);
  if (rValue == 1) valueTopR |=  (rValue << pixelPosition);
  if (gValue == 1) valueTopG |=  (gValue << pixelPosition);
  if (bValue == 1) valueTopB |=  (bValue << pixelPosition);

  I2c.write(IS31FL3728_I2C, TOP_R, valueTopR);
  I2c.write(IS31FL3728_I2C, TOP_G, valueTopG);
  I2c.write(IS31FL3728_I2C, TOP_B, valueTopB);
  I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
}
