#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define UNIQUE_ID 00001 // Number to differenciate measure points

#define WARNING_PIN D0  // Led pin number
#define ONE_WIRE_BUS A0   // Sensor pin number

// WiFi
/*char* WIFI_SSID = "ieeehotspot";
  char* WIFI_PASSWORD = "Jk638td9og35";*/
char* WIFI_SSID = "Vodafone-F543D9";
char* WIFI_PASSWORD = "038E3744D1";

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

ESP8266WebServer server(80);  // Define port 80 for the web server port

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices

DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.

int temp;

void respond() {
  char data[10]; // variable to hold the temperature as string
  sprintf(data, "%d;%d", UNIQUE_ID, temp);  // Arranges data in a coma separated string for easy data extraction
  server.send(200, "text/plain", data); // Send data as response
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("");
  Serial.println(WiFi.localIP());   // Prints internal IP on serial TODO: Make it print on the LCD
  pinMode(ONE_WIRE_BUS, INPUT);       // Defines sensor pin as being an input
  pinMode(WARNING_PIN, OUTPUT);     // Defines sensor pin as being an input
  digitalWrite(WARNING_PIN, LOW);   // Set the starting state of the led as turned off
  server.on("/", respond);          // Listen for HTTP/GET requests to respond appropriately
  server.begin();                   // Start web server
}

void loop() {
  server.handleClient();  // Makes sure to reconnect if wifi fails

  sensors.requestTemperatures(); // Send the command to get temperatures

  temp = sensors.getTempCByIndex(0);        // Gets the current temperature
  if (temp > TEMP_MAX || temp < TEMP_MIN) { //Checks if the temperature is within the defined limits
    digitalWrite(WARNING_PIN, HIGH);  // If the temperature is out of limits lights up the warning led
  } else {
    digitalWrite(WARNING_PIN, LOW);   // If the temperature is within the limits turns off the warning led
  }
}

  client.flush();*/
  s += WiFi.macAddress();
  s += ";";
  float temp = (rand() % 25) + 15; //TODO: Ler temperatura e humidade
  float hum = rand() % 100;
  s += temp;
  s += ";";
  s += hum;
  Serial.println("Response Sent");
  client.print(s);
}

#include <ESP8266WebServer.h>
// WiFi
char* WIFI_SSID = "ieeehotspot";
char* WIFI_PASSWORD = "Jk638td9og35";
ESP8266WebServer server(7568);  // Define port 80 for the web server port
void respond() {
  String s = "";
  s += WiFi.macAddress();
  s += ";";
  s += (rand() % 15) + 15;
  s += ";";
  s += rand() % 100;
  s += ";";
  server.send(200, "text/html", s); // Send data as response
  Serial.println("");
  Serial.println(WiFi.localIP());   // Prints internal IP on serial TODO: Make it print on the LCD
  server.on("/", respond);          // Listen for HTTP/GET requests to respond appropriately
  server.begin();                   // Start web server
}
void loop() {
  server.handleClient();  // Makes sure to reconnect if wifi fails
}