#include "Arduino.h"
#include "UTFT.h"

extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

UTFT myGLCD(ITDB32S, 38, 39, 40, 41);

void setup() {
    Serial1.begin(115200);
    myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    Serial1.write("IP");
    delay(100);
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.setColor(0, 255, 0);
    myGLCD.print(Serial1.readString(), 300, 200, 180);
}

void loop() {

    if (Serial.available() > 0) {

    }
    delay(1000);
}