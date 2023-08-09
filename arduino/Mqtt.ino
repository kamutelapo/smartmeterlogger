#include "Mqtt.h"

#include <ArduinoMqttClient.h>
#if defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_AVR_UNO_WIFI_REV2)
#include <WiFiNINA.h>
#elif defined(ARDUINO_SAMD_MKR1000)
#include <WiFi101.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_NICLA_VISION) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_GIGA)
#include <WiFi.h>
#endif

#include "arduino_secrets.h"

#define RECONNECT_TIMEOUT 5000

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = MQTT_BROKER_HOST;
int        port     = MQTT_BROKER_PORT;
const char topic[]  = MQTT_TOPIC;

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

void MQTT::setup() {
  connecting = true;
  reconnectDelay = false;
  mqttConnected = false;

  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  mqttClient.setUsernamePassword(SECRET_MQTT_USER, SECRET_MQTT_PASS);
  mqttClient.setTxPayloadSize(5500);
}

void MQTT::reconnect() {
  if ( mqttConnected ) {
    mqttClient.stop();
  }
  connecting = true;
  reconnectDelay = true;
  mqttConnected = false;
  reconnectStart = millis();

  WiFi.begin(ssid, pass);
}

void MQTT::loop() {
  if (reconnectDelay) {
    uint32_t elapsed = millis() - reconnectStart;
    if ( elapsed >= RECONNECT_TIMEOUT ) {
      reconnectDelay = false;
      WiFi.begin(ssid, pass);
    }
  } else if ( connecting ) {
    if (WiFi.status() == WL_CONNECTED) {
      connecting = false;
      Serial.println("Wifi connected successfully");

      Serial.print("Attempting to connect to the MQTT broker: ");
      Serial.println(broker);

      mqttClient.beginWill(String(topic) + "/LWT", true, 1);
      mqttClient.print("Offline");
      mqttClient.endWill();

      if (!mqttClient.connect(broker, port)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());
        reconnect();
      } else {
        Serial.println("MQTT connected successfully");

        mqttClient.beginMessage(String(topic) + "/LWT");
        mqttClient.print("Online");
        mqttClient.endMessage();

        mqttConnected = true;
      }
    } else if (WiFi.status() != WL_DISCONNECTED) {
      Serial.print("Error connecting to WiFi:");
      Serial.println(WiFi.status());
      reconnect();
    }
  } else {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wifi connection failed, reconnecting");
      reconnect();
    }
  }
  if ( mqttConnected && !mqttClient.connected()) {
    Serial.println("MQTT failure, reconnecting");
    reconnect();
  }
  mqttClient.poll();
}

void MQTT::publishJSON(const String &json) {
  if ( ! connecting ) {
    mqttClient.beginMessage(String(topic) + "/SENSOR");
    mqttClient.print(json);
    mqttClient.endMessage();
  }
}

void MQTT::publishDebug(const String &debug) {
  if ( mqttConnected ) {
    mqttClient.beginMessage(String(topic) + "/DEBUG");
    mqttClient.print(debug);
    mqttClient.endMessage();
  }
}
