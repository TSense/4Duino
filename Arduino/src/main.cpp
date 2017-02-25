#include "Arduino.h"
#include "UTFT.h"

extern uint8_t BigFont[];

UTFT myGLCD(SSD1289, 38, 39, 40, 41);


void setup() {
    Serial1.begin(115200);
    myGLCD.InitLCD();
    myGLCD.clrScr();
    myGLCD.setFont(BigFont);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("TSense", 120, 110, 0);
    delay(1500);
    myGLCD.clrScr();
}

void loop() {
    String mac = Serial1.readStringUntil(';');
    String temperature = Serial1.readStringUntil(';');
    String humidity = Serial1.readStringUntil(';');
    myGLCD.print(mac, 20, 20, 0);
    myGLCD.print(temperature, 20, 110, 0);
    myGLCD.print(humidity, 150, 150, 0);
}