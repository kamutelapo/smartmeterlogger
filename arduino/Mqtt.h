#ifndef MQTT_H
#define MQTT_H

class WiFiClient;

class MQTT {
  private:
    bool     connecting;
    bool     mqttConnected;
    bool     reconnectDelay;
    uint32_t reconnectStart;
    void     reconnect();

  public:
    void     setup();
    void     loop();
    void     publishJSON(const String &);
    void     publishDebug(const String &);
};

#endif /* MQTT_H */
