//ESP_web.ino
#define STOCK_NAME_SIZE 32
#define WEATHER_INFO_SIZE 32
#define TRANSPORTATION_INFO_SIZE 32
#define RGB_INFO_SIZE 3 // Size for R, G, B values

#include <WiFi.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include "webpage.h"

const char* ssid = "RATS_2.4G";
const char* password = "rats8005";
AsyncWebServer server(80);

int valueToDisplay = 0;
int neopixelRed = 0;
int neopixelGreen = 0;
int neopixelBlue = 0;

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

    if (request->hasParam("red") && request->hasParam("green") && request->hasParam("blue")) {
    neopixelRed = request->getParam("red")->value().toInt();
    neopixelGreen = request->getParam("green")->value().toInt();
    neopixelBlue = request->getParam("blue")->value().toInt();
    }

    // Save to EEPROM
    saveToEEPROM();

    request->send(200, "text/plain", "All information updated and saved to EEPROM");
  });

  server.begin();
}

void setup() {
  setupWiFi();
  loadFromEEPROM(); // Load data from EEPROM
  setupServer();
}

void loop() {

  checkAndPrintStockName();
  checkAndPrintWeather();
  checkAndPrintTransportation();
  checkAndPrintRGB();

  delay(100);
}

void saveToEEPROM() {
  EEPROM.begin(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE + (RGB_INFO_SIZE * 3));

  // Write each variable to EEPROM
  EEPROM.writeString(0, stockName);
  EEPROM.writeString(STOCK_NAME_SIZE, weatherInfo);
  EEPROM.writeString(STOCK_NAME_SIZE + WEATHER_INFO_SIZE, transportationInfo);

  EEPROM.writeInt(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE, neopixelRed);
  EEPROM.writeInt(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE + RGB_INFO_SIZE, neopixelGreen);
  EEPROM.writeInt(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE + (2 * RGB_INFO_SIZE), neopixelBlue);

  EEPROM.commit();
  EEPROM.end();
}

void loadFromEEPROM() {
  EEPROM.begin(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE + (RGB_INFO_SIZE * 3));

  // Read each variable from EEPROM
  stockName = EEPROM.readString(0);
  weatherInfo = EEPROM.readString(STOCK_NAME_SIZE);
  transportationInfo = EEPROM.readString(STOCK_NAME_SIZE + WEATHER_INFO_SIZE);

  neopixelRed = EEPROM.readInt(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE);
  neopixelGreen = EEPROM.readInt(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE + RGB_INFO_SIZE);
  neopixelBlue = EEPROM.readInt(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE + (2 * RGB_INFO_SIZE));

  EEPROM.end();
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

void checkAndPrintRGB() {
  // Print Neopixel color
  static int previousneopixelRed = 0;
  static int previousneopixelGreen = 0;
  static int previousneopixelBlue = 0;

  if (neopixelRed != previousneopixelRed) {
    Serial.print("R= ");
    Serial.println(neopixelRed);
    previousneopixelRed = neopixelRed;
  }
  if (neopixelGreen != previousneopixelGreen) {
    Serial.print(" G= ");
    Serial.println(neopixelGreen);
    previousneopixelGreen = neopixelGreen;
  }
  if (neopixelBlue != previousneopixelBlue) {
    Serial.print(" B= ");
    Serial.println(neopixelBlue);
    previousneopixelBlue = neopixelBlue;
  }
}