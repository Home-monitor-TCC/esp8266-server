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
    OneWire pinoGPIO;
    DallasTemperature sensor; 
};

SensorTemperatura::SensorTemperatura(String nome, String descricao, int pino, int tipo): Componente(nome, descricao, pino, tipo) {
        OneWire owTemp(pino);

        pinoGPIO = owTemp;
        
        DallasTemperature dtTemp(&pinoGPIO);
        
        sensor = dtTemp;
        sensor.begin();
};

float SensorTemperatura::getTemperatura() {
  _temperatura = sensor.getTempCByIndex(0);
  
  return _temperatura;
};
