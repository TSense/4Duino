#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h> 
#include <OneWire.h>
#include <DallasTemperature.h>

#define WARNING_PIN D0  // Led pin number
#define SENSOR_PIN D5   // Sensor pin number

// WiFi
/*#define WIFI_SSID = "ieeehotspot";
  #define WIFI_PASSWORD = "Jk638td9og35";*/
#define WIFI_SSID = "Vodafone-F543D9";
#define WIFI_PASSWORD = "038E3744D1";

#define uniqueID ""

int TEMP_MAX = 22;
int TEMP_MIN = 30;

ESP8266WebServer server(80);  // Define port 80 for the web server port

OneWire oneWire(SENSOR_PIN); // Setup a oneWire instance to communicate with any OneWire devices

DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.

/* 
 *  Esta mal se tiver mais de 1 sensor por nodeMCU
 *  E preciso enviar uniqueID - VERIFICAR
 */
void respond() {
  string data; // variable to hold the temperature as string
  sprintf(data, "%d", sensors.getTempCByIndex(0));  // Convert data to string (char*)
  delay(1);
  data += ";"; 
  data += uniqueID;
  server.send(200, "text/plain", data); // Send data as response
  delay(1);
}

void setup() {
  Serial.begin(115200); // Initializes serial with baud rate as a parameter
  WiFi.begin(ssid, pass); // Sets Wifi credentials
  delay(1);
  while (WiFi.status()  !=  WL_CONNECTED) delay(500); // Try to connect until it does
  Serial.println("");
  Serial.println(WiFi.localIP());   // Prints internal IP on serial TODO: Make it print on the LCD
  pinMode(WARNING_PIN, OUTPUT);     // Defines sensor pin as being an input
  digitalWrite(WARNING_PIN, LOW);   // Set the starting state of the led as turned off
  server.on("/", respond);          // Listen for HTTP/GET requests to respond appropriately
  delay(1);
  server.begin();                   // Start web server
  delay(1);
}

void loop() {
  /*
   * Aprender o que isto faz e arranjar solução caso não tenha net para continuar
   */
  server.handleClient();  // Makes sure to reconnect if wifi fails and other stuff

  /*
   * Aprender o que isto faz
   */
  sensors.requestTemperatures(); // Send the command to get temperatures

  int temp = sensors.getTempCByIndex(0);        // Gets the current temperature
  delay(1);
  if (temp > TEMP_MAX || temp < TEMP_MIN) { //Checks if the temperature is within the defined limits
    digitalWrite(WARNING_PIN, HIGH);  // If the temperature is out of limits lights up the warning led
  } else {
    digitalWrite(WARNING_PIN, LOW);   // If the temperature is within the limits turns off the warning led
  }
  delay(1);
}
