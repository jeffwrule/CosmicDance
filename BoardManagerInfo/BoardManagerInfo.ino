#include "TeensyBoardManager.h"

ArduinoBoardManager arduino = ArduinoBoardManager(); // required if you want to know the board name and specific features

void setup() {
Serial.begin(9600);

Serial.print("Board is compatible with Arduino ");
Serial.println(arduino.BOARD_NAME);

Serial.println("Speed/SRAM/Flash: ");
Serial.print(ArduinoBoardManager::MAX_MHZ);
Serial.println(ArduinoBoardManager::SRAM_SIZE);
Serial.println(ArduinoBoardManager::FLASH_SIZE);

// Board features (multiple serial ports on Mega, for example)
if (arduino.featureExists(ArduinoBoardManager::FEATURE_MULTIPLE_SERIAL)) {
Serial.println("Your board supports multiple serial connections");
}

}

void loop() {
}
