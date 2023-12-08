#include "SdFat.h"
#include "USBHost_t36.h"
#define USBBAUD 115200

// Use Teensy SDIO
#define SD_CONFIG  SdioConfig(FIFO_SDIO)
 
uint32_t baud = USBBAUD;
uint32_t format = USBHOST_SERIAL_8N1;
USBHost myusb;
USBSerial userial(myusb);

SdFs sd;
FsFile file;

void writeSD(char* message, uint16_t n ){
    digitalWrite(13, HIGH);
    file = sd.open("/logs.txt", FILE_WRITE);
    if(!file){
      Serial.println("Failed to open file for writing");
      return;
    }
    file.seekEnd();
    file.write(message, n);
    file.close();
    digitalWrite(13,LOW);
}

void setup() {
    pinMode(13, OUTPUT);
    delay(500);
    Serial.begin(115200);

    if(!sd.begin(SD_CONFIG)){
      Serial.println("Card Mount Failed");
      return;
    }
    char divider[100] = "################################################################################\n";

    writeSD(divider, sizeof(divider));

    myusb.begin();
    userial.begin(baud);
}

// String incomingByte;
char buffer[512];
uint16_t n, rd;
void loop() {
  rd = userial.available();
  if (rd > 0) {
    n = userial.readBytes((char *)buffer, rd);
    writeSD(buffer, n);
  }
  yield();
}