

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <Wire.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>

boolean connectWifi();

//on/off callbacks
bool ArCondicionadoOn();
bool ArCondicionadoOff();



// Configs de Wifi
const char* ssid = "wiwi_usr";
const char* password = "wiwi_pass";

boolean wifiConnected = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *arCondicionado = NULL;



// Setando pinos de relay
#define relayOne D4
bool isArCondicionadoOn = false;

IRsend irsend(relayOne);
 
void setup()
{
 

  Serial.begin(115200);

  // Iniciar wifi
  wifiConnected = connectWifi();
  Serial.print("WiFi Connected");

  if (wifiConnected) {
    upnpBroadcastResponder.beginUdpMulticast();

    // Definição de Switches. Max 14
    // Format: Alexa invocation, Número da porta, on callback, off callback  
    arCondicionado = new Switch("Ar-Condicionado", 3611, ArCondicionadoOn, ArCondicionadoOff);   

    Serial.println("Adding switches upnp broadcast responder");
    upnpBroadcastResponder.addDevice(*arCondicionado);
 

    //Setando relay como output
    pinMode(relayOne, OUTPUT);

  }
}

void loop()
{
  if (wifiConnected) {
    upnpBroadcastResponder.serverLoop();
    arCondicionado->serverLoop();
  }
}

// AC ON
bool ArCondicionadoOn() {
  Serial.print("Ligando Ar-Condicionado ...");
  isArCondicionadoOn = true;
  irsend.sendLG2(0x880095E, 28); //AC HEX code com o número de bytes para enviar que liga o ar.
   return isArCondicionadoOn;
}

// AC OFF
bool ArCondicionadoOff() {
  Serial.print("Desligando Ar-Condicionado ...");
  isArCondicionadoOn = false;
  irsend.sendLG2(0x88C0051, 28);//AC HEX code com o número de bytes para enviar que desliga o ar.
  return isArCondicionadoOn;
 
}

// Conexão com wifi
  boolean connectWifi() {
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 10) {
      state = false;
      break;
    }
    i++;
  }

  if (state) {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }

  return state;
}
