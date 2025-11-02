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

// HARDWARE SPI
MD_Parola P(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// MQTT
#include <MQTT.h>
#include "data.h"

WiFiClient net; 
MQTTClient clienteMQTT; 

unsigned long tiempoAnterior = 0; 

// CUANDO RECIBA MENSAJE MQTT
void MensajeMQTT(String &topic, String &payload) {
  payload.trim(); 
  int n = payload.toInt(); 
  Serial.print("Topic: "); Serial.print(topic); 
  Serial.print(" Mensaje: "); Serial.print(payload); 
  setMessage(n); 
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

// MANDAR EL MENSAJE 
void setMessage(int n) {
  static char msg[40]; 
  if (n <= 0) snprintf(msg, sizeof(msg), "0 LUGARES"); 
  else if (n == 1) snprintf(msg, sizeof(msg), "1 LUGAR DISP."); 
  else snprintf(msg, sizeof(msg), "%d LUGARES DISP.", n); 

  P.displayClear(); 
  P.displayReset(); 
  P.displayText(
    msg, 
    PA_CENTER, 
    50, 
    0, 
    PA_SCROLL_LEFT, 
    PA_SCROLL_LEFT
  );
}

//SETUP MQTT
void setup() {
  Serial.begin(115200);

  // Inicializar Matriz
  P.begin(); 
  P.setIntensity(2);
  setMessage(0); // Si jala la matriz o no

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
  if (P.displayAnimate()) {
    P.displayReset(); 
  }
}
