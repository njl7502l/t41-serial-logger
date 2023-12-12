#include "SdFat.h"
#include "USBHost_t36.h"
#define USBBAUD 921600

// Use Teensy SDIO
#define SD_CONFIG SdioConfig(FIFO_SDIO)

/// @brief ボーレート
uint32_t baud = USBBAUD;

USBHost myusb;
USBSerial_BigBuffer userial(myusb, 1);

SdFs sd;
FsFile file;

/// @brief SDカードに書き込み
/// @param message 配列として定義したバイト列
/// @param n 配列の長さ
void writeSD(char *buf, uint16_t len) {
    digitalWrite(13, HIGH);
    file = sd.open("/logs.txt", FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.seekEnd();
    file.write(buf, len);
    file.close();
    digitalWrite(13, LOW);
}

void setup() {
    pinMode(13, OUTPUT);
    delay(500);
    Serial.begin(baud);

    if (!sd.begin(SD_CONFIG)) {
        Serial.println("Card Mount Failed");
        return;
    }
    char divider[100] = "########################################################"
                        "########################\n";

    writeSD(divider, sizeof(divider));

    myusb.begin();
    userial.begin(baud);
}

/// @brief USBホスト受信バッファ
char buffer[4096];
uint16_t n, rd;
void loop() {
    rd = userial.available();
    if (rd > 0) {
        n = userial.readBytes((char *)buffer, rd);
        writeSD(buffer, n);
    }
    yield();
}