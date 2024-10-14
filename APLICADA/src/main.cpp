#include <WiFi.h>
#include <SPIFFS.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include "FS.h"
#include <LiquidCrystal_I2C.h> //lcd



const int ledPin = 2;
const int trigPin = 13;
const int echoPin = 12;
const double soundSpeed = 0.034;
const int dataTxTimeInterval = 500; //ms
long dist;

//Inicio de servidor asincrono, WebSocket y LiquidCrystal
AsyncWebServer server(80);
WebSocketsServer WebSockets(81);
LiquidCrystal_I2C lcd(0x27,16,2);

//PROTOTIPADO
void notFound(AsyncWebServerRequest *request);
void WebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void listSPIFFSFiles(); // Función para listar archivos en SPIFFS
long getDistance();
void pantalla(long distancia, String msj);



//Inicio del setUp
void setup() {
  Serial.begin(115200);
  Serial.println("============================Inicio============================\n");
  

  //CONFIGURACIONES DE PINES (LED, SENSOR ULTRASONICO Y PANTALLA)
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //CONFIGURACION DE LA PANTALLA
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);


  WiFi.softAP("ProbandoAP", "");
  Serial.println("\nsoftAP");
  Serial.println(WiFi.softAPIP());

  if(!SPIFFS.begin(true)) {
    Serial.println("ERROR al montar SPIFFS");
    return;
  }

  Serial.println("============================SEGUNDAPARTE============================\n");
  
  // Llamada a la función para listar archivos en SPIFFS
  listSPIFFSFiles(); 

  // Ajuste del server.on para enviar el archivo HTML
  // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
  //     File file = SPIFFS.open("/index.html");
  //     if (!file) {
  //         Serial.println("Archivo no encontrado: /index.html");
  //         request->send(404, "text/plain", "Archivo no encontrado");
  //         return;
  //     }
  //     request->send(file, "/index.html", "text/html");
  // });

//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//     if (SPIFFS.exists("/index.html")) {
//         File file = SPIFFS.open("/index.html", "r");
//         if (file) {
//             request->send(200, file, "text/html");
//         } else {
//             request->send(500, "text/plain", "Error abriendo el archivo");
//         }
//     } else {
//         request->send(404, "text/plain", "Archivo no encontrado");
//     }
// });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");  // Enviar el archivo directamente con el tipo MIME 'text/html'
  });



  server.onNotFound(notFound);
  server.begin();

  WebSockets.begin();
  WebSockets.onEvent(WebSocketEvent);


}

void loop(void) {
  WebSockets.loop();
  
  static uint32_t prevMillis = 0;

  dist = getDistance();

  if(millis() - prevMillis >= dataTxTimeInterval){
    
    prevMillis = millis();

    String data = "{\"distance\": " + String(dist) + "}";

    WebSockets.broadcastTXT(data);

    Serial.println(data);

  }

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
        Serial.printf(" Mensaje recibido: %s\n", msg);
        pantalla(dist, "LLenando tanque");
      }

      if (msg.equalsIgnoreCase("ledoff")) {
        digitalWrite(ledPin, LOW);
        Serial.printf(" Mensaje recibido: %s\n", msg);
        pantalla(dist, "Llenado Detenido");

      }
  }
}

//Funcion para obtener la lectura del ultrasonico

long getDistance(){

  long duration, distance;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * soundSpeed/2;

  distance = distance > 350 ? 200 : distance;



  return distance;
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


void pantalla(long distancia, String msj){
  if (distancia > 5) {

    // digitalWrite(ledPin, LOW);  // Enciende el LED 1 cuando la distancia es mayor a 150 cm
    lcd.print(distancia);
    lcd.print(" cm");
    lcd.setCursor(0,1); // column#4 and Row #1 
    lcd.print(msj);
    delay(500);
    lcd.clear();



  } 
  else {

    //digitalWrite(ledPin, HIGH);   // Apaga el LED 1
    
    lcd.print(distancia);
    lcd.print(" cm");
    lcd.setCursor(1,1); // column#4 and Row #1 
    lcd.print("Detener Llenado");
    
    delay(500);
    lcd.clear();



  }
}

