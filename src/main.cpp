#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "SPI.h"
#include "SparkFunBME280.h"
#include "Wire.h"

#define SSID "SSID"
#define PASS "PASS"

#define HUM_LED 1
#define TEMP_LED 14

// Set default configs that will be changed at the first request (as defined in respond())
float hum = 0;
float humLow = 0;
float humHigh = 100;
float temp = 0;
float tempLow = 10;
float tempHigh = 30;

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
  // Setup Warning Leds
  pinMode(HUM_LED, OUTPUT);
  pinMode(TEMP_LED, OUTPUT);
  digitalWrite(HUM_LED, LOW);
  digitalWrite(TEMP_LED, LOW);

  tempSensor.settings.commInterface = I2C_MODE;
  tempSensor.settings.I2CAddress = 0x77;
  tempSensor.settings.runMode = 3; // 0 = Sleep Mode, 1 or 2= Force Mode, 3 = Normal mode
  tempSensor.settings.tStandby = 0; //  0 = 0.5ms, 1 = 62.5ms, 2 = 125ms, 3 = 250ms, 4 = 500ms, 5 = 1000ms, 6 = 10ms, 7 = 20ms
  tempSensor.settings.filter = 0; //  0 = filter off, 1 = coefficients = 2, 2 = coefficients = 4, 3 = coefficients = 8, 4 = coefficients = 16
  tempSensor.settings.tempOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)
  tempSensor.settings.pressOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)
  tempSensor.settings.humidOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)

  tempSensor.begin();

  WiFi.begin(SSID, PASS);                 // Sets Wifi credentials
  while (WiFi.status() != WL_CONNECTED) { // Try to connect until it does
    delay(10);
  }
  server.on("/", respond); // Listen for HTTP/GET requests to respond appropriately
  server.begin();     // Start web server
}

void loop() {
  server.handleClient(); // Makes sure to reconnect if wifi fails

  temp = tempSensor.readTempC();
  hum = tempSensor.readFloatHumidity();

  if (hum < humLow || hum > humHigh) {
    digitalWrite(HUM_LED, HIGH);
  } else {
    digitalWrite(HUM_LED, LOW);
  }

  if (temp < tempLow || temp > tempHigh) {
    digitalWrite(TEMP_LED, HIGH);
  } else {
    digitalWrite(TEMP_LED, LOW);
  }
}
