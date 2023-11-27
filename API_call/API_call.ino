#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebSrv.h>
#include <Time.h>

const char* ssid = "MJU_WIFI";
const char* password = "mjuwlan!";
//MJU_WIFI , mjuwlan!
//310352, 22222222

// Weather API
const char* weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=yongin,kr&appid=1a485d0da9911216d18dd97c782eb32f";

// Google Calendar API
const char* calendarApiKey = "AIzaSyBzuoDpuWBwncCQy0sgaZxkeWBB7CCgVGE";
const char* calendarId = "385956556797-c8ps53eei1tcb836qmhetc3o294noqki.apps.googleusercontent.com";  // Calendar ID for the calendar you want to access

// finnhubApi
const char* finnhubApiKey = "cletue1r01qnc24enns0cletue1r01qnc24ennsg";
const char* symbol = "AAPL";

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

    // Set up the time zone
    configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    // Set up the server
    server.on("/", HTTP_GET, handleRoot);
    server.begin();
}

void loop() {
    // Repeatedly fetch and print data every 10 minutes
    while (true) {
        API_CALL();
        delay(600000);  // Wait for 10 minutes before the next iteration
    }
}

void handleRoot(AsyncWebServerRequest* request) {
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

void API_CALL() {
    // Call get weather information
    getKoreaWeather();

    // Call the function to get stock data from Finnhub
    getFinnhubStockData();

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
            String description = doc["weather"][0]["main"];
            float temperature = doc["main"]["temp"];

            Serial.println("City: " + city);
            Serial.println("Description: " + description);
            Serial.println("Temperature: " + String(temperature - 273) + " °C");
        }
        else {
            Serial.println("HTTP request failed with error code: " + String(httpCode));
        }
    }
    else {
        Serial.println("HTTP request failed");
    }

    // Close the connection
    http.end();
}

void getFinnhubStockData() {
  // Create an HTTP client object
  HTTPClient http;

  // Build the Finnhub API request URL with API key and correct endpoint for getting stock quotes
  String finnhubApiUrl = "https://finnhub.io/api/v1/quote?symbol=" + String(symbol) + "&token=" + String(finnhubApiKey); // Example symbol (AAPL)

  // Make the request
  http.begin(finnhubApiUrl.c_str());

  // Check for the response code
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      // Parse JSON response
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      // Check if the response contains the necessary keys
      if (doc.containsKey("c") && doc.containsKey("pc") && doc.containsKey("t")) {
        // Extract and print stock data
        Serial.println("Stock Data from Finnhub:");
        //Serial.println("Symbol: " + doc["symbol"].as<String>());
        Serial.println("Symbol: " + String(symbol));
        Serial.println("Current Price: " + String(doc["c"].as<float>()));
        Serial.println("Previous Close: " + String(doc["pc"].as<float>()));

        // Extract and print UNIX timestamp in milliseconds
        unsigned long timestamp = doc["t"].as<unsigned long>();
        Serial.print("Timestamp (UNIX milliseconds): " + String(timestamp));

        // Convert UNIX timestamp to a common time zone (e.g., UTC)
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
          Serial.println("Failed to obtain time");
          return;
        }

        Serial.printf(", Converted Time: %04d-%02d-%02d %02d:%02d:%02d",
                      timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

        Serial.println();
      } else {
        Serial.println("Error: Required keys not found in Finnhub API response.");
        Serial.println("Payload: " + payload);
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
        }
        else {
            Serial.println("HTTP request failed with error code: " + String(httpCode));
        }
    }
    else {
        Serial.println("HTTP request failed");
    }

    // Close the connection
    http.end();
}