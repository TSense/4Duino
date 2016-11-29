#include <ESP8266WiFi.h>

WiFiServer server(7568); // Create an instance of the server and specify the port to listen on as an argument
const char* ssid = "ieeehotspot";
const char* password = "Jk638td9og35";

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin(); // Start the server

  Serial.println(WiFi.localIP()); //TODO: Make it print on the lcd
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("new client");
  String s = "";
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  // HTTP 1.1 GET /temp
  // HOST: jdarrr.tk
  // Accept-enco
  Serial.println(req);
  client.flush();

  if (req.indexOf("/temp") == -1) // Match the request
  {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  client.flush();
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
