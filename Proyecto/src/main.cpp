#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char *ssid = "Wifi-ESP32";
const char *password = "miwifiesp";

WebServer server(80);

String mensajeESP = "hola mundo";

// ===================================
// ===================================
// INTERACCION ESP32 CON FRONTEND
// ===================================
// ===================================
void handleMensaje(){
  server.send(200, "text/plain", mensajeESP);
}

//Primero el ESP usa handlemensaje para subir el mensaje. Cuando yo
//actualizo, envia una consulta aqui y actualiza el mensajeESP
//entonces ahora en la funcion "cargarmensaje" se carga el nuevo mensaje
//ya que habla con el servidor que detecto un cambio.
void handleEnviarMensaje(){
  if(server.hasArg("mensaje")){
    mensajeESP = server.arg("mensaje");

    Serial.println("Nuevo mensaje:");
    Serial.println(mensajeESP);
    
    server.send(200, "text/plain", "Mensaje actualizado");
  }
}

void handleGrafica(){

    String binario = "";

    // =====================================
    // Convertir mensaje a binario
    // =====================================

    for(int i = 0; i < mensajeESP.length(); i++){

        char c = mensajeESP[i];

        for(int bit = 7; bit >= 0; bit--){

            if(c & (1 << bit)){
                binario += "1";
            }
            else{
                binario += "0";
            }
        }
    }

    // =====================================
    // Crear SVG
    // =====================================

    String svg = "";

    svg += "<svg xmlns='http://www.w3.org/2000/svg' ";
    svg += "width='1200' height='220'>";

    svg += "<rect width='100%' height='100%' fill='white'/>";

    int x = 20;

    int anchoBit = 40;

    int yAnterior = 150;

    for(int i = 0; i < binario.length(); i++){

        char bit = binario[i];

        int yActual;

        if(bit == '1'){
            yActual = 50;
        }
        else{
            yActual = 150;
        }

        // ==============================
        // Línea vertical de transición
        // ==============================

        if(i != 0){

            svg += "<line ";
            svg += "x1='" + String(x) + "' ";
            svg += "y1='" + String(yAnterior) + "' ";
            svg += "x2='" + String(x) + "' ";
            svg += "y2='" + String(yActual) + "' ";
            svg += "stroke='black' ";
            svg += "stroke-width='3' />";
        }

        // ==============================
        // Línea horizontal del bit
        // ==============================

        svg += "<line ";
        svg += "x1='" + String(x) + "' ";
        svg += "y1='" + String(yActual) + "' ";
        svg += "x2='" + String(x + anchoBit) + "' ";
        svg += "y2='" + String(yActual) + "' ";
        svg += "stroke='black' ";
        svg += "stroke-width='3' />";

        yAnterior = yActual;

        x += anchoBit;
    }

    svg += "</svg>";

    server.send(200, "image/svg+xml", svg);
}

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

  //Registrar endpoint para la interaccion ESP32-Frontend
  server.on("/mensaje", HTTP_GET, handleMensaje);
  server.on("/enviar", HTTP_POST, handleEnviarMensaje);
  server.on("/grafica", HTTP_GET, handleGrafica);

  // ----------------------------------------------------
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

