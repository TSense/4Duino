#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "SPI.h"
#include "SparkFunBME280.h"
#include "Wire.h"
#include <SoftwareSerial.h>

#include "secrets.h" // comment these lines and uncomment the following
//#define SSID "**********"
//#define PASS "**********"

// Set default configs that will be changed at the first request (as defined in respond())
float hum = 1;
float humLow = 0;
float humHigh = 100;
float temp = 1;
float tempLow = 0;
float tempHigh = 100;

BME280 tempSensor;

ESP8266WebServer server(7568); // Define port 80 for the web server port

void respond() {
    // Gets configs from the GET request
    String str_tempLow = server.arg("templow");
    String str_tempHigh = server.arg("temphigh");
    String str_humLow = server.arg("humlow");
    String str_humHigh = server.arg("humhigh");
    tempLow = str_tempLow.toFloat();
    tempHigh = str_tempHigh.toFloat();
    humLow = str_humLow.toFloat();
    humHigh = str_humHigh.toFloat();

    // Cooks response
    String s = "";
    s += WiFi.macAddress();
    s += ";";
    s += temp;
    s += ";";
    s += hum;
    s += ";";
    server.send(200, "text/html", s); // Send response as html for library bug purpose
}

void setup() {
    Serial.begin(115200);
    pinMode(BUILTIN_LED, OUTPUT);
    digitalWrite(BUILTIN_LED, HIGH);

    tempSensor.settings.commInterface = I2C_MODE;
    tempSensor.settings.I2CAddress = 0x77;
    tempSensor.settings.runMode = 3; // 0 = Sleep Mode, 1 or 2= Force Mode, 3 = Normal mode
    tempSensor.settings.tStandby = 0; //  0 = 0.5ms, 1 = 62.5ms, 2 = 125ms, 3 = 250ms, 4 = 500ms, 5 = 1000ms, 6 = 10ms, 7 = 20ms
    tempSensor.settings.filter = 0; //  0 = filter off, 1 = coefficients = 2, 2 = coefficients = 4, 3 = coefficients = 8, 4 = coefficients = 16
    tempSensor.settings.tempOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)
    tempSensor.settings.pressOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)
    tempSensor.settings.humidOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)

    tempSensor.begin();

    delay(2000);

    WiFi.begin(SSID, PASS); // Sets Wifi credentials
    while (WiFi.status() != WL_CONNECTED) { // Try to connect until it does
        delay(100);
    }
    digitalWrite(BUILTIN_LED, LOW);

    server.on("/", respond); // Listen for HTTP/GET requests to respond appropriately
    server.begin();     // Start web server
}

void loop() {
    server.handleClient();

    temp = tempSensor.readTempC();
    hum = tempSensor.readFloatHumidity();

    if (Serial.available()) {
        // Cooks response
        String s = "";
        s += WiFi.macAddress();
        s += ";";
        s += temp;
        s += ";";
        s += hum;
        Serial.print(s);
        s = "";
    }
    delay(500);
}
