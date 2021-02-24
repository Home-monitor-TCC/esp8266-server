#pragma once

#define Server_H

#include <algorithm>
#include <iostream>
#include <list>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <exception>

using std::list; 

ESP8266WebServer server(80);
std::list<Componente> componentes = {};

void setup(){
  Serial.begin(9600);
  WiFi.enableInsecureWEP(true);

  String ssid = "";
  String pw = "";
  
  WiFi.begin(ssid, pw);
  
  delay(5000);
  
  Serial.println("Conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    delay(2000);
  }

  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
  
  server.begin();


  server.onNotFound(handleNotFound);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/componentes/adicionar", HTTP_POST,handleAdicionarComponente);
  server.on("/componentes/remover", HTTP_POST,handleAdicionarComponente);
  server.on("/componentes/editar", HTTP_POST,handleAdicionarComponente);
  server.on("/componentes/led/acender", HTTP_POST, handleAcenderLed);
  server.on("/componentes/led/apagar", HTTP_POST, handleApagarLed);
  server.on("/componentes/sensores", HTTP_GET, handleSensores);
  server.on("/bd", HTTP_GET, handleBancoDeDados);
}

void loop(){
  server.handleClient();
}
  
void handleRoot() {
  server.send(200, "text/plain", "Servidor Online");
}

void handleNotFound(){
  server.send(404, "text/html", "404: Not Found"); 
}

void handleAdicionarComponente(){
    String data = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    String nome = doc["nome"];
    String descricao = doc["descricao"];
    int tipo = doc["tipo"];
    int pino = doc["pino"];

    if(tipo == 1){
      ComponenteLED *comp = new ComponenteLED(nome, descricao, pino, tipo);
      
      componentes.push_back(*comp);
    } else if(tipo == 2) {
      ComponenteSensorTemperatura *comp = new ComponenteSensorTemperatura(nome, descricao, pino, tipo);
      componentes.push_back(*comp);
    } else {
        server.send(400, "text/plain", "400: Bad Request");
    }

    DynamicJsonDocument docRes(1024);
    String resposta = "";
    
    docRes["nome"] = nome;
    docRes["descricao"] = descricao;
    docRes["pino"] = pino;

    serializeJson(docRes, resposta);

    server.send(200, "text/plain", resposta);
}

void handleRemoverComponente(){
    
    String data = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);           
    std::list<Componente>::iterator it = componentes.begin();
    
    String nome = doc["nome"];
    String descricao = doc["descricao"];
    int tipo = doc["tipo"]; 
    int pino = doc["pino"];
    Componente *compAux, *temp;
    compAux = new Componente(nome, descricao, pino, tipo);     


    // int pinoAux = temp->getPino();
    for (std::list<Componente>::iterator it = componentes.begin(); it != componentes.end(); ++it){
      if(it->getPino() == compAux->getPino()){
        componentes.erase(it);
      }         
    }
    
    Componente temp = Componente(nome,descricao,pino);

    for (std::list<Componente>::iterator it = componentes.begin(); it != componentes.end(); ++it){
            if(*it == pinoRemoved){
              componentes.erase(it);
            } 
    
}

void handleEditarComponente(){
      String data = server.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, data);
      
      String nome = doc["nome"];
      String descricao = doc["descricao"];
      int tipo = doc["tipo"]; //LED = 1 && SENSOR = 2
      int pino = doc["pino"];

}

void handleAcenderLed(){
      String data = server.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, data);
      
      int pino = doc["pino"];
      
      pinMode(pino, OUTPUT);
      digitalWrite(pino, HIGH);   
}

void handleApagarLed(){
      String data = server.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, data);

      int pino = doc["pino"];

      pinMode(pino, OUTPUT);
      digitalWrite(pino, LOW);  
}

void handleSensores(){
  
}

void handleBancoDeDados(){
  
  DynamicJsonDocument docRes(1024);

  int aux = 0;

  std::list<Componente>::iterator it = componentes.begin();


 for (auto  i : componentes) {
   docRes["data"][aux]["nome"] = it->getNome();
   docRes["data"][aux]["descricao"] = it->getDescricao();
   docRes["data"][aux]["pino"] = it->getPino();
 }

 String resposta = "";
 serializeJson(docRes, resposta);
 server.send(200, "text/plain", resposta);
}


   