#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "webpage.h"

const char* ssid = "RATS_2.4G";
const char* password = "rats8005";
AsyncWebServer server(80);

int valueToDisplay = 0;
String stockName = "No Stock";
String weatherInfo = "No Weather Info";
String transportationInfo = "No Transportation Info";

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
    page.replace("No Stock", stockName);
    page.replace("No Weather Info", weatherInfo);
    page.replace("No Transportation Info", transportationInfo);
    request->send(200, "text/html", page);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    // Update values here
    valueToDisplay = analogRead(A0);

    // Serve only the updated value without the full HTML content
    request->send(200, "text/plain", String(valueToDisplay));
  });

  server.on("/updateAll", HTTP_GET, [](AsyncWebServerRequest *request){
    // Update stock name
    if (request->hasParam("name")) {
      stockName = request->getParam("name")->value();
    }

    // Update weather information
    if (request->hasParam("country") && request->hasParam("city")) {
      String country = request->getParam("country")->value();
      String city = request->getParam("city")->value();
      weatherInfo = city + ", " + country; // Implement your logic to get weather info
    }

    // Update transportation information
    if (request->hasParam("platform") && request->hasParam("bus")) {
      String platform = request->getParam("platform")->value();
      String bus = request->getParam("bus")->value();
      transportationInfo = platform + ", " + bus; // Implement your logic to get transportation info
    }

    request->send(200, "text/plain", "All information updated");
  });

  server.begin();
}

void setup() {
  setupWiFi();
  setupServer();
}

void loop() {
  // You can handle other tasks here

  checkAndPrintStockName();
  checkAndPrintWeather();
  checkAndPrintTransportation();

  delay(100); // Delay to control how often the values are updated
}

void checkAndPrintStockName() {
  // Check if stockName has changed
  static String previousStockName = "No Stock";
  if (stockName != previousStockName) {
    Serial.print("Stock Name Info Updated: ");
    Serial.println(stockName);
    previousStockName = stockName;
  }
}

void checkAndPrintWeather() {
  // Check if weather information has changed
  static String previousWeatherInfo = "No Weather Info";
  if (weatherInfo != previousWeatherInfo) {
    Serial.print("Weather Info Updated: ");
    Serial.println(weatherInfo);
    previousWeatherInfo = weatherInfo;
  }
}

void checkAndPrintTransportation() {
  // Check if transportation information has changed
  static String previousTransportationInfo = "No Transportation Info";
  if (transportationInfo != previousTransportationInfo) {
    Serial.print("Transportation Info Updated: ");
    Serial.println(transportationInfo);
    previousTransportationInfo = transportationInfo;
  }
}