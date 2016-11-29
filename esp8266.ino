#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi
char* WIFI_SSID = "ieeehotspot";
char* WIFI_PASSWORD = "Jk638td9og35";

int TEMP_MAX = 22;
int TEMP_MIN = 30;

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
}

void setup() {
  Serial.begin(115200); // Initializes serial with baud rate as a parameter
  int i = 0;
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Sets Wifi credentials
  while (WiFi.status()  !=  WL_CONNECTED) { // Try to connect until it does
    delay(100);
  }
  Serial.println("");
  Serial.println(WiFi.localIP());   // Prints internal IP on serial TODO: Make it print on the LCD
  server.on("/", respond);          // Listen for HTTP/GET requests to respond appropriately
  server.begin();                   // Start web server
}

void loop() {
  server.handleClient();  // Makes sure to reconnect if wifi fails
}
