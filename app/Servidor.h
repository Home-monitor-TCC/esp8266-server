#include <ESP8266WebServer.h>
#include<ArduinoJson.h>
#include <list>
#include "Componente.h"
#include "Led.h"
#include "SensorTemperatura.h"

using std::list;
ESP8266WebServer servidorEsp(80);

std::list<Componente> componentes = {};
int listaGPIO[] = {16, 5, 4, 0, 2, 14, 12, 13, 15, 3, 1};

class Servidor {
  public:
    Servidor();
    void handler();

  private:  
    void handleRoot();
    void handleNotFound();
    void handleAdicionarComponente();
    void handleRemoverComponente();
    void handleEditarComponente();
    void handleAcenderLed();
    void handleApagarLed();
    void handleSensores();
    void handleBancoDeDados();
};

Servidor::Servidor(){
  servidorEsp.onNotFound(std::bind(&Servidor::handleNotFound, this));
  servidorEsp.on("/", HTTP_GET, std::bind(&Servidor::handleRoot, this));
  servidorEsp.on("/componentes/adicionar", HTTP_POST, std::bind(&Servidor::handleAdicionarComponente, this));
  servidorEsp.on("/componentes/remover", HTTP_POST, std::bind(&Servidor::handleRemoverComponente, this));
  servidorEsp.on("/componentes/editar", HTTP_POST, std::bind(&Servidor::handleEditarComponente, this));
  servidorEsp.on("/componentes/led/acender", HTTP_POST, std::bind(&Servidor::handleAcenderLed, this));
  servidorEsp.on("/componentes/led/apagar", HTTP_POST, std::bind(&Servidor::handleApagarLed, this));
  servidorEsp.on("/componentes/sensores", HTTP_GET, std::bind(&Servidor::handleSensores, this));
  servidorEsp.on("/bd", HTTP_GET, std::bind(&Servidor::handleBancoDeDados, this));
  servidorEsp.begin();
}

void Servidor::handleRoot() {
  Serial.print("a");
  servidorEsp.send(200, "text/plain", "Servidor Online");
}

void Servidor::handleNotFound() {
  servidorEsp.send(404, "text/html", "404: Not Found");
}

void Servidor::handleAdicionarComponente() {
  String data = servidorEsp.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);
  DynamicJsonDocument docRes(1024);

  String nome = doc["nome"];
  String descricao = doc["descricao"];
  int tipo = doc["tipo"];
  int pino = doc["pino"];

  if (tipo == 1) {
    Led *comp = new Led(nome, descricao, pino, tipo);
    componentes.push_back(*comp);
  }

  else if (tipo == 2) {
    SensorTemperatura *comp = new SensorTemperatura(nome, descricao, pino, tipo);
    componentes.push_back(*comp);
  }

  else {
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  
  if (nome.length() > 64) { //Limite tamanho do nome
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  if (descricao.length() > 256) { //Limite tamanho da descricao
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }

  for (auto x : componentes) {
    if(pino == x.getPino()){ // Verifica se o pino já tá em uso.
      servidorEsp.send(400, "text/plain", "400: Bad Request"); 
    }
  }
  for (int x: listaGPIO) {//Verifica a validade do pino escolhido
    if(pino == x){
  
      DynamicJsonDocument docRes(1024);
      String resposta = "";

      docRes["nome"] = nome;
      docRes["descricao"] = descricao;
      docRes["pino"] = pino;

      serializeJson(docRes, resposta);

      servidorEsp.send(200, "text/plain", resposta);
    }
  }
    servidorEsp.send(400, "text/plain", "400: Bad Request"); //Se nenhum pino tiver sido escolhido, informar erro
}

void Servidor::handleRemoverComponente() {
  String data = servidorEsp.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);
  DynamicJsonDocument docRes(1024);

  String nome = doc["nome"];
  String descricao = doc["descricao"];
  int pino = doc["pino"];

  std::list<Componente>::iterator it;
  for (it = componentes.begin(); it != componentes.end(); it++) {
    if (it->getNome() == nome && it->getDescricao() == descricao && it->getPino() == pino) {
      it = componentes.erase(it);
    }
  }

  String resposta = "";

  docRes["nome"] = nome;
  docRes["descricao"] = descricao;
  docRes["pino"] = pino;

  serializeJson(docRes, resposta);

  servidorEsp.send(200, "text/plain", resposta);
}

void Servidor::handleEditarComponente() {
  String data = servidorEsp.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);
  DynamicJsonDocument docRes(1024);

  String nome = doc["nome"];
  String descricao = doc["descricao"];
  int pino = doc["pino"];

  String nomeNovo = doc["nomeNovo"];
  String descricaoNovo = doc["descricaoNovo"];
  int pinoNovo = doc["pinoNovo"];
  bool validapino = false;
  bool validaedit = false;
  if (nomeNovo.length() > 64) { //Limite tamanho do nome
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  if (descricaoNovo.length() > 256) { //Limite tamanho da descricao
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  for (auto x : componentes) {
    if(pinoNovo == x.getPino() && pinoNovo != pino){ // Verifica se o pino já tá em uso ou simplesmente sendo mantido
      servidorEsp.send(400, "text/plain", "400: Bad Request"); 
    }
  }
  for (int x: listaGPIO) {//Verifica a validade do pino escolhido
    if(pinoNovo = x){
      validapino = true;
    }
  }
  if(validapino){// Só adiciona o 
  for (auto i : componentes) {
      if (i.getNome() == nome && i.getDescricao() == descricao && i.getPino() == pino) { 
        i.setNome(nomeNovo);
        i.setDescricao(descricaoNovo);
        i.setPino(pinoNovo);
        validaedit = true;
      }
    }
  }else{
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  
  if(validaedit){
    servidorEsp.send(200, "text/plain");
  }else{
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
}
void Servidor::handleAcenderLed() {

  String data = servidorEsp.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);
  DynamicJsonDocument docRes(1024);

  bool flag = false;
  String resposta = "";
  int pino = doc["pino"];
  int tipo = doc["tipo"];
    
  for (int x : listaGPIO) { //Verifica a validade do pino escolhido
    if(pino == x){
      flag = true;
      }
  }
  if(flag == false){
    docRes["erro"] = "Pino inválido.";
    serializeJson(docRes, resposta);
    servidorEsp.send(400, "text/plain", resposta); 
  }
  if(tipo == 1){ 
    for(auto i : componentes){   
        if( pino == i.getPino()){
          if(digitalRead(pino) == HIGH){
           docRes["erro"] = "Não é possível ligar um LED já aceso"; //Monta o docRes apenas com a mensagem de erro 
           serializeJson(docRes, resposta);
           servidorEsp.send(400, "text/plain", resposta);
          }
       }
    }
  }
  else{
    docRes["erro"] = "Não é possível acender um sensor";
    serializeJson(docRes, resposta);
    servidorEsp.send(400, "text/plain", resposta); 
  }
  
  for (auto i : componentes) {
    if (i.getTipo() == 1) {
        Led *pLed;
//        pLed = i;
//        pLed->setEstado(false);
    }
  }
}

void Servidor::handleApagarLed() {
 
  String data = servidorEsp.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);
  DynamicJsonDocument docRes(1024);
  bool flag = false;
  String resposta = "";
  int pino = doc["pino"];
  int tipo = doc["tipo"];
    
  for (int x : listaGPIO) { //Verifica a validade do pino escolhido
    if(pino == x){
      flag = true;
      }
  }
  if(flag == false){
    docRes["erro"] = "Pino inválido.";
    serializeJson(docRes, resposta);
    servidorEsp.send(400, "text/plain", resposta); 
  }
  if(tipo == 1){ 
    for(auto i : componentes){   
        if( pino == i.getPino()){
          if(digitalRead(pino) == LOW){
           docRes["erro"] = "Não é possível apagar um LED já apagado"; //Monta o docRes apenas com a mensagem de erro 
           serializeJson(docRes, resposta);
           servidorEsp.send(400, "text/plain", resposta);
          }
       }
    }
  }
  else{
    docRes["erro"] = "Não é possível apagar um sensor";
    serializeJson(docRes, resposta);
    servidorEsp.send(400, "text/plain", resposta); 
  }
  
  for (auto i : componentes) {
    if (i.getTipo() == 1) {
        Led *pLed;
        //pLed = i;
        //pLed->setEstado(false);
    }
  }
}

void Servidor::handleSensores() {
  int aux = 0;
  DynamicJsonDocument docRes(1024);
  String resposta = "";

  for (auto i : componentes) {
    if (i.getTipo() == 2) {
      docRes[aux]["pino"] = i.getNome();
     // docRes[aux]["dados"] = (SensorTemperatura)i.getTemperatura();
      aux++;
    }
  }

  serializeJson(docRes, resposta);

  servidorEsp.send(200, "text/plain", resposta);
}

void Servidor::handleBancoDeDados() {

  DynamicJsonDocument docRes(1024);

  int aux = 0;

  for (auto i : componentes) {
    docRes["data"][aux]["nome"] = i.getNome();
    docRes["data"][aux]["descricao"] = i.getDescricao();
    docRes["data"][aux]["pino"] = i.getPino();
    docRes["data"][aux]["tipo"] = i.getTipo();
    aux++;
  }
  String resposta = "";
  serializeJson(docRes, resposta);
  Serial.print(resposta);

  servidorEsp.send(200, "text/plain", resposta);
}

void Servidor::handler(){
    servidorEsp.handleClient();
}
