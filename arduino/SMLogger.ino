#include "MeterData.h"
#include "Mqtt.h"

typedef enum {
  WAIT_START,
  PROCESS_DATA,

} RECEIVE_STATE;

MQTT mqtt;

int bufferptr;

#define MAXLINELENGTH 1024 // longest normal line is 47 char (+3 for \r\n\0)
#define MAXBUFFERLEN  5000 // max buffer size
char telegram[MAXBUFFERLEN];

RECEIVE_STATE state = WAIT_START;

void setup() {
  Serial.begin(115200);
  Serial.setRxBufferSize(3000);
  mqtt.setup();
  state = WAIT_START;
}

void processBuffer() {
  int ptr = 0;

  MeterData meterData;

  while (telegram[ptr] != 0) {
    meterData.parseLine(telegram + ptr);
    ptr += strlen(telegram + ptr) + 1;
  }

  if (meterData.isValid()) {
    String json = meterData.toJSON();
    mqtt.publishJSON(json);
  }

}

void loop() {
  if (Serial.available()) {
    while (Serial.available()) {
      int len = Serial.readBytesUntil('\n', telegram + bufferptr, MAXLINELENGTH);
      telegram[len + bufferptr] = '\n';
      telegram[len + bufferptr + 1] = 0;

      switch (state) {
        case WAIT_START:
          if ( telegram[0] == '/' ) {
            state = PROCESS_DATA;
            bufferptr += len + 2;
          }
          break;
        case PROCESS_DATA:
          if ( telegram[bufferptr] == '!' ) {
            telegram[len + bufferptr + 2] = 0;
            processBuffer();
            state = WAIT_START;
            bufferptr = 0;
          } else {
            bufferptr += len + 2;
            if ( bufferptr >= MAXBUFFERLEN - MAXLINELENGTH ) {
              Serial.println("Invalid data, buffer overflow!");
              state = PROCESS_DATA;
              bufferptr = 0;
            }
          }
          break;
        default:
          break;
      }
    }
  }
  mqtt.loop();
}
