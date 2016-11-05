#include "MyWifi.h"

void connectWifi(char* ssid, char* pass) {
  int i = 0;
  WiFi.begin(ssid, pass); // Sets Wifi credentials
  while (WiFi.status()  !=  WL_CONNECTED) { // Try to connect until it does
    delay(100);
  }
}
