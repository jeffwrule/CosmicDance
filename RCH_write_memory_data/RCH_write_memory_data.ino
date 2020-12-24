/*

 A sketch to program data to the 8-Mbit AT25SF081 Flash Memory IC.  This sketch will write
 a single byte of memory to the memory address specified in the some24bitLocation variable
 within Setup.

 The code supposes the use of the Education Shield, but if you're using a breakout
 board, connect the CS pin to Digital 10, HOLD to Digital 9, and the SPI pins in
 their usual locations.

 Website:   http://www.rheingoldheavy.com/at25sf081-tutorial-04-programming-memory
 Datasheet: http://www.adestotech.com/wp-content/uploads/DS-AT25SF081_045.pdf

*/


#include <SPI.h>                            // Include the SPI library

SPISettings AT25SF081(8000000, MSBFIRST, SPI_MODE0);

// Sets the pin values for the AT25SF081 and MCP3008
const int  CS_AT25SF081   = 10;             // Flash Memory Chip Select - Active Low
//const int  holdPin        = 9;              // HOLD Pin - Active Low

// Set bits for the various Flash IC OpCodes (see datasheet)
const byte OPCODE_SLOWREAD_ARRAY       = 0x03;   // Memory read up <= 50Mhz (see datasheet)
const byte OPCODE_FASTREAD_ARRAY       = 0x0B;   // Memory read up <= 85Mhz (see datasheet)
const byte OPCODE_WRITEENABLE          = 0x06;   // Enable programming
const byte OPCODE_PROGRAM              = 0x02;   // Write command
const byte OPCODE_READSTATUSREGISTER01 = 0x05;   // Read Status Register 01

const int  programButton               = 7;      // LATCH button used to set resolution

void setup()
{

  Serial.begin   (9600);
  pinMode        (programButton, INPUT);
  AT25SF081_init ();

  Serial.println ("Press the LATCH button to execute the PROGRAM command...");
  Serial.println ();

}

void loop()
{
  int  buttonPressProg   = 1;          // Determins if this is the first button press
  long some24bitLocation = 0x000000;   // Starting address we'll use to R/W
  byte someData          = 0xDD;       // Some data to write to memory
  byte pageData[256];                  // Array of bytes to hold a page of data
  
  while (digitalRead(programButton) == HIGH) {
  if (buttonPressProg == 1) {
      buttonPressProg = 0;
    
      SPI.beginTransaction (AT25SF081);
      Serial.println       ("BEFORE...");
      readPage             (some24bitLocation, pageData);
      outputData           (pageData);
      write_byte           (some24bitLocation, someData);

      checkStatus();

      Serial.println       ();
      Serial.println       ("AFTER...");
      readPage             (some24bitLocation, pageData);
      outputData           (pageData);
      SPI.endTransaction   ();
    }
  }
}


// Function to prepare the Flash IC for SPI communication.
void AT25SF081_init()
{
  // Set the pin modes
  pinMode              (CS_AT25SF081,  OUTPUT);
  // pinMode              (holdPin,       OUTPUT);

  // Initialize the device for it's first interaction.
  // digitalWrite         (holdPin,      HIGH);
  digitalWrite         (CS_AT25SF081, LOW);
  digitalWrite         (CS_AT25SF081, HIGH);

  SPI.begin();
}

void checkStatus()
{
  byte readyBusy = 1;       // Preset the readyBusy variable as BUSY
  byte JUNK      = 0xFF;    // Junk data for MOSI output

  digitalWrite (CS_AT25SF081, LOW);
  SPI.transfer (OPCODE_READSTATUSREGISTER01);
  
  while (readyBusy == 1) {
    readyBusy = SPI.transfer(JUNK) & 0x01;
  }
  
  digitalWrite (CS_AT25SF081, HIGH);

}

void write_byte(long writeAddress, byte dataToProgram)
{

  // Set the WEL bit
  digitalWrite (CS_AT25SF081, LOW);
  SPI.transfer (OPCODE_WRITEENABLE);
  digitalWrite (CS_AT25SF081, HIGH);

  // Write a byte of data to writeAddress
  digitalWrite (CS_AT25SF081, LOW);
  SPI.transfer (OPCODE_PROGRAM);
  SPI.transfer ((writeAddress & 0xFF0000) >> 16);  // Byte address - MSB Sig Byte
  SPI.transfer ((writeAddress & 0x00FF00) >>  8);  // Byte address - MID Sig Byte
  SPI.transfer ((writeAddress & 0x0000FF) >>  0);  // Byte address - LSB Sig Byte
  SPI.transfer (dataToProgram);                    // Write data to address
  digitalWrite (CS_AT25SF081, HIGH);

}

void readPage(long readAddress, byte memoryData[])
{

  byte JUNK = 0xFF;    // Junk data for MOSI output

  digitalWrite        (CS_AT25SF081, LOW);

  SPI.transfer        (OPCODE_SLOWREAD_ARRAY);           // Send OpCode
  SPI.transfer        ((readAddress & 0xFF0000) >> 16);  // Byte address - MSB Sig Byte
  SPI.transfer        ((readAddress & 0x00FF00) >>  8);  // Byte address - MID Sig Byte
  SPI.transfer        ((readAddress & 0x0000FF) >>  0);  // Byte address - LSB Sig Byte

  for (int i = 0; i < 256; i++) {
    memoryData[i] =  SPI.transfer(JUNK);                   // Read byte from address
  }

  digitalWrite        (CS_AT25SF081, HIGH);

}

void outputData(byte memoryData[])
{

  for (int i = 0; i < 256; i = i + 8) {
    if (i <= 9)            Serial.print ("  ");
    if (i >  9 && i < 100) Serial.print (" ");

    Serial.print   (i);
    Serial.print   (": ");
    Serial.print   (memoryData[i],       HEX);
    Serial.print   (" ");
    Serial.print   (memoryData[i + 1],   HEX);
    Serial.print   (" ");
    Serial.print   (memoryData[i + 2],   HEX);
    Serial.print   (" ");
    Serial.print   (memoryData[i + 3],   HEX);
    Serial.print   (" ");
    Serial.print   (memoryData[i + 4],   HEX);
    Serial.print   (" ");
    Serial.print   (memoryData[i + 5],   HEX);
    Serial.print   (" ");
    Serial.print   (memoryData[i + 6],   HEX);
    Serial.print   (" ");
    Serial.println (memoryData[i + 7],   HEX);
  }

}
