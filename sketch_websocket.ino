#include "M5Atom.h"
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "AR";
const char* password = "ararararar";
const String host = "172.16.201.101";
const int port = 3001;

const int camersIds[] = {4, 37, 38, 39};
int camersTally[] = {0, 0, 0, 0};

DynamicJsonDocument doc(1024);
WebSocketsClient webSocket;

void setup() {
	M5.begin(true, false, true);
  
	WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    M5.dis.fillpix(0xAA11D9);
    delay(500);
    M5.dis.fillpix(0x000000);
    delay(500);
  }
  Serial.println("Connected to WiFi.");
	webSocket.begin(host, port, "/web-socket");
	webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
}

void loop() {
	webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  String str = (char*)payload;
	switch(type) {
		case WStype_DISCONNECTED:
      M5.dis.fillpix(0xAA11D9);
      delay(2400);
      M5.dis.fillpix(0x000000);
      delay(100);
			Serial.println("Disconnected from Tally service!");
			break;

		case WStype_CONNECTED:
      M5.dis.fillpix(0x000000);
			Serial.println("Connected from Tally service!");
			break;

		case WStype_TEXT:
      String str = (char*)payload;

      deserializeJson(doc, str);
      int id = doc["id"];
      const char* signalName = doc["name"];
      const char* tallyState = doc["tallyState"];

      for (int i=0; i < (sizeof(camersIds) / sizeof(int)); i++){
        if(camersIds[i] == id){

          int newState;
          if(strcmp(tallyState, "EMPTY") == 0){
            newState = 0;
          }else if(strcmp(tallyState, "PRV") == 0){
            newState = 1;
          }else {
            newState = 2;
          }
          camersTally[i] = newState;
        }
      }

      Serial.print("New State is ");  
      switch(maxState()){  
        case 0:
          M5.dis.fillpix(0x000000);
          Serial.println("hide");
          break;
        case 1:
          M5.dis.fillpix(0x1AEB50);
          Serial.println("PRV");
          break;
        case 2:
          M5.dis.fillpix(0xE01307);
          Serial.println("PGM");
          break;
      }

		  break;
	}
}

int maxState(){
  int max = 0;
  for (int tally : camersTally) {
    if (tally >= max) {
      max = tally;
    }
  }
  return max;
}
