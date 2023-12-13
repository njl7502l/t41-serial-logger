#include "SdFat.h"
#include "USBHost_t36.h"
#include <TimeLib.h>

#define USBBAUD 921600

// Use Teensy SDIO
#define SD_CONFIG SdioConfig(FIFO_SDIO)

/// @brief ボーレート
uint32_t baud = USBBAUD;

USBHost myusb;
USBSerial_BigBuffer userial(myusb, 1);

SdFs sd;
FsFile file;

int hoge;

String fileName;

time_t getTeensy3Time() {
    return Teensy3Clock.get();
}

String filDigit(int digits) {
    if (digits < 10) {
        return '0' + String(digits);
    } else {
        return String(digits);
    }
}
String getDatetimeString() {
    String date = filDigit(year()) + '-' + filDigit(month()) + '-' + filDigit(day());
    String time = filDigit(hour()) + '-' + filDigit(minute()) + '-' + filDigit(second());
    return date + "_" + time;
}

/// @brief SDカードに書き込み
/// @param message 配列として定義したバイト列
/// @param n 配列の長さ
void writeSD(char *buf, uint16_t len) {
    digitalWrite(13, HIGH);
    file = sd.open(fileName.c_str(), FILE_WRITE);
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }
    file.seekEnd();
    file.print(getDatetimeString());
    file.print(" >\t");
    file.write(buf, len);
    file.close();
    digitalWrite(13, LOW);
}

void setup() {
    pinMode(13, OUTPUT);
    Serial.begin(baud);

    setSyncProvider(getTeensy3Time);

    if (timeStatus() != timeSet) {
        fileName = "/logs.txt";
    } else {
        fileName = "/" + getDatetimeString() + ".txt";
    }

    if (!sd.begin(SD_CONFIG)) {
        Serial.println("Card Mount Failed");
        return;
    }

    file.open(fileName.c_str(), O_WRONLY | O_CREAT);
    char divider[] = "########################################################\n";

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