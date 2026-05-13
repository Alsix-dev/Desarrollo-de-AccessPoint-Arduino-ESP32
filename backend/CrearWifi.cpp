#include <WiFi.h>

const char *ssid = "Wifi-ESP32";
const char *password = "miwifiesp";

void crearWifi(){
  WiFi.mode(WIFI_AP);

  bool ok = WiFi.softAP(ssid, password);
  if(!ok){
    Serial.println("Error creando el AP");
    return;
  }

  Serial.print("\n\n");
  Serial.print("Inicializando el access point (AP)");
  Serial.println(ssid);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // Serial.println(WiFi.softAP(ssid, password));
}