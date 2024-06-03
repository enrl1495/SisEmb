#include <DHT11.h>
#include <LittleFS.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <WebSocketsServer.h>

//const char* ssid = "DESKTOP-7TLKNVR 2834";
//const char* password = "9@Y75c31";
const char* ssid = "AgroSmart";
const char* password = "12345678";


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const int rele_fuente = 2;
const int rele_ventilador_externo = 3;
const int rele_luz = 4;
const int builtin_led = 15;



AsyncWebServer server(80);

WebSocketsServer wsTemperatura(81);
WebSocketsServer wsHumedad(82);
WebSocketsServer wsGetTemp(83);
WebSocketsServer wsControlLuz(84);
WebSocketsServer wsSensorHumedadSuelo(85);


DHT11 dht11(12);
const int sensor_humedad_tierra = 7;


float set_temp;
bool sentinela_get_temp = false;
bool estadoLuz = false;

void setup() {
  // put your setup code here, to run once:
  
  /*Wire.begin(9, 10);
  display.begin();
  display.clearDisplay();
  display.display();*/


  
  delay(2000);

  WiFi.softAP(ssid, password);

  delay(1000);

  
  /*display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Punto de acceso: ");
  display.setCursor(0, 10);
  display.print("AgroSmart. Clave: ");
  display.setCursor(0, 20);
  display.print("12345678");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(WiFi.softAPIP());
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();*/

  pinMode(builtin_led, OUTPUT);
  pinMode(rele_fuente, OUTPUT);
  pinMode(rele_ventilador_externo, OUTPUT);
  pinMode(rele_luz, OUTPUT);
  pinMode(sensor_humedad_tierra, INPUT);
  digitalWrite(rele_fuente, LOW);
  digitalWrite(rele_ventilador_externo, LOW);
  digitalWrite(rele_luz, LOW);
  digitalWrite(builtin_led, LOW);
  analogReadResolution(10);

  
  LittleFS.begin();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/INV.html", "text/html");
  });

  server.serveStatic("/", LittleFS, "/");

  // Manejo de rutas no encontradas
  server.onNotFound([](AsyncWebServerRequest *request) {
    if (request->url() == "/") {
      // Servir un archivo específico si la ruta es "/"
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, "/INV.html", "text/html");
      if (!response) {
        request->send(404, "text/plain", "Error 404: Archivo no encontrado");
      } else {
        request->send(response);
      }
    } else {
      // Si el archivo no se encuentra, enviar una respuesta de error 404
      request->send(404, "text/plain", "Error 404: Archivo no encontrado");
    }
  });
  server.begin();

  wsTemperatura.onEvent(onWebSocketEventLeerTemperatura);
  wsTemperatura.begin();

  wsHumedad.onEvent(onWebSocketEventLeerHumedad);
  wsHumedad.begin();

  wsGetTemp.onEvent(onWebSocketEventGetTemp);
  wsGetTemp.begin();

  wsControlLuz.onEvent(onWebSocketEventControlLuz);
  wsControlLuz.begin();

  wsSensorHumedadSuelo.onEvent(onWebSocketEventSensorHumedadSuelo);
  wsSensorHumedadSuelo.begin();

  digitalWrite(builtin_led, HIGH);
  delay(1000);
  digitalWrite(builtin_led, LOW);
  
}

void loop()
{
  // put your main code here, to run repeatedly:
  wsTemperatura.loop();
  wsHumedad.loop();
  wsGetTemp.loop();
  wsControlLuz.loop();
  wsSensorHumedadSuelo.loop();
  if(sentinela_get_temp)
  {
    prenderClima();
  }
  
}

void onWebSocketEventLeerTemperatura(uint8_t num, WStype_t type, uint8_t *payload, size_t length) 
{
  if (type == WStype_CONNECTED) {
    // Cliente conectado, no hacer nada especial
  } else if (type == WStype_DISCONNECTED) {
    // Cliente desconectado, no hacer nada especial
  } else if (type == WStype_TEXT) {
    // Cliente envió un mensaje de texto, verificamos si es para solicitar la temperatura
    String message = String((char*)payload).substring(0, length);
    if (message == "getTemperature") {
      // El cliente solicita la temperatura, la enviamos
      enviarTemperatura();
    }
  }
}

void enviarTemperatura() 
{
  float temperature = dht11.readTemperature();
  /*display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(temperature);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();*/

  String temperatureStr = String(temperature) + " °C";
  wsTemperatura.broadcastTXT(temperatureStr);
}


void onWebSocketEventLeerHumedad(uint8_t num, WStype_t type, uint8_t *payload, size_t length) 
{
  if (type == WStype_CONNECTED) {
    // Cliente conectado, no hacer nada especial
  } else if (type == WStype_DISCONNECTED) {
    // Cliente desconectado, no hacer nada especial
  } else if (type == WStype_TEXT) {
    // Cliente envió un mensaje de texto, verificamos si es para solicitar la temperatura
    String message = String((char*)payload).substring(0, length);
    if (message == "getHumedad") {
      // El cliente solicita la temperatura, la enviamos
      enviarHumedad();
    }
  }
}

void enviarHumedad() 
{
  float humedad = dht11.readHumidity();
  String humedadStr = String(humedad) + "%";
  wsHumedad.broadcastTXT(humedadStr);
}

void onWebSocketEventGetTemp(uint8_t num, WStype_t type, uint8_t *payload, size_t length) 
{
  if (type == WStype_CONNECTED) {
    // Cliente conectado, no hacer nada especial
  } else if (type == WStype_DISCONNECTED) {
    // Cliente desconectado, no hacer nada especial
  } else if (type == WStype_TEXT) {
    // Cliente envió un mensaje de texto, verificamos si es para solicitar la temperatura
    String message = String((char*)payload).substring(0, length);
    float temp = message.toFloat();
    set_temp = temp;
    sentinela_get_temp = true;
    wsGetTemp.broadcastTXT("Ajustando temperatura a: " + String(set_temp) + "°C");
  }
}

void prenderClima()
{
  float lectura_temp = dht11.readTemperature();
  if(set_temp < lectura_temp)
  {
    digitalWrite(rele_fuente, HIGH);
    digitalWrite(rele_ventilador_externo, HIGH);
  }
  else
  {
    digitalWrite(rele_fuente, LOW);
    digitalWrite(rele_ventilador_externo, LOW);
    sentinela_get_temp = false;
  }

}

void onWebSocketEventControlLuz(uint8_t num, WStype_t type, uint8_t *payload, size_t length) 
{
  if (type == WStype_CONNECTED) {
    // Cliente conectado, no hacer nada especial
  } else if (type == WStype_DISCONNECTED) {
    // Cliente desconectado, no hacer nada especial
  } else if (type == WStype_TEXT) {
    // Cliente envió un mensaje de texto, verificamos si es para solicitar la temperatura
    String message = String((char*)payload).substring(0, length);
    if (message == "Luz") {
      // El cliente solicita la temperatura, la enviamos
      estadoLuz = !estadoLuz;
      digitalWrite(rele_luz, estadoLuz ? HIGH : LOW);
    }
  }
}


void onWebSocketEventSensorHumedadSuelo(uint8_t num, WStype_t type, uint8_t *payload, size_t length) 
{
  if (type == WStype_CONNECTED) {
    // Cliente conectado, no hacer nada especial
  } else if (type == WStype_DISCONNECTED) {
    // Cliente desconectado, no hacer nada especial
  } else if (type == WStype_TEXT) {
    // Cliente envió un mensaje de texto, verificamos si es para solicitar la temperatura
    String message = String((char*)payload).substring(0, length);
    if (message == "HumedadSuelo") {
      // El cliente solicita la temperatura, la enviamos
      enviarHumedadSuelo();
    }
  }
}

void enviarHumedadSuelo() 
{
  float humedadSuelo = analogRead(sensor_humedad_tierra);
  float humedadPorcentaje = map(humedadSuelo, 0, 1023, 100, 0);
  String humedadSueloStr = String(humedadPorcentaje) + "%";
  wsSensorHumedadSuelo.broadcastTXT(humedadSueloStr);
}

