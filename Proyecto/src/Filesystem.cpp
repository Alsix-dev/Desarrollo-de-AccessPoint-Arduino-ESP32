#include <LittleFS.h>
#include <Filesystem.h>

void montarFilesystem(){
  if (!LittleFS.begin(true)) {
    Serial.println("Error montando LittleFS");
    return;
  }

  Serial.println("LittleFS listo");
  servirArchivos();
}

void servirArchivos(){
  server.on("/index.html", HTTP_GET, handleIndex);
  server.on("/style.css", HTTP_GET, handleCSS);
}

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