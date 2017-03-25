#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "SPI.h"
#include "SparkFunBME280.h"
#include "Wire.h"
#include "md5.h"

#define ssid ""
#define pass ""

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
MD5Builder nonce_md5;

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
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);

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
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Try to connect until it does
    digitalWrite(D0, LOW);
    delay(50);
    Serial.print(".");
    digitalWrite(D0, HIGH);
    delay(50);
    i++;
    if (i > 200) {
      digitalWrite(D0, LOW);
      delay(1000);
      ESP.reset();
    }
  }
  Serial.println();
  Serial.println("Connected! IP: ");
  Serial.print(WiFi.localIP());

  server.on("/", respond); // Listen for HTTP/GET requests to respond appropriately
  server.begin();     // Start web server

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

char ROT13(char source) {
  if ((tolower(source) - 'a') < 14)
    return source + 13;

  return source - 13;
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
    digitalWrite(D5, HIGH);
    digitalWrite(D6, HIGH);
    digitalWrite(D7, HIGH);

    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }

    char ip[20];
    strcpy(ip, WiFi.localIP().toString().c_str());
    char aux[4];
    String responseBuffer(incomingPacket);
    {
      int i=strlen(ip);
      while(ip[i]!='.'){
        i--;
      }
      i++;
      Serial.println(i+ip);
      strncpy(aux, ip+i, 4);
      //Serial.println("IP: "+ip);
    }
    
    {
      for(int i=0;i<strlen(aux);i++){
          responseBuffer[i+8]=aux[i];
        }
        Serial.println("After insering IP: "+responseBuffer);
    }


    String auxA;
    String auxB;
    for (int i = 0; i < responseBuffer.length() / 2; i++)
    {
      char temp = responseBuffer[i];
      responseBuffer[i] = responseBuffer[responseBuffer.length() - i - 1];
      responseBuffer[responseBuffer.length() - i - 1] = temp;
    }

    auxA = responseBuffer.substring(0, 16);
    auxB = responseBuffer.substring(16, 32);
    Serial.println("auxA: " + auxA);
    Serial.println("auxB: " + auxB);
    responseBuffer = auxB + auxA;

    /* for (int i = 0; i < responseBuffer.length(); i++){
         if(i%2==0){
           responseBuffer[i]=ROT13(responseBuffer[i]);
         }
      }*/
    Serial.println("afterROT13 : " + responseBuffer);
    responseBuffer[23] = '2';

    Serial.println("afterMagic : " + responseBuffer);


    char response[33];
    nonce_md5.begin();
    nonce_md5.add(responseBuffer);
    nonce_md5.calculate();
    nonce_md5.getChars(response);

    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    char rsp[600];
    sprintf(rsp, "%s;%s", incomingPacket, response);
    Udp.write(rsp);
    Udp.endPacket();
    delay(50);
    digitalWrite(D0, HIGH);
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
  }

  delay(100);
}