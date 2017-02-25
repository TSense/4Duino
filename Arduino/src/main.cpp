#include "Arduino.h"
#include "UTFT.h"

extern uint8_t BigFont[];

UTFT myGLCD(SSD1289, 38, 39, 40, 41);

String temperature = "";

void setup() {
    Serial1.begin(115200);
    myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("TSense", 120, 110, 0);
    delay(3000);
    myGLCD.clrScr();
    if (Serial1.available() > 0) {
        Serial1.print("IP");
        delay(2000);
        myGLCD.print(Serial.readString(), 20, 20, 0);
    }
}

void loop() {
    if (Serial1.available() > 0) {
        Serial1.print("TEMP");
        delay(2000);
        temperature = Serial1.readString();
        myGLCD.print(temperature, 120, 110, 0);
    }
}