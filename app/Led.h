#pragma once

#include "Componente.h"

#define Led_H

class Led: public Componente {
  public:

    Led(String nome, String descricao, int pino, int tipo);
    
    void setEstado(bool estadoLED);
    bool getEstado();

  protected:
    bool _estadoLED = false;
};

Led::Led(String nome, String descricao, int pino, int tipo):Componente(nome, descricao, pino, tipo) {
  pinMode(pino, OUTPUT);
}

void Led::setEstado(bool estadoLED) {  
  _estadoLED = estadoLED;
  digitalWrite(_pino, _estadoLED);
}

bool Led::getEstado() {
  _estadoLED = digitalRead(_pino);
  return _estadoLED;
}
