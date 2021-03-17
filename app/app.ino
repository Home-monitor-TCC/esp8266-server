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

  wifiHandler = new WifiHandler("", "");
  servidor = new Servidor(wifiHandler->getMACAddress());
}

void loop() {
  servidor->handler();
}
