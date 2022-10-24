#include "M5Atom.h"
#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "AR";
const char* password = "ararararar";

const String host = "172.16.201.101";
const int port = 3001;
const char* cameraId = "4,37,38,39";

WebSocketsClient webSocket;

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
      webSocket.sendTXT(cameraId);
			break;
		case WStype_TEXT:
			Serial.println(str);
      switch(str.toInt()){  
        case 0:
          M5.dis.fillpix(0x000000);
          Serial.println("hide");
          break;
        case 1:
          M5.dis.fillpix(0xE01307);
          Serial.println("PGM");
          break;
        case 2:
          M5.dis.fillpix(0x1AEB50);
          Serial.println("PRV");
          break;
        default:
          M5.dis.fillpix(0xE01307);
          Serial.println("PGM AND PRV");
          break;
      }
		  break;
		default:
			break;
	}

}

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
