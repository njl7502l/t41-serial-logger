#include "SdFat.h"
 
// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)
 
SdFs sd;
FsFile file;

void writeSD(String message){
    file = sd.open("/logs.txt", FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    file.println(message);
    file.close();
}

void setup() {
    Serial.begin(921600);

    delay(500);
    if(!sd.begin(SD_CONFIG)){
        Serial.println("Card Mount Failed");
        return;
    }
    String divider = "###################################################################";
    writeSD(divider);
}

String incomingByte;
void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.readStringUntil('\n');
    writeSD(incomingByte);
  }
  yield();
}