#include <WiFi.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"


// #include "soc/soc.h"
// #include "soc/rtc_cntl_reg.h"

const uint8_t ledPin = 2;

AsyncWebServer server(80);
WebSocketsServer WebSockets(81);

void notFound(AsyncWebServerRequest *request);
void WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);




void setup() {
  // put your setup code here, to run once:
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);

  Serial.println("============================Inicio============================\n");
  
  pinMode(ledPin, OUTPUT);
  

  WiFi.softAP("ProbandoAP", "");
  Serial.println("\nsoftAP");
  Serial.println(WiFi.softAPIP());

  if(!SPIFFS.begin(true))
  {
    Serial.println("ERROR al montar SPIFFS");
    return;
  }

  //  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //         request->send(SPIFFS, "/index.html", "text/html");});

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    File file = SPIFFS.open("/index.html");
    if (!file) {
        request->send(404, "text/plain", "File Not Found");
        return;
    }
    request->send(file, "/index.html", "text/html");
  });

  
  server.onNotFound(notFound);
  server.begin();

  WebSockets.begin();
  WebSockets.onEvent(WebSocketEvent);


  
}

void loop(void)
{
  WebSockets.loop();
}

void notFound(AsyncWebServerRequest *request)
{
  request ->send(404, "text/plain", "404");
}

void WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

  switch(type)
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED:
    {
        IPAddress ip = WebSockets.remoteIP(num);
        Serial.printf("[%u] conectado en %d.%d.%d.%d url: %s \n", num, ip[0], ip[1], ip[2], ip[3], ip);

        WebSockets.sendTXT(num, "Conectado en servidor: ");
    }

    break;

    case WStype_TEXT:
      Serial.printf("[%u] Mensaje recibido: %s\n", num, payload);
      String msg = String((char *)(payload));

      if(msg.equalsIgnoreCase("ledon"))
      {
        digitalWrite(ledPin, HIGH);

      }

      if(msg.equalsIgnoreCase("ledoff"))
      {
        digitalWrite(ledPin, LOW);
      }
  }
}



