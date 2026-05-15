#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char *ssid = "Wifi-ESP32";
const char *password = "miwifiesp";

WebServer server(80);


// ===================================
// ===================================
// FILESYSTEM
// ===================================
// ===================================
void handleIndex(){
  File file = LittleFS.open("/index.html", "r");

  if (!file) {
    server.send(500, "text/plain", "Error cargando index.html");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

void handleCSS(){
  File file = LittleFS.open("/style.css", "r");

  if (!file) {
    server.send(500, "text/plain", "Error cargando style.css");
    return;
  }

  server.streamFile(file, "text/css");
  file.close();
}

void servirArchivos(){
  server.on("/index.html", HTTP_GET, handleIndex);
  server.on("/style.css", HTTP_GET, handleCSS);
}
// ===================================
// ===================================
// WEBSERVER
// ===================================
// ===================================

void handleNotFound(){
  String message = "Archivo no encontrado\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleInline(){
  server.send(200, "text/plain", "Usted a accedido al recurso inline''");
}

void clienteConecta(){
  // Verifica continuamente si un cliente intenta conectarse al servidor o enviar una peticion HTTP.
  server.handleClient();
}

void setup(void) {
  Serial.begin(115200);
  delay(2000);

  // Cargando construccion de red wifi.
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

  // Cargando asignacion de rol de servidor.
  server.on("/", HTTP_GET, handleIndex);
  server.on("/index.html", HTTP_GET, handleIndex);
  server.on("/style.css", HTTP_GET, handleCSS);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  // Cargando el filesystem y archivos.
  if (!LittleFS.begin(true)) {
    Serial.println("Error montando LittleFS");
    return;
  }

  Serial.println("LittleFS listo");
  servirArchivos();
}

void loop(void) {
  clienteConecta();
}

