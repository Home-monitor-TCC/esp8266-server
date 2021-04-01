#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include "Componente.h"

#define SensorTemperatura_H

//Modelo do sensor usado: DS18B20

class SensorTemperatura: public Componente {
  public:
    SensorTemperatura(String nome, String descricao, int pino, int tipo);

    float getTemperatura();

  protected:
    float _temperatura;
};

SensorTemperatura::SensorTemperatura(String nome, String descricao, int pino, int tipo):Componente(nome, descricao, pino, tipo) {};

float SensorTemperatura::getTemperatura() {
  OneWire gpio(_pino);
  DallasTemperature sensores(&gpio);
  DeviceAddress sensor;

  sensores.begin();
  sensores.getAddress(sensor, 0);

  sensores.requestTemperatures();
  _temperatura = sensores.getTempC(sensor);
 
  return _temperatura;
};
