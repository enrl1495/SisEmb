La idea es hacer el ESP32 un servidor cargando una pagina web que se pueda comunicar con el mismo ESP32 para controlar un invernadero.

Para lograrlo, se tienen los siguientes puntos clave:

1. Crear un sistema de archivos en el ESP33.
2. Hacer que el ESP32 sea un punto de acceso.
3. Hacer que el ESP32 sea un servidor web.
4. Usar websockets para la intercomunicacion.
5. Acceder a la página y empezar a controlar el invernadero.

Explicación.

1. SISTEMA DE ARCHIVOS.
   LittleFS es un sistema de archivos diseñado para sistemas embebidos como el ESP32. Permite almacenar y manejar archivos de manera eficiente
   en dispositivos con recursos limitados. Al cargar una carpeta para servir una página web en ESP32, estarías utilizando LittleFS para
   almacenar los archivos HTML, CSS, JavaScript, y otros recursos de la página web en la memoria del ESP32. Esto permite que el ESP32
   pueda servir estos archivos de manera rápida y eficiente cuando se accede a la página web desde un navegador.

   Para este paso es importante contar con una version antigua de Arduino IDE, luego, descomprimir el archivo .rar y en la ruta
   'C:\Users\*usuario*\Documents\Arduino\tools\ESP32FS\tool', pegar el archivo descomprimido. Para subir archivos se tiene que ejecutar el siguiente codigo:
'''cpp
#include "FS.h"
#include "LittleFS.h"

void setup() {
   Serial.begin(115200);
  
   if (!LittleFS.begin()) {
       Serial.println("No se pudo montar LittleFS");
       return;
}
  
   Serial.println("Formateando LittleFS...");
  
   if (LittleFS.format()) {
      Serial.println("LittleFS formateado con éxito");
   } else {
      Serial.println("Error al formatear LittleFS");
    }
}
  
void loop() {
   // El loop está vacío
}
'''

   Esto formatea la memoria al formato de LittleFS para poder subir archivos como si se tratara de explorador de windows.

   Luego, en la carpeta donde se está trabajando hay que crear un directorio llamado 'data' y poner dentro, los archivos a cargar. Y en la versión antigua de Arduino IDE aparece la siguiente opción:

   ![ArduinoUpload](upload.png)

   Luego de subir los arhivos, hay que ingresar el codigo que carga los archivos para poder visualizarlos en forma
   de pagina web con el siguiente codigo:

   File file = LittleFS.open("/INV.html", "r");
      if (file) {
        server.streamFile(file, "text/html");
        file.close();

2.PUNTO DE ACCESO
   El punto de acceso se crea de la siguiete forma:
   
   const char* ssid = "AgroSmart";
   const char* password = "12345678";
   WiFi.softAP(ssid, password);
   
3. Hacer servidor web el ESP32.

      AsyncWebServer server(80);
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

4. Usar webSockets para la comunicacion.

   WebSocketsServer wsGetTemp(83);
   wsGetTemp.onEvent(onWebSocketEventGetTemp);
   wsGetTemp.begin();
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




