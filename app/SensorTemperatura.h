#pragma once

#include "Componente.h"

#define SensorTemperatura_H

class SensorTemperatura: public Componente {
  public:
    SensorTemperatura(String nome, String descricao, int pino, int tipo): Componente(nome, descricao, pino, tipo) {};

    float getTemperatura();

  protected:
    float _temperatura;
};

float SensorTemperatura::getTemperatura() {
  return _temperatura;
}
