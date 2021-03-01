#pragma once

#define Componente_H

class Componente {
  public:
    Componente(String nome, String descricao, int pino, int tipo);

    void setPino(int pino);
    int getPino();

    void setNome(String nome);
    String getNome();

    void setDescricao(String descricao);
    String getDescricao();

    void setTipo(int tipo);
    int getTipo();

    //1 - LED
    //2 - SENSORES

  protected:
    int _pino;
    int _tipo;
    String _nome;
    String _descricao;
};

Componente::Componente(String nome, String descricao, int pino, int tipo) {
  setNome(nome);
  setDescricao(descricao);
  setPino(pino);
  setTipo(tipo);
}

void Componente::setPino(int pino) {
  _pino = pino;
}

int Componente::getPino() {
  return _pino;
}

void Componente::setNome(String nome) {
  _nome = nome;
}

String Componente::getNome() {
  return _nome;
}

void Componente::setDescricao(String descricao) {
  _descricao = descricao;
}

String Componente::getDescricao() {
  return _descricao;
}

void Componente::setTipo(int tipo) {
  _tipo = tipo;
}

int Componente::getTipo() {
  return _tipo;
}
