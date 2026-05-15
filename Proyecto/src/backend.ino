#include <CrearWifi.h>
#include <CrearServidor.h>
void setup(void) {
  Serial.begin(115200);
  delay(2000);

  // Cargando construccion de red wifi.
  crearWifi();

  // Cargando asignacion de rol de servidor.
  mainWebServer();

  // Cargando el filesystem y archivos.
  montarFilesystem();
}

void loop(void) {
  clienteConecta();
}

