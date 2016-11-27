#include <ESP8266WiFi.h>

const char* ssid = "Vodafone-F543D9";
const char* password = "038E3744D1";

WiFiServer server(7568); // Create an instance of the server and specify the port to listen on as an argument

void setup() {
  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  server.begin(); // Start the server
  
  Serial.println(""); 
  Serial.println(WiFi.localIP()); //TODO: Make it print on the lcd
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("new client");
  while (!client.available()) { // Wait until the client sends some data
    delay(1);
  }

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

  // Prepare the response
  String s = WiFi.macAddress();
  s += ";";
  const int sensorPin = 5;
  float temp = (rand()%25)+15; //TODO: Ler temperatura e humidade
  float hum = rand()%100;
  s += temp;
  s += ";";
  s += hum;

  client.print(s); // Send the response to the client
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
