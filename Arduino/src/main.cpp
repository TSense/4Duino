#include "Arduino.h"
#include "UTFT.h"

extern uint8_t BigFont[];

UTFT myGLCD(ITDB32S, 38, 39, 40, 41);

void setup() {
    Serial1.begin(115200);
    myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("TSense", 120, 110, 0);
    delay(3000);
    myGLCD.clrScr();

}

void loop() {
    if (Serial.available() > 0) {

    }
    delay(1000);
}