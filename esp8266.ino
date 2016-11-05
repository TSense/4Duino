#include "MyWifi.h"

ESP8266WebServer server(80); // Define port 80 for the web server port

#define WARNING_PIN D0  // Led pin number
#define SENSOR_PIN A0   // Sensor pin number

// WiFi
char* WIFI_SSID = "ieeehotspot";
char* WIFI_PASSWORD = "Jk638td9og35";

void respond() {
  char response[50];  // variable to hold the data to send
  sprintf(response, "%d", analogRead(SENSOR_PIN));  // Convert data to string (char*)
  server.send(200, "text/plain", response); // Send data as response
}

void setup() {
  Serial.begin(115200); // Initializes serial with baud rate as a parameter
  connectWifi(WIFI_SSID, WIFI_PASSWORD); // Tries to connect to a wifi network with credentials as parameters
  Serial.println("");
  Serial.println(WiFi.localIP()); // Prints internal IP on serial TODO: Make it print on the LCD
  pinMode(SENSOR_PIN, INPUT);       // Defines sensor pin as being an input
  pinMode(WARNING_PIN, OUTPUT);     // Defines sensor pin as being an input
  digitalWrite(WARNING_PIN, LOW);   // Set the starting state of the led as turned off
  server.on("/", respond);          // Listen for HTTP/GET requests to respond appropriately
  server.begin();                   // Start web server
}

void loop() {
  server.handleClient();  // Makes sure to reconnect if wifi fails and other stuff
}
