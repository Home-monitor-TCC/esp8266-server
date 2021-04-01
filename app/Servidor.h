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

class Servidor {
  public:
    Servidor(String macAddress);
    void handler();

  private:
    unsigned long currentMillis;
    unsigned long previousMillis = 0;
    unsigned long interval = 60000;
    String _reqUrl = "http://192.168.0.122:2233"; // URL do servidor Node
    String _macAddress; //MACAddress da placa ESP8266
    boolean _setupProcess = true;
    void handleRoot();
    void handleNotFound();
    void handleAdicionarComponente();
    void handleRemoverComponente();
    void handleEditarComponente();
    void handleAcenderLed();
    void handleApagarLed();
    void handleSensores();
    void handleBancoDeDados();
    void handlePinosLivres();
    void setupHandler();
};

Servidor::Servidor(String macAddress) {
  _macAddress = macAddress;
  servidorEsp.onNotFound(std::bind(&Servidor::handleNotFound, this));
  servidorEsp.on("/", HTTP_GET, std::bind(&Servidor::handleRoot, this));
  servidorEsp.on("/componentes/adicionar", HTTP_POST, std::bind(&Servidor::handleAdicionarComponente, this));
  servidorEsp.on("/componentes/remover", HTTP_POST, std::bind(&Servidor::handleRemoverComponente, this));
  servidorEsp.on("/componentes/editar", HTTP_POST, std::bind(&Servidor::handleEditarComponente, this));
  servidorEsp.on("/componentes/led/acender", HTTP_POST, std::bind(&Servidor::handleAcenderLed, this));
  servidorEsp.on("/componentes/led/apagar", HTTP_POST, std::bind(&Servidor::handleApagarLed, this));
  servidorEsp.on("/componentes/pinos", HTTP_GET, std::bind(&Servidor::handlePinosLivres, this));
  servidorEsp.on("/db", HTTP_GET, std::bind(&Servidor::handleBancoDeDados, this));
  servidorEsp.begin();

}

void Servidor::handleRoot() {
  servidorEsp.send(200, "text/plain", "Servidor Online");
}

void Servidor::handleNotFound() {
  servidorEsp.send(404, "text/html", "404: Not Found");
}

void Servidor::handleAdicionarComponente() {
  DynamicJsonDocument doc(1024);
  DynamicJsonDocument docRes(1024);

  String data = servidorEsp.arg("plain");
 
  deserializeJson(doc, data);

  String nome = doc["name"];
  String descricao = doc["description"];
  int tipo = doc["type"];
  int pino = doc["pin"];

  for (int x : listaPinosValidos) { //Verifica a validade do pino escolhido
    if (pino == x) {
      if (tipo != 1 && tipo != 2) {
        servidorEsp.send(400, "text/plain", "Tipo Invalido");
      }

      // inicio da comunicação com node

      HTTPClient client;
      client.begin(_reqUrl + "/component/create"); //especifica o destino da request
      client.addHeader("Content-Type", "application/json");

      DynamicJsonDocument docResHttp(1024);
      String msgHttp;

      docResHttp["board_mac_address"] = _macAddress;
      docResHttp["name"] = nome;
      docResHttp["description"] = descricao;
      docResHttp["pin"] = pino;
      docResHttp["type"] = tipo;

      serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

      int httpCode = client.POST(msgHttp); //envia a request de adição e recebe o codigo de resposta
      String respostaHttp = client.getString(); //recebe a resposta ao request

      client.end(); //encerra conexão

      if (httpCode >= 200 && httpCode <= 299) {
        if(tipo == 1){
          pinMode(pino, OUTPUT);
        }
        else if(tipo == 2){
          listaPinosSensor.push_back(pino);  
        }
        servidorEsp.send(200, "text/plain", respostaHttp);  //envia mensagem de sucesso para o servidor
      }
      else {
        servidorEsp.send(400, "text/plain", respostaHttp);  //envia mensagem de sucesso para o servidor
      }
    }
  }
}

void Servidor::handleRemoverComponente() {
  DynamicJsonDocument doc(1024);

  String data = servidorEsp.arg("plain");
  boolean flag = false;

  deserializeJson(doc, data);

  String id = doc["id"];
  String nome = doc["name"];
  String descricao = doc["description"];
  String tipo = doc["type"];
  int pino = doc["pin"];

  // inicio da comunicação com node

  HTTPClient client;
  client.begin(_reqUrl + "/component/remove"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");

  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["id"] = id;
  docResHttp["board_mac_address"] = _macAddress;
  docResHttp["name"] = nome;
  docResHttp["description"] = descricao;
  docResHttp["pin"] = pino;
  docResHttp["type"] = tipo;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("DELETE", msgHttp); //envia a request de deleção e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end(); //encerra conexão

  // fim da comunicação com node

  if (httpCode == 200) { //caso o codigo do node seja de sucesso, envia uma mensagem de resposta para o servidor

    std::vector<int>::iterator it;
    for (it = listaPinosSensor.begin(); it != listaPinosSensor.end(); it++) { //percorre a lista de componentes
      if (*it == pino) { //encontra o pino especificado
        it = listaPinosSensor.erase(it);
        flag = true;
      }
    }
    if(!flag){
      digitalWrite(pino, LOW);
    }
    
    servidorEsp.send(200, "text/plain", respostaHttp);  // envia o objeto deletado para o servidor em JSON
  }
  else {
    servidorEsp.send(400, "text/plain", respostaHttp);
  }
}

void Servidor::handleEditarComponente() {
  DynamicJsonDocument doc(1024);
  DynamicJsonDocument docRes(1024);

  String data = servidorEsp.arg("plain");

  deserializeJson(doc, data);

  String nome = doc["name"];
  String descricao = doc["description"];
  String id = doc["id"];

  // inicio da comunicação com node

  HTTPClient client;
  client.begin(_reqUrl + "/component/edit"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");

  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["id"] = id;
  docResHttp["mac_address"] = _macAddress;
  docResHttp["name"] = nome;
  docResHttp["description"] = descricao;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("PATCH", msgHttp); //envia a request de deleção e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end(); //encerra conexão

  // fim da comunicação com node

  if (httpCode >= 200 && httpCode <= 299) { //se o Node retornou sucesso, envia sucesso para o servidor
    servidorEsp.send(200, "text/plain", respostaHttp);
  }
  else {
    servidorEsp.send(400, "text/plain", respostaHttp);
  }
}

void Servidor::handleAcenderLed() {
  DynamicJsonDocument doc(1024);
  DynamicJsonDocument docRes(1024);

  String data = servidorEsp.arg("plain");
  String resposta = "";

  deserializeJson(doc, data);

  int pin = doc["pin"];

  HTTPClient client;
  client.begin(_reqUrl + "/led/on"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");


  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["mac_address"] = _macAddress;
  docResHttp["pin"] = pin;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("PATCH", msgHttp); //envia a request e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end(); //encerra conexão

  if (httpCode >= 200 && httpCode <= 299) { //se o Node retornou sucesso, envia sucesso para o servidor
    servidorEsp.send(200, "text/plain", respostaHttp);
    digitalWrite(pin, HIGH);
  } else {
    servidorEsp.send(400, "text/plain", respostaHttp);
  }
}

void Servidor::handleApagarLed() {
  DynamicJsonDocument doc(1024);
  DynamicJsonDocument docRes(1024);

  String data = servidorEsp.arg("plain");
  String resposta = "";

  deserializeJson(doc, data);

  String pin = doc["pin"];

  HTTPClient client;
  client.begin(_reqUrl + "/led/off"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");


  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["mac_address"] = _macAddress;
  docResHttp["pin"] = pin;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("PATCH", msgHttp); //envia a request e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end(); //encerra conexão

  if (httpCode >= 200 && httpCode <= 299) { //se o Node retornou sucesso, envia sucesso para o servidor
    servidorEsp.send(200, "text/plain", respostaHttp);
    digitalWrite(pin.toInt(), LOW);
  } else {
    servidorEsp.send(400, "text/plain", respostaHttp);
  }
}

void Servidor::handleBancoDeDados() {
  // inicio da comunicação com node

  HTTPClient client;
  client.begin(_reqUrl + "/db/read"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");

  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["mac_address"] = _macAddress;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("GET", msgHttp); //envia a request e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end(); //encerra conexão

  // fim da comunicação com node

  DynamicJsonDocument docRes(1024);

  deserializeJson(docRes, respostaHttp);

  for (int i = 0; i < docRes["leds"].size(); i++) {
    if (docRes["leds"][i]["state"] == true) {
      digitalWrite(i["pino"], HIGH);
    }
    else {
      digitalWrite(i["pino"], LOW);
    }
  }

  for (int j = 0; j < docRes["temperatureSensors"].size(); j++) {
    listaPinosSensor.push_back(docRes["temperatureSensors"][j]["pin"]);
  }


  if (httpCode == 200) {
    servidorEsp.send(200, "text/plain", respostaHttp);
  } else {
    servidorEsp.send(400, "text/plain", respostaHttp);
  }

}

void Servidor::handleSensores() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (listaPinosSensor.size() >= 1) {

      int i = 0;
      String msgHttp;

      DynamicJsonDocument doc(1024);
      HTTPClient client;
      client.begin(_reqUrl + "/temperature/create"); //especifica o destino da request
      client.addHeader("Content-Type", "application/json");

      doc["mac_address"] = _macAddress;

      for (i = 0; i < listaPinosSensor.size(); i++) {
        SensorTemperatura *sensor = new SensorTemperatura("", "", listaPinosSensor[i], 2);
        doc["temperatureDataArray"][i]["pin"] = listaPinosSensor[i];
        doc["temperatureDataArray"][i]["temperature"] = sensor->getTemperatura();
      }

      serializeJson(doc, msgHttp);
      client.sendRequest("POST", msgHttp);
      client.end();
    }
  }
}

void Servidor::setupHandler() {
  HTTPClient client;
  client.begin(_reqUrl + "/db/read"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");

  DynamicJsonDocument docResHttp(1024);
  String msgHttp;

  docResHttp["mac_address"] = _macAddress;

  serializeJson(docResHttp, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("GET", msgHttp); //envia a request e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end(); //encerra conexão

  // fim da comunicação com node

  DynamicJsonDocument docRes(1024);

  deserializeJson(docRes, respostaHttp);

  for (int i = 0; i < docRes["leds"].size(); i++) {
    int pinoTemp = docRes["leds"][i]["pin"];
    if (docRes["leds"][i]["state"] == true) {
      pinMode(pinoTemp, OUTPUT);
      digitalWrite(pinoTemp, HIGH);
    }
    else {
      pinMode(pinoTemp, OUTPUT);
      digitalWrite(pinoTemp, LOW);
    }
  }

  for (int j = 0; j < docRes["temperatureSensors"].size(); j++) {
    int pinoTemp = docRes["temperatureSensors"][j]["pin"];
    pinMode(pinoTemp, INPUT);
    listaPinosSensor.push_back(docRes["temperatureSensors"][j]["pin"]);
  }
  _setupProcess = false;
}

void Servidor::handlePinosLivres(){ 
  HTTPClient client;
  client.begin(_reqUrl + "/board/pins"); //especifica o destino da request
  client.addHeader("Content-Type", "application/json");

  DynamicJsonDocument doc(1024);
  String msgHttp;

  doc["mac_address"] = _macAddress;

  serializeJson(doc, msgHttp); //serializa o conteudo de docResHttp em uma string

  int httpCode = client.sendRequest("GET", msgHttp); //envia a request e recebe o codigo de resposta
  String respostaHttp = client.getString(); //recebe a resposta ao request

  client.end();

  servidorEsp.send(200, "text/plain", respostaHttp);
}

void Servidor::handler() {
  if (_setupProcess) {
    setupHandler();
  }
  
  servidorEsp.handleClient();
  handleSensores();
}
