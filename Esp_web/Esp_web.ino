#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "webpage.h"

const char* ssid = "RATS_2.4G";
const char* password = "rats8005";

AsyncWebServer server(80);

int valueToDisplay = 0;

void setupWiFi() {
  Serial.begin(115200);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("Wifi IP: ");
  Serial.println(WiFi.localIP());
}

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    // Serve the page with the current value
    String page = String(index_html);
    page.replace("%VALUE%", String(valueToDisplay));
    request->send(200, "text/html", page);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    // Update values here
    valueToDisplay = analogRead(A0);

    // Serve only the updated value without the full HTML content
    request->send(200, "text/plain", String(valueToDisplay));
  });

  server.begin();
}

void setup() {
  setupWiFi();
  setupServer();
}

void loop() {
  // You can handle other tasks here

  delay(1000); // Delay to control how often the values are updated
}