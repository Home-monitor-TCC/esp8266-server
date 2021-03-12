#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Componente.h"
#include "Led.h"
#include "SensorTemperatura.h"
#include "ESP8266HTTPClient.h"
#include <vector>

using std::vector;
ESP8266WebServer servidorEsp(80);
int listaPinosValidos[] = {16, 5, 4, 0, 2, 14, 12, 13, 15, 3, 1}; //Lista de pinos válidos para uso
vector<int> listaPinosSensor; // Lista de pinos dos sensores de temperatura
vector<String> listaIdSensor; // Lista de ids dos sensores de temperatura

class Servidor {
  public:
    Servidor();
    void handler();

  private:
    unsigned long currentMillis;
    unsigned long previousMillis = 0;
    unsigned long interval = 60000;
    String _reqUrl = ""; // URL do servidor Node
    String _macAddress; //MACAddress da placa ESP8266
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

Servidor::Servidor(String macAddress){
  _macAddress = macAddress;
  servidorEsp.onNotFound(std::bind(&Servidor::handleNotFound, this));
  servidorEsp.on("/", HTTP_GET, std::bind(&Servidor::handleRoot, this));
  servidorEsp.on("/componentes/adicionar", HTTP_POST, std::bind(&Servidor::handleAdicionarComponente, this));
  servidorEsp.on("/componentes/remover", HTTP_POST, std::bind(&Servidor::handleRemoverComponente, this));
  servidorEsp.on("/componentes/editar", HTTP_POST, std::bind(&Servidor::handleEditarComponente, this));
  servidorEsp.on("/componentes/led/acender", HTTP_POST, std::bind(&Servidor::handleAcenderLed, this));
  servidorEsp.on("/componentes/led/apagar", HTTP_POST, std::bind(&Servidor::handleApagarLed, this));
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

  if (nome.length() > 64) { //Limite tamanho do nome
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  if (descricao.length() > 256) { //Limite tamanho da descricao
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }

  for (int x: listaPinosValidos) {//Verifica a validade do pino escolhido
    if(pino == x){
      if(tipo != 1 && tipo != 2){
        servidorEsp.send(400, "text/plain", "400: Bad Request");
      }

      // inicio da comunicação com node

      HTTPClient client;
      client.begin(_reqUrl+"/component/create"); //especifica o destino da request
      client.addHeader("Content-Type","text/plain"); // especifica o tipo de conteudo do header

      DynamicJsonDocument docResHttp(1024);
      String msgHttp;

      docResHttp["macAddress"] = _macAddress;
      docResHttp["nome"] = nome;
      docResHttp["descricao"] = descricao;
      docResHttp["pino"] = pino;
      docResHttp["tipo"] = tipo;

      serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

      int httpCode = client.POST(msgHttp); //envia a request de adição e recebe o codigo de resposta
      String respostaHttp = client.getString(); //recebe a resposta ao request
      // Serial.println(httpCode); //imprime o codigo de resposta
      // Serial.println(respostaHttp); //imprime a resposta ao request

      client.end(); //encerra conexão

      // fim da comunicação com node

      DynamicJsonDocument docRes(1024);
      String resposta = "";

      docRes["nome"] = nome;
      docRes["descricao"] = descricao;
      docRes["pino"] = pino;

      serializeJson(docRes, resposta);  //serializa o conteudo de docRes  em uma string

      servidorEsp.send(200, "text/plain", resposta);  //envia mensagem de sucesso para o servidor

    }
  }
    servidorEsp.send(400, "text/plain", "400: Bad Request"); //Se nenhum pino tiver sido escolhido, informar erro
}

void Servidor::handleRemoverComponente() {
  String data = servidorEsp.arg("plain");
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);

  String id = doc["id"];
  String nome = doc["nome"];
  String descricao = doc["descricao"];
  int pino = doc["pino"];

  // inicio da comunicação com node

  HTTPClient client;
  client.begin(_reqUrl+"/component/remove"); //especifica o destino da request
  client.addHeader("Content-Type","text/plain"); // especifica o tipo de conteudo do header

  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["id"] = id;
  docResHttp["macAddress"] = _macAddress;
  docResHttp["nome"] = nome;
  docResHttp["descricao"] = descricao;
  docResHttp["pino"] = pino;
  docResHttp["tipo"] = tipo;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.DELETE(msgHttp); //envia a request de deleção e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request
  
  //remover abaixo na entrega final - PODE ESTOURAR A RAM
  Serial.println(httpCode); //imprime o codigo de resposta
  Serial.println(respostaHttp); //imprime a resposta ao request
  //remover até aqui

  client.end(); //encerra conexão

  // fim da comunicação com node
  
  DynamicJsonDocument docRes(1024);
  String resposta = "";

  if(httpCode>=200 && httpCode<=299){  //caso o codigo do node seja de sucesso, envia uma mensagem de resposta para o servidor
    docRes["nome"] = nome;
    docRes["descricao"] = descricao;
    docRes["pino"] = pino;
    serializeJson(docRes, resposta);
    servidorEsp.send(200, "text/plain", resposta);  // envia o objeto deletado para o servidor em JSON
  }
  else {
    docRes["erro"] = "Componente não foi encontrado na tentativa de remover."; //Monta o docRes apenas com a mensagem de erro
    serializeJson(docRes, resposta);
    servidorEsp.send(400, "text/plain", resposta);
  }
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
  if (nomeNovo.length() > 64) { //Limite tamanho do nome
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  if (descricaoNovo.length() > 256) { //Limite tamanho da descricao
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }
  for (int x: listaPinosValidos) {//Verifica a validade do pino escolhido
    if(pinoNovo = x){
      validapino = true;
    }
  }
  if(validapino){
    // inicio da comunicação com node

    HTTPClient client;
    client.begin(_reqUrl+"/component/edit"); //especifica o destino da request
    client.addHeader("Content-Type","text/plain"); // especifica o tipo de conteudo do header

    DynamicJsonDocument docResHttp(1024);
    String msgHttp;

    docResHttp["id"] = id;
    docResHttp["macAddress"] = _macAddress;
    docResHttp["nome"] = nome;
    docResHttp["descricao"] = descricao;

    serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

    int httpCode = client.PATCH(msgHttp); //envia a request de deleção e recebe o codigo de resposta
    String respostaHttp = client.getString(); //recebe a resposta ao request
    
    //remover abaixo na entrega final - PODE ESTOURAR A RAM
    Serial.println(httpCode); //imprime o codigo de resposta
    Serial.println(respostaHttp); //imprime a resposta ao request
    //remover até aqui

    client.end(); //encerra conexão

    // fim da comunicação com node
  }
  else{
    servidorEsp.send(400, "text/plain", "400: Bad Request");
  }

  if(httpCode>=200 && httpCode<=299){ //se o Node retornou sucesso, envia sucesso para o servidor
    DynamicJsonDocument docRes(1024);
    String resposta = "";
    docRes["id"] = id;
    docRes["nome"] = nome;
    docRes["descricao"] = descricao;
    servidorEsp.send(200, "text/plain");
  }
  else{
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

  for (int x : listaPinosValidos) { //Verifica a validade do pino escolhido
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

  for (int x : listaPinosValidos) { //Verifica a validade do pino escolhido
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

void Servidor::handleBancoDeDados() {

  // inicio da comunicação com node

  HTTPClient client;
  client.begin(_reqUrl+"/dbdump/read"); //especifica o destino da request
  client.addHeader("Content-Type","text/plain"); // especifica o tipo de conteudo do header

  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["macAddress"] = _macAddress;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.GET(msgHttp); //envia a request e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request
  
  //remover abaixo na entrega final - PODE ESTOURAR A RAM
  Serial.println("Codigo http e payload do client.getString()");
  Serial.println(httpCode); //imprime o codigo de resposta
  Serial.println(respostaHttp); //imprime a resposta ao request
  //remover até aqui

  client.end(); //encerra conexão

  // fim da comunicação com node

  DynamicJsonDocument docRes(1024);

  deserializeJson(docRes,respostaHttp);

  for (auto i : docRes["leds"]) {
    if(i["state"] == true) {
      digitalWrite(i["pino"], HIGH);
    }
    else {
      digitalWrite(i["pino"], LOW);
    }
  }

  for (auto j : docRes["temperatureSensors"]) {
    listaPinosSensor.push_back(j["pino"]);
    listaIdSensor.push_back(j["id"]);
  }

  servidorEsp.send(200, "text/plain", respostaHttp);
}

void Servidor::handler(){
  servidorEsp.handleClient();
}

/*
void Servidor::handleSensores(){
  currentMillis = millis(); // armazena quanto tempo se passou desde que a placa iniciou
  if(currentMillis - previousMillis >= interval){ //caso tenham-se passado (interval) segundos, executa novamente
    previousMillis = currentMillis;

  }
}
*/