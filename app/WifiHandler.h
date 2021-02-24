#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define WifiHandler_H

ESP8266WebServer server(80);

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

    void getConnectionStatus(){
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

    void setSsid(char ssid){
        _ssid = ssid;
    }
    
    void setPw(char pw){
        _pw = pw;
    }

    char getSsid(){
        return _ssid; 
    }
    
    char getPw(){
        return _pw; 
    }