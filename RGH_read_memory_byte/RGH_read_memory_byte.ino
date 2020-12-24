#include <SPI.h>                            // Include the SPI library

SPISettings AT25SF081(8000000, MSBFIRST, SPI_MODE0);

// Sets the pin values for the AT25SF081 and MCP3008
const int  CS_AT25SF081   = 10;             // Flash Memory Chip Select - Active Low
// const int  holdPin        = 9;              // HOLD Pin - Active Low

// Set bits for the various Flash IC OpCodes (see datasheet)        
const byte OPCODE_SLOWREAD_ARRAY = (0x03);  // Memory read up <= 50Mhz (see datasheet)
const byte OPCODE_FASTREAD_ARRAY = (0x0B);  // Memory read up <= 85Mhz (see datasheet)

void setup() {
  Serial.begin(9600);        
  SPI.begin();
  
  // Set the pin modes
  pinMode              (CS_AT25SF081,  OUTPUT);
  // pinMode              (holdPin,       OUTPUT);
  
  // Initialize the device for it's first interaction.
  // digitalWrite         (holdPin,      HIGH);
  digitalWrite         (CS_AT25SF081, LOW);
  digitalWrite         (CS_AT25SF081, HIGH); 

  long some24bitLocation = 0x0FE8FF;

  byte memoryData = 0;
  byte JUNK = 0xFF;    // Junk data for MOSI output

  SPI.beginTransaction (AT25SF081);
  digitalWrite         (CS_AT25SF081, LOW);
  SPI.transfer         (OPCODE_SLOWREAD_ARRAY);

  SPI.transfer ((some24bitLocation & 0xFF0000) >> 16);  // Byte address - MSB Sig Byte
  SPI.transfer ((some24bitLocation & 0x00FF00) >>  8);  // Byte address - MID Sig Byte
  SPI.transfer ((some24bitLocation & 0x0000FF) >>  0);  // Byte address - LSB Sig Byte
  memoryData =       SPI.transfer(JUNK);

  digitalWrite       (CS_AT25SF081, HIGH);
  SPI.endTransaction ();  

  // Display the byte returned in the serial monitor with pretty formatting
  Serial.print    ("Data in memory address 0x0");
  Serial.print    (some24bitLocation, HEX);
  Serial.print    (": 0x");
  Serial.println  (memoryData, HEX);
  
}

void loop() {
  // put your main code here, to run repeatedly:

}
