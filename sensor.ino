//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//
// Define LOG_MESSAGES to a serial port to send SPE errors messages to. Do not use the same Serial port as SPE
#define LOG_MESSAGES Serial

#define RESETLINE     30

#define DisplaySerial Serial1


#include "sensorConst.h"

#include "Picaso_Serial_4DLib.h"
#include "Picaso_Const4D.h"

Picaso_Serial_4DLib Display(&DisplaySerial);

#define ESPRESET 17
#include <SoftwareSerial.h>
#define ESPserial SerialS
SoftwareSerial SerialS(8, 9) ;

#include "ESP8266.h"
#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "SparkFunBME280.h"
ESP8266 wifi(SerialS,19200);

#define SSID "ieeehotspot"
#define PASS "Jk638td9og35"
#define rpi_ip "192.168.1.75";

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

void ota(){
    ESPhttpUpdate.update(rpi_ip, 3000, "/ota");
}

// routine to handle Serial errors
void mycallback(int ErrCode, unsigned char Errorbyte)
{
#ifdef LOG_MESSAGES
  const char *Error4DText[] = {"OK\0", "Timeout\0", "NAK\0", "Length\0", "Invalid\0"} ;
  LOG_MESSAGES.print(F("Serial 4D Library reports error ")) ;
  LOG_MESSAGES.print(Error4DText[ErrCode]) ;
  if (ErrCode == Err4D_NAK)
  {
    LOG_MESSAGES.print(F(" returned data= ")) ;
    LOG_MESSAGES.println(Errorbyte) ;
  }
  else
    LOG_MESSAGES.println(F("")) ;
    return;
#else
#define led 13
  while (1)
  {
    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(200);                // wait for a second
    digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
    delay(200);                // wait for a second
  }
#endif
}
// end of routine to handle Serial errors

word hndl ;

void setup()
{
// Ucomment to use the Serial link to the PC for debugging
Serial.begin(115200) ;        // serial to USB port
// Note! The next statement will stop the sketch from running until the serial monitor is started
//       If it is not present the monitor will be missing the initial writes
while (!Serial) ;             // wait for serial to be established

  pinMode(RESETLINE, OUTPUT);       // Display reset pin
  digitalWrite(RESETLINE, 1);       // Reset Display, using shield
  delay(100);                       // wait for it to be recognised
  digitalWrite(RESETLINE, 0);       // Release Display Reset, using shield
// Uncomment when using ESP8266
  pinMode(ESPRESET, OUTPUT);        // ESP reset pin
  digitalWrite(ESPRESET, 1);        // Reset ESP
  delay(100);                       // wait for it t
  digitalWrite(ESPRESET, 0);        // Release ESP reset
  delay(3000) ;                     // give display time to startup

  // now start display as Serial lines should have 'stabilised'
  DisplaySerial.begin(200000) ;     // Hardware serial to Display, same as SPE on display is set to
  Display.TimeLimit4D = 5000 ;      // 5 second timeout on all commands
  Display.Callback4D = mycallback ;

  ESPserial.begin(115200) ;         // assume esp set to 115200 baud, it's default setting
                                    // what we need to do is attempt to flip it to 19200
                                    // the maximum baud rate at which software serial actually works
                                    // if we run a program without resetting the ESP it will already be 19200
                                    // and hence the next command will not be understood or executed
  ESPserial.println("AT+UART_CUR=19200,8,1,0,0\r\n") ;
  ESPserial.end() ;
  delay(10) ;                         // Necessary to allow for baud rate changes
  ESPserial.begin(19200) ;            // start again at a resonable baud rate
  Display.gfx_ScreenMode(PORTRAIT) ; // change manually if orientation change
  Display.putstr("Mounting...\n");
  if (!(Display.file_Mount()))
  {
    while(!(Display.file_Mount()))
    {
      Display.putstr("Drive not mounted...");
      delay(200);
      Display.gfx_Cls();
      delay(200);
    }
  }
//hFontn = Display.file_LoadImageControl("NoName1.dnn", "NoName1.gnn", 1); // Open handle to access uSD fonts, uncomment if required and change nn to font number
//hstrings = Display.file_Open("sensor.txf", 'r') ;                            // Open handle to access uSD strings, uncomment if required
  hndl = Display.file_LoadImageControl("sensor.dat", "sensor.gci", 1);
  // put your setup code here, to run once:
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

  Serial.begin(115200); // Initializes serial with baud rate as a parameter
  //Calling .begin() causes the settings to be loaded
  delay(10); //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
  Serial.println(tempSensor.begin(), HEX);

  WiFi.begin(ssid, password); // Sets Wifi credentials
  while (WiFi.status() != WL_CONNECTED)
  { // Try to connect until it does
    delay(10);
  }
  Serial.println(WiFi.localIP()); // Prints internal IP on serial TODO: Make it print on the LCD
  server.on("/", respond);        // Listen for HTTP/GET requests to respond appropriately
  server.on("/upgrade", ota);
  server.begin();                 // Start web server
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  // put your main code here, to run repeatedly:
  server.handleClient(); // Makes sure to reconnect if wifi fails
  temp = tempSensor.readTempC();
  hum = tempSensor.readFloatHumidity();
  if (temp < tempLow)
  {
    Serial.println("Temperature is below the limit!"); // TODO: Make background blue!
  }
  else if (temp > tempHigh)
  {
    Serial.println("Temperature is above the limit!"); // TODO: Make background red!
  }
}
