#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebSrv.h>

const char *ssid = "310352";
const char *password = "22222222";
//MJU_WIFI , mjuwlan!

// Weather API
const char *weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=yongin,kr&appid=1a485d0da9911216d18dd97c782eb32f";

// StockData API
const char *alphaVantageApiKey = "90SJ6Y9ECCZU8VNC";
const char *alphaVantageApiUrl = "https://www.alphavantage.co/query";

// Google Calendar API
const char *calendarApiKey = "AIzaSyBzuoDpuWBwncCQy0sgaZxkeWBB7CCgVGE";
const char *calendarId = "385956556797-c8ps53eei1tcb836qmhetc3o294noqki.apps.googleusercontent.com";  // Calendar ID for the calendar you want to access

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // Set up the server
  server.on("/", HTTP_GET, handleRoot);
  server.begin();
}

void loop() {
  // Repeatedly fetch and print data every 10 minutes
  while (true) {
    fetchAndPrintData();
    delay(600000);  // Wait for 10 minutes before the next iteration
  }
}

void handleRoot(AsyncWebServerRequest *request) {
  // Redirect to the authorization URL
  request->redirect(getAuthorizationUrl().c_str());
}

String getAuthorizationUrl() {
  // Construct the authorization URL
  String url = "https://accounts.google.com/o/oauth2/auth?";
  url += "client_id=your-385956556797-c8ps53eei1tcb836qmhetc3o294noqki.apps.googleusercontent.com";
  url += "&redirect_uri=http://localhost:8080/test";
  url += "&scope=https://www.googleapis.com/auth/calendar.readonly";
  url += "&response_type=code";
  url += "&access_type=offline";
  return url;
}

void fetchAndPrintData() {
  // Call get weather information
  getKoreaWeather();

  // Call stock data from Alpha Vantage
  getStockData();

  // Call the day's schedule from Google Calendar
  getGoogleCalendarEvents();
}

void getKoreaWeather() {
  // Create an HTTP client object
  HTTPClient http;

  // Make the request
  http.begin(weatherApiUrl);

  // Check for the response code
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Parse JSON response
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      // Extract and print weather information
      String city = doc["name"];
      String description = doc["weather"][0]["description"];
      float temperature = doc["main"]["temp"];

      Serial.println("City: " + city);
      Serial.println("Description: " + description);
      Serial.println("Temperature: " + String(temperature - 273)  + " °C");
    } else {
      Serial.println("HTTP request failed with error code: " + String(httpCode));
    }
  } else {
    Serial.println("HTTP request failed");
  }

  // Close the connection
  http.end();
}

void getStockData() {
  // Create an HTTP client object
  HTTPClient http;

  String alphaVantageUrl = String(alphaVantageApiUrl) + "?function=TIME_SERIES_INTRADAY&symbol=AAPL&interval=5min&apikey=" + String(alphaVantageApiKey);

  // Make the request
  http.begin(alphaVantageUrl.c_str());

 // Check for the response code
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Parse JSON response
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      // Extract and print stock information (example for TIME_SERIES_INTRADAY)
      String stockName = doc["Meta Data"]["2. Symbol"];
      JsonObject timeSeries = doc["Time Series (5min)"];
      String latestDataTime = timeSeries.begin()->key().c_str();
      float latestPrice = timeSeries[latestDataTime]["4. close"];

      Serial.println("Stock Name: " + stockName);
      Serial.println("Stock Data Time: " + latestDataTime);
      Serial.println("Stock Price: " + String(latestPrice) + "$");
    } else {
      Serial.println("HTTP request failed with error code: " + String(httpCode));
    }
  } else {
    Serial.println("HTTP request failed");
  }

  // Close the connection
  http.end();
}

void getGoogleCalendarEvents() {
  // Create an HTTP client object
  HTTPClient http;

  // Build the Google Calendar API request URL
  String calendarApiUrl = "https://www.googleapis.com/calendar/v3/calendars/" + String(calendarId) + "/events?key=" + String(calendarApiKey);

  // Make the request
  http.begin(calendarApiUrl.c_str());

  // Check for the response code
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Parse JSON response
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      // Extract and print events
      Serial.println("Today's Schedule:");
      JsonArray items = doc["items"];
      for (JsonObject event : items) {
        String eventSummary = event["summary"];
        String eventStart = event["start"]["dateTime"];
        String eventEnd = event["end"]["dateTime"];

        Serial.println("Event: " + eventSummary);
        Serial.println("Start: " + eventStart);
        Serial.println("End: " + eventEnd);
        Serial.println();
      }
    } else {
      Serial.println("HTTP request failed with error code: " + String(httpCode));
    }
  } else {
    Serial.println("HTTP request failed");
  }

  // Close the connection
  http.end();
}