#include <WiFi.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"

const uint8_t ledPin = 2;

AsyncWebServer server(80);
WebSocketsServer WebSockets(81);

void notFound(AsyncWebServerRequest *request);
void WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void listSPIFFSFiles(); // Función para listar archivos en SPIFFS

void setup() {
  Serial.begin(115200);
  Serial.println("============================Inicio============================\n");
  
  pinMode(ledPin, OUTPUT);

  WiFi.softAP("ProbandoAP", "");
  Serial.println("\nsoftAP");
  Serial.println(WiFi.softAPIP());

  if(!SPIFFS.begin(true)) {
    Serial.println("ERROR al montar SPIFFS");
    return;
  }

  // Llamada a la función para listar archivos en SPIFFS
  listSPIFFSFiles(); 

  // Ajuste del server.on para enviar el archivo HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
      File file = SPIFFS.open("/index.html");
      if (!file) {
          Serial.println("Archivo no encontrado: /index.html");
          request->send(404, "text/plain", "Archivo no encontrado");
          return;
      }
      request->send(file, "/index.html", "text/html");
  });

  server.onNotFound(notFound);
  server.begin();

  WebSockets.begin();
  WebSockets.onEvent(WebSocketEvent);

  Serial.println(WiFi.softAPIP());

}

void loop(void) {
  WebSockets.loop();
}

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "404");
}

void WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch(type) {
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

      if (msg.equalsIgnoreCase("ledon")) {
        digitalWrite(ledPin, HIGH);
      }

      if (msg.equalsIgnoreCase("ledoff")) {
        digitalWrite(ledPin, LOW);
      }
  }
}

// Función para listar los archivos en SPIFFS
void listSPIFFSFiles() {
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    
    Serial.println("Archivos en SPIFFS:");
    
    while (file) {
        Serial.print("File: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
}
