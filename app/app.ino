#include "Componente.h"
#include "Led.h"
#include "SensorTemperatura.h"
#include "Servidor.h"
#include "WifiHandler.h"

Servidor *servidor;
WifiHandler  *wifiHandler;

void setup() {
  Serial.begin(9600);
  delay(5000);

  wifiHandler = new WifiHandler("SSID", "PW");
  servidor = new Servidor();
}

void loop() {
  servidor->handler();
}
