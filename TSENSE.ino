// Bibliotecas
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS D5
#define WRONG_TEMP_LAMP D1

// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);

// Armazena os valores constantes
#define ssid "ieeehotspot"
#define password "Jk638td9og35"
/*



    ADD UNIQUE ID



*/
#define uniqueID ""

// Armazena temperaturas minima e maxima
/*



    ADD tempMin and tempMax



*/
float tempMin = 20;
float tempMax = 30;

DallasTemperature sensors(&oneWire);
DeviceAddress sensor1;

// Prepara o server
WiFiServer server(80);

void setup() {

  // Prepara a serial
  Serial.begin(115200);
  delay(1);

  // Prepara a leitura dos sensores
  sensors.begin();
  delay(1);

  // Localiza e mostra enderecos dos sensores
  Serial.println("Localizando sensores DS18B20...");
  Serial.print("Foram encontrados ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" sensores.");
  if (!sensors.getAddress(sensor1, 0)) {
    Serial.println("Sensores nao encontrados !");
  }

  // Mostra o endereco do sensor encontrado no barramento
  Serial.print("Endereco sensor: ");
  mostra_endereco_sensor(sensor1);
  Serial.println();
  Serial.println();
  Serial.println();

  // Prepara a ligação a rede wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Inicia o wifi
  WiFi.begin(ssid, password);

  // Esperar ate conectar a rede wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Iniciar o servidor
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Preprar pinMode
  pinMode(WRONG_TEMP_LAMP, OUTPUT);
}

void loop() {

  // Verificar se o servidor tem algum cliente
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Esperar por um novo cliente
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Le a informacao do sensor
  sensors.requestTemperatures();
  delay(1);

  // Temperatura atual
  float tempA = sensors.getTempC(sensor1);
  delay(1);

  // Avisa caso a temperatura ultrapasse as pre-definidas
  if (tempA < tempMin || tempA > tempMax)  {
    wrong_temp();
  }
  delay(1);

  // Imprimir temperatureas no terminal
  Serial.print("Temperatura Atual: ");  Serial.print(tempA);
  Serial.print("Temperatura Minima: ");  Serial.print(tempMin);
  Serial.print("Temperatura Maxima: ");  Serial.println(tempMax);
  delay(1);

  // Ler o primeiro caracter do request
  String req = client.readStringUntil('\r');
  // HTTP 1.1 GET /temp
  // HOST: jdarrr.tk
  // Accept-enco
  Serial.println(req);
  client.flush();

  // Verificar se o request e verdadeiro
  if (req.indexOf("/temp") == -1)
  {
    Serial.println("invalid request");
    client.stop();
    return;
  }
  delay(1);

  client.flush();

  // Preparar a resposta
  String out = uniqueID;
  out += ";"; 
  out += tempA;
  // Enviar a resposta
  client.print(out);
  delay(1);
  Serial.println("Client disconnected");

}

void mostra_endereco_sensor(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {

    // Adiciona zeros se necessário
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void wrong_temp(void) {
  
}


