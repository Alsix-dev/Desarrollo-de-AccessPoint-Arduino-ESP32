/*
 * Ejemplo de servidor Web para ESP32
 */
#include <CrearWifi.h>
#include <CrearServidor.h>

void setup(void) {
  Serial.begin(115200);
  delay(2000);

  // Cargando construccion de red wifi.
  crearWifi();

  // Cargando asignacion de rol de servidor.
  mainWebServer();
}

void loop(void) {
  clienteConecta();
}

