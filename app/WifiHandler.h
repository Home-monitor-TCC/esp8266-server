#pragma once

#include <ESP8266WiFi.h>

#define WifiHandler_H

class WifiHandler{
    public:
        WifiHandler(char ssid, char pw);
        void setSsid(char ssid);
        char getSsid();
        void setPw(char pw);
        char getPw();
        void getConnectionStatus();
    private:
        char* _ssid[] = "";
        char* _pw[] = "";
}

WifiHandler::WifiHandler(char ssid, char pw){
    setSsid(ssid);
    setPw(pw);
    WiFi.begin(ssid, pw);
}

void WifiHandler::getConnectionStatus(){
    String Wifistatus;
    Wifistatus = WiFi.status();
    String returnStatus = "";

    switch(status) {
        case "0":
            returnStatus = "Mudando o estado...";
            getConnectionStatus();
            return getConnectionStatus();
            break; 
            
        case "1"  :
            returnStatus = "Nome da rede não encontrado";
            return returnStatus; 
            break; 
            
        case "3"  :
            returnStatus = "Conectado";
            return returnStatus;
            break; 
            
        case "4"  :
            returnStatus = "Conexão falhou";
            return returnStatus;
            break; 
            
        case "6"  :
            returnStatus = "Senha incorreta";
            return returnStatus;
            break; 

        case "7"  :
            returnStatus = "Rede desconectada";
            return returnStatus;
            break; 
    }
}

void WifiHandler::setSsid(char ssid){
    _ssid = ssid;
}
    
void WifiHandler::setPw(char pw){
    _pw = pw;
}

char WifiHandler::getSsid(){
    return _ssid; 
}
    
char WifiHandler::getPw(){
    return _pw; 
}
