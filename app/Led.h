#pragma once

#include "Componente.h"

#define Led_H

class Led: public Componente {
  public:   
    Led(String nome, String descricao, int pino, int tipo):Componente(nome, descricao, pino, tipo){};

    void setEstado(bool estadoLED);
    bool getEstado();

  protected:
    bool _estadoLED = false;
};

void Led::setEstado(bool estadoLED){
  _estadoLED = estadoLED;
}

bool Led::getEstado(){
  return _estadoLED;
}
