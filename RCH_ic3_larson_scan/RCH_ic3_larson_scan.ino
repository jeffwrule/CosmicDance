/*

 A sketch that displays a simple Larson Scanner using the blue bar graph LEDs
 on the I2C Display Add-on v2 board, and displays a running count of how many times
 the scan has cycled through.

 http://www.rheingoldheavy.com/i2c-display-add-on-tutorial-02-larson-scanner/

 */
 
 #include <I2C.h>

const byte IS31FL3728_I2C  = 0x60; // IS31FL3728 I2C Address and Registers. Driver Address Pin is Tied Low.
const byte IS31_R_CONFIG   = 0x00; // Configuration Register
const byte IS31_R_BRIGHT   = 0x0D; // Brightness / Audio Gain Register
const byte IS31_R_UPDATE   = 0x0C; // OpCode used to latch display update
const byte DUMMY_DATA      = 0xCC; // OpCode needs to be sent dummy value

const byte TOP_R           = 0x01; // top RED LED in RGB die
const byte TOP_G           = 0x02; // top GRN LED in RGB die
const byte TOP_B           = 0x03; // top BLU LED in RGB die
const byte DIG_D           = 0x04; // extra marking LEDs in display
const byte DIG_3           = 0x05; // Number 3 display in 0 1 2 3 order
const byte DIG_2           = 0x06; // Number 2 display in 0 1 2 3 order
const byte DIG_1           = 0x07; // Number 1 display in 0 1 2 3 order
const byte DIG_0           = 0x08; // Number 0 display in 0 1 2 3 order

const byte numPatterns[20] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x0E, 0xFE, 0xDE};

const byte pinShutdown     = A3;
int   scanCycles           = 0;    // A variable to hold how many times the scanner cycles

void setup() {

  pinMode      (pinShutdown, OUTPUT);   // Configure the shutdown pin as output
  digitalWrite (pinShutdown, HIGH);     // Enable the bottom row red LED, not L1/L2/L3

  I2c.begin    ();             // Initialize the I2C library
  I2c.pullup   (1);            // Enable the internal pullup resistors
  I2c.setSpeed (1);            // Enable 100kHz I2C Bus Speed
  I2c.timeOut  (250);          // Set a 250ms timeout before the bus resets

  Serial.begin(9600);

  I2c.write(IS31FL3728_I2C, IS31_R_CONFIG, 0x00);  // Configure Driver for Normal Operation, Audio Disabled, 8x8 Matrix Mode
  I2c.write(IS31FL3728_I2C, IS31_R_BRIGHT, 0x08);  // Configure brightness to lowest value

  clearDisplay();

}

void loop() {

  cycleLarsonScanner();
  updateScanCount();

}

void cycleLarsonScanner()
{

  byte scannerSpeed = 15;

  // Send the bit to the left
  for (int i = 0; i < 7; i ++) {
    I2c.write(IS31FL3728_I2C, TOP_B, 0x01 << i);
    I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
    delay(scannerSpeed);
  }

  // Send the bit to the right
  for (int i = 0; i < 7; i ++) {
    I2c.write(IS31FL3728_I2C, TOP_B, 0x80 >> i);
    I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
    delay(scannerSpeed);
  }

  // increment the scanCycles variable, unless it's already at 9999, then reset it
  if (scanCycles == 9999) scanCycles = 0;
  scanCycles++;
}

void updateScanCount()
{

  I2c.write(IS31FL3728_I2C, DIG_0, numPatterns[(scanCycles / 1000)]);
  I2c.write(IS31FL3728_I2C, DIG_1, numPatterns[(scanCycles % 1000 / 100)]);
  I2c.write(IS31FL3728_I2C, DIG_2, numPatterns[(scanCycles % 1000 % 100 / 10)]);
  I2c.write(IS31FL3728_I2C, DIG_3, numPatterns[(scanCycles % 1000 % 100 % 10)]);
  I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);

}

void clearDisplay() {
  for (int i = 1; i < 9; i++) {
    I2c.write(IS31FL3728_I2C, i, 0x00);
  }
  I2c.write(IS31FL3728_I2C, IS31_R_UPDATE, DUMMY_DATA);
}
