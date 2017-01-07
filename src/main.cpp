#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "SPI.h"
#include "SparkFunBME280.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

#define SSID "SSID"
#define PASS "PASS"

#define HUM_LED 2
#define TEMP_LED 0

// Set default configs that will be changed at the first request (as defined in respond())
float hum = 1;
float humLow = 0;
float humHigh = 100;
float temp = 1;
float tempLow = 0;
float tempHigh = 100;

BME280 tempSensor;

ESP8266WebServer server(7568); // Define port 80 for the web server port

LiquidCrystal_I2C lcd(0x3F, 16, 2);

uint8_t backslash[8] = {
    0b00000,
    0b10000,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b00000,
    0b00000
};

int state = 0;

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
  server.send(200, "text/html", s); // Send response as html for library bug purpose
}

void setup() {
  // LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, backslash);
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print(" Brought you by  ");  //Welcome Msg
  lcd.setCursor(0,1); //Start at character 0 on line 0
  lcd.print("    TSense     ");

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

  delay(2000);

  WiFi.begin(SSID, PASS); // Sets Wifi credentials
  lcd.clear();  //clear the LCD
  lcd.print("   Connecting   ");
  int state = 0;
  while (WiFi.status() != WL_CONNECTED) { // Try to connect until it does
    lcd.setCursor(7,1); //Start at character 0 on line 0
    switch(state){
      case 0:
      lcd.print("-");
      break;
      case 1:
      lcd.write(0);
      break;
      case 2:
      lcd.print("|");
      break;
      case 3:
      lcd.print("/");
      break;
    }
    if(state < 3) state++;
    else state = 0;
    delay(400);
  }
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("   WiFi Found   ");  //Welcome Msg
  lcd.setCursor(2,1); //Start at character 0 on line 0
  lcd.print(WiFi.localIP());

  server.on("/", respond); // Listen for HTTP/GET requests to respond appropriately
  server.begin();     // Start web server
  delay(1000);
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Tempe");
  lcd.setCursor(9,0);
  lcd.print("Humid");
}

void loop() {
  server.handleClient();
  lcd.setCursor(15, 0);
  if(WiFi.status() != WL_CONNECTED){
    switch(state){
      case 0:
      lcd.print("-");
      break;
      case 1:
      lcd.write(0);
      break;
      case 2:
      lcd.print("|");
      break;
      case 3:
      lcd.print("/");
      break;
    }
    if(state < 3) state++;
    else state = 0;
  } else{
    lcd.print(" ");
  }

  temp = tempSensor.readTempC();
  hum = tempSensor.readFloatHumidity();

  lcd.setCursor(2,1);
  lcd.print(temp);
  lcd.setCursor(9,1);
  lcd.print(hum);

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
  delay(1000);
}
