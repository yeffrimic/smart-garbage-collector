/*
   Colabroacion sistema de compost,

  ingenieria del envio de datos hacia broker.mqtt-dashboard.com
  utilizando el procolo de mqtt

  Yeffri J. Salazar
  Xibalba Hackerspace, Julio 2019


*/


#include <ESP8266WiFi.h>
#include <PubSubClient.h>


#define wifiSSID "Xibalba Hackerspace"
#define wifiPassword "IOT1234567"
#define broker "broker.mqtt-dashboard.com"
#define subscribeTopic "/holi"
#define updateTopic "/msg"
#define brokerPort 1883
#define clientID "prueba" // clientID to connect to the broker

int publishFails = 0;
String inputString = "";
boolean stringComplete = false;
WiFiClient wemosClient;
PubSubClient mqttClient(wemosClient);


boolean publishString(String topic, String data) {
  char tempTopic[topic.length()];
  topic.toCharArray(tempTopic, topic.length() + 1);
  char tempData[data.length()];
  data.toCharArray(tempData, data.length() + 1);
  return mqttClient.publish(tempTopic, tempData);
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Recibiendo Información de  ");
  Serial.println(topic);
  Serial.println("payload = ");
  for (int i = 0; i < length; i++) {
    Serial.println((char) payload[i]);
  }
}

void mqttReconnect() {
  while (mqttClient.connected() == false) {
    Serial.print("conectando al Broker");
    Serial.println(broker);
    if (mqttClient.connect(clientID)) {
      Serial.println("Conectado al broker");
      mqttClient.subscribe(subscribeTopic);
    } else {
      Serial.println("falla numero: ");
      Serial.println(mqttClient.state());
      Serial.println("Intentando Reconexion en 5 segundos");
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

boolean wifiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  Serial.println("Conectado al Wifi");
  Serial.println("Direccion IP");
  Serial.println(WiFi.localIP());
  return true;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Conectando al WiFi");
  Serial.println(wifiSSID);
  WiFi.begin(wifiSSID, wifiPassword);
  while (wifiConnect() != true) {
    Serial.println(".");
    delay(500);
  }
  mqttClient.setServer(broker, brokerPort);
  mqttClient.setCallback(callback);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
}

void loop() {
  serialEvent();
  if (mqttClient.connected() == false) {
    mqttReconnect();
  } else {
    mqttClient.loop();
    if (stringComplete) {
      Serial.println(inputString);
      // clear the string:
      publishString(updateTopic, inputString);
      inputString = "";
      stringComplete = false;
    }
  }
  if (publishFails > 200) {
    //reset or do something check the wifi
  }
}
