#pragma once

#include <ESP8266WiFi.h>

#define WifiHandler_H

class WifiHandler {
  public:
    WifiHandler(String ssid, String pw);
    String getIp();
    String getConnectionStatus();
    void setSsid(String ssid);
    String getSsid();
    void setPw(String pw);
    String getPw();

  private:
    String _ssid = "";
    String _pw = "";
};

WifiHandler::WifiHandler(String ssid, String pw) {
  setSsid(ssid);
  setPw(pw);
  WiFi.begin(ssid, pw);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    delay(2000);
  }

  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

String WifiHandler::getMACAddress(){
  return WiFi.macAddress();
}

void WifiHandler::setSsid(String ssid) {
  _ssid = ssid;
}

void WifiHandler::setPw(String pw) {
  _pw = pw;
}

String WifiHandler::getSsid() {
  return _ssid;
}

String WifiHandler::getPw() {
  return _pw;
}
