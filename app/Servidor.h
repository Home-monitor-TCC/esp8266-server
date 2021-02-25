#include <ESP8266WebServer.h>
#include <Componente.h>
#include <Led.h>
#include <SensorTemperatura.h>

using std::list; 

ESP8266WebServer server(80);
std::list<Componente> componentes = {};


class Servidor {
  public:
    Servidor();
  
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
}

Servidor::Servidor(){
  server.begin();

  server.onNotFound(handleNotFound);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/componentes/adicionar", HTTP_POST,handleAdicionarComponente);
  server.on("/componentes/remover", HTTP_POST,handleRemoverComponente);
  server.on("/componentes/editar", HTTP_POST,handleEditarComponente);
  server.on("/componentes/led/acender", HTTP_POST, handleAcenderLed);
  server.on("/componentes/led/apagar", HTTP_POST, handleApagarLed);
  server.on("/componentes/sensores", HTTP_GET, handleSensores);
  server.on("/bd", HTTP_GET, handleBancoDeDados);
}

void handleRoot() {
  server.send(200, "text/plain", "Servidor Online");
}

void handleNotFound(){
  server.send(404, "text/html", "404: Not Found"); 
}

Servidor::handleAdicionarComponente(){
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
    } 
    
    else if(tipo == 2) {
      ComponenteSensorTemperatura *comp = new ComponenteSensorTemperatura(nome, descricao, pino, tipo);
      componentes.push_back(*comp);
    } 
    
    else {
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

Servidor::handleRemoverComponente(){
    String data = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    String nome = doc["nome"];
    String descricao = doc["descricao"];
    int pino = doc["pino"];

    std::list<Componente>::iterator it;
    for (it = componentes.begin(); it != componentes.end(); it++){
       if(it->getNome() == nome && it->getDescricao() == descricao && it->getPino() == pino){
          it = componentes.erase(it);
       }
    }

    DynamicJsonDocument docRes(1024);
    String resposta = "";
    
    docRes["nome"] = nome;
    docRes["descricao"] = descricao;
    docRes["pino"] = pino;

    serializeJson(docRes, resposta);

    server.send(200, "text/plain", resposta);  
}

Servidor::handleEditarComponente(){
    String data = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, data);

    String nome = doc["nome"];
    String descricao = doc["descricao"];
    int pino = doc["pino"];

    String nomeNovo = doc["nomeNovo"];
    String descricaoNovo = doc["descricaoNovo"];
    int pinoNovo = doc["pinoNovo"];
    
    std::list<Componente>::iterator it;
    for (it = componentes.begin(); it != componentes.end(); it++){
       if(it->getNome() == nome && it->getDescricao() == descricao && it->getPino() == pino){
          it->setNome(nomeNovo);
          it->setDescricao(descricaoNovo);
          it->setPino(pinoNovo);
       }
    }
    
    server.send(200, "text/plain");  
}

Servidor::handleAcenderLed(){
      String data = server.arg("plain");
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, data);
      
      int pino = doc["pino"];
      
      std::list<Componente>::iterator it;
      for (it = componentes.begin(); it != componentes.end(); it++){
         if(it->getTipo() == 1){
            it->setEstado(true)
         }
      }   
}

Servidor::handleApagarLed(){
     String data = server.arg("plain");
     DynamicJsonDocument doc(1024);
     deserializeJson(doc, data);
      
     int pino = doc["pino"];
      
     std::list<Componente>::iterator it;
     for (it = componentes.begin(); it != componentes.end(); it++){
        if(it->getTipo() == 1){
           it->setEstado(false)
        }
     }    
}

Servidor::handleSensores(){
    DynamicJsonDocument docRes(1024);
    String resposta = "";
    
    std::list<Componente>::iterator it;
       for (it = componentes.begin(); it != componentes.end(); it++){
          if(it->getTipo() == 2){
            docRes[std::distance(componentes.begin(), it)]["pino"] = nome;
            docRes[std::distance(componentes.begin(), it)]["dados"] = it->getTemperatura();
          }
       }
    }    

    
    
    docRes["nome"] = nome;
    docRes["descricao"] = descricao;
    docRes["pino"] = pino;

    serializeJson(docRes, resposta);

    server.send(200, "text/plain", resposta);  
}

Servidor::handleBancoDeDados(){
  
    DynamicJsonDocument docRes(1024);

    int aux = 0;

    std::list<Componente>::iterator it;


    for (it = componentes.begin(); it != componentes.end(); it++){
      docRes["data"][aux]["nome"] = it->getNome();
      docRes["data"][aux]["descricao"] = it->getDescricao();
      docRes["data"][aux]["pino"] = it->getPino();
    }

    String resposta = "";
    serializeJson(docRes, resposta);
    server.send(200, "text/plain", resposta);
}
