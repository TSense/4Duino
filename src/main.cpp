#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "SparkFunBME280.h"
#include "Wire.h"
#include "SPI.h"

#define SSID "ieeehotspot"
#define PASS "Jk638td9og35"

float hum = 50;
float humLow = 0;
float humHigh = 100;
float temp = 20;
float tempLow = 10;
float tempHigh = 30;

BME280 tempSensor;

ESP8266WebServer server(7568); // Define port 80 for the web server port

void respond()
{
        String str_tempLow = server.arg("templow");
        String str_tempHigh = server.arg("temphigh");
        String str_humLow = server.arg("humlow");
        String str_humpHigh = server.arg("humhigh");
        tempLow = str_tempLow.toFloat();
        tempHigh = str_tempHigh.toFloat();
        humLow = str_tempLow.toFloat();
        humHigh = str_tempHigh.toFloat();
        String s = "";
        s += WiFi.macAddress();
        s += ";";
        s += temp;
        s += ";";
        s += hum;
        s += ";";
        server.send(200, "text/html", s); // Send data as response
}

void setup()
{
//***Driver settings********************************//
//commInterface can be I2C_MODE or SPI_MODE
//specify chipSelectPin using arduino pin names
//specify I2C address.  Can be 0x77(default) or 0x76

//For I2C, enable the following and disable the SPI section
        tempSensor.settings.commInterface = I2C_MODE;
        tempSensor.settings.I2CAddress = 0x77;

//For SPI enable the following and dissable the I2C section
//tempSensor.settings.commInterface = SPI_MODE;
//tempSensor.settings.chipSelectPin = 10;

//***Operation settings*****************************//

//renMode can be:
//  0, Sleep mode
//  1 or 2, Forced mode
//  3, Normal mode
        tempSensor.settings.runMode = 3; //Normal mode

//tStandby can be:
//  0, 0.5ms
//  1, 62.5ms
//  2, 125ms
//  3, 250ms
//  4, 500ms
//  5, 1000ms
//  6, 10ms
//  7, 20ms
        tempSensor.settings.tStandby = 0;

//filter can be off or number of FIR coefficients to use:
//  0, filter off
//  1, coefficients = 2
//  2, coefficients = 4
//  3, coefficients = 8
//  4, coefficients = 16
        tempSensor.settings.filter = 0;

//tempOverSample can be:
//  0, skipped
//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
        tempSensor.settings.tempOverSample = 16;

//pressOverSample can be:
//  0, skipped
//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
        tempSensor.settings.pressOverSample = 16;

//humidOverSample can be:
//  0, skipped
//  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
        tempSensor.settings.humidOverSample = 16;

        tempSensor.begin(); //Calling .begin() causes the settings to be loaded
        delay(10); //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.

        WiFi.begin(SSID, PASS); // Sets Wifi credentials
        while (WiFi.status() != WL_CONNECTED)
        { // Try to connect until it does
                delay(10);
        }
        server.on("/", respond); // Listen for HTTP/GET requests to respond appropriately
        server.begin();         // Start web server
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
        server.handleClient(); // Makes sure to reconnect if wifi fails
        temp = tempSensor.readTempC();
        hum = tempSensor.readFloatHumidity();
        if (temp < tempLow)
        {
                // Temperature is below the limit
        }
        else if (temp > tempHigh)
        {
                // Temperature is above the limit
        }
}
