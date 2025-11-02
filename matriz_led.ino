// HARDWARE ESP32
#if defined(ESP32)
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti; 
#endif

// MATRIZ LED
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN   18 
#define DATA_PIN  23 
#define CS_PIN    15 

// MQTT
#include <MQTT.h>
#include "data.h"

WiFiClient net; 
MQTTClient clienteMQTT; 

unsigned long tiempoAnterior = 0; 

// CUANDO RECIBA MENSAJE MQTT
void MensajeMQTT(String &topic, String &payload) {
  // EL TOPIC
  Serial.print("Topic: "); 
  Serial.print(topic); 
  // EL MENSAJE
  Serial.print(" Mensaje: "); 
  Serial.println(payload); 
}

//CONECTAR 
// Ver si el Wifi esta conectado
// Ver si el MQTT esta conectado
void conectar() {
  Serial.print("Conectando con Wifi..."); 
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print("."); 
    delay(100); 
  }

  Serial.print("Se conecto a Wifi"); 
  Serial.print(WiFi.SSID()); 
  Serial.print(" -  "); 
  Serial.println(WiFi.localIP()); 

  Serial.print("Conectar a MQTT***"); 
   // Si el MQTT tiene contraseña
   // while (!client.connect(NombreESP, "public", "public"))
  while(!clienteMQTT.connect(NombreESP)) {
    Serial.print("*"); 
    delay(1000); 
  }
  Serial.println("Conectado a MQTT"); 
  clienteMQTT.subscribe("Entrada/01"); 
}

//SETUP MQTT
void setup() {
  Serial.begin(115200);

  wifiMulti.addAP(ssid_1, password_1); 
  
  WiFi.mode(WIFI_STA); 
  clienteMQTT.begin(BrokerMQTT, 1883, net); 
  // Cuando llegue el mensaje llama a esta función
  clienteMQTT.onMessage(MensajeMQTT);

  conectar();
}

// Checa si esta conectado y si no llama a conectar 
void loop() {
  clienteMQTT.loop();
  if(!clienteMQTT.connected()){
    conectar();
  }
}
