#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "SPI.h"
#include "SparkFunBME280.h"
#include "Wire.h"
#include <SoftwareSerial.h>

char ssid[] = "ieeehotspot";        //  your network SSID (name)
char pass[] = "Jk638td9og35";       // your network password

// Set default configs that will be changed at the first request (as defined in respond())
float hum = 1;
float humLow = 0;
float humHigh = 100;
float temp = 1;
float tempLow = 0;
float tempHigh = 100;
float alt = 1;


WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacekt[] = "Hi there! Got the message :-)";  // a reply string to send back


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
    s += alt;
    server.send(200, "text/html", s); // Send response as html for library bug purpose
}

void setup() {
    pinMode(D0, OUTPUT);
    tempSensor.settings.commInterface = I2C_MODE;
    tempSensor.settings.I2CAddress = 0x77;
    tempSensor.settings.runMode = 3; // 0 = Sleep Mode, 1 or 2= Force Mode, 3 = Normal mode
    tempSensor.settings.tStandby = 0; //  0 = 0.5ms, 1 = 62.5ms, 2 = 125ms, 3 = 250ms, 4 = 500ms, 5 = 1000ms, 6 = 10ms, 7 = 20ms
    tempSensor.settings.filter = 0; //  0 = filter off, 1 = coefficients = 2, 2 = coefficients = 4, 3 = coefficients = 8, 4 = coefficients = 16
    tempSensor.settings.tempOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)
    tempSensor.settings.pressOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)
    tempSensor.settings.humidOverSample = 1; //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively (0 to skip)

    tempSensor.begin();

    delay(1000);
    Serial.begin(115200);
    Serial.println();
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) { // Try to connect until it does
        digitalWrite(D0, LOW);
        delay(100);
        Serial.print(".");
        digitalWrite(D0, HIGH);
        delay(50);
    }
    Serial.println();
    Serial.println("Connected! IP: ");
    Serial.print(WiFi.localIP());

    server.on("/", respond); // Listen for HTTP/GET requests to respond appropriately
    server.begin();     // Start web server

     Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop() {
    server.handleClient();

    temp = tempSensor.readTempC();
    hum = tempSensor.readFloatHumidity();
    alt = tempSensor.readFloatAltitudeFeet();

    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        digitalWrite(D0, LOW);
        // receive incoming UDP packets
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        int len = Udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        // send back a reply, to the IP address and port we got the packet from
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyPacekt);
        Udp.endPacket();
        delay(50);
        digitalWrite(D0, HIGH);
        
    }

    delay(100);
}
