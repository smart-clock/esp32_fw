#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebSrv.h>
#include <Time.h>

const char* ssid = "MJU_WIFI";
const char* password = "mjuwlan!";
//MJU_WIFI , mjuwlan!

// Weather API
const char* weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=yongin,kr&appid=1a485d0da9911216d18dd97c782eb32f";

// StockData API
const char* alphaVantageApiKey = "90SJ6Y9ECCZU8VNC";
const char* alphaVantageApiUrl = "https://www.alphavantage.co/query";

// Google Calendar API
const char* calendarApiKey = "AIzaSyBzuoDpuWBwncCQy0sgaZxkeWBB7CCgVGE";
const char* calendarId = "385956556797-c8ps53eei1tcb836qmhetc3o294noqki.apps.googleusercontent.com";  // Calendar ID for the calendar you want to access

// finnhubApi
const char* finnhubApiKey = "cletue1r01qnc24enns0cletue1r01qnc24ennsg";

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

    getStockDataMonth("AMZN");
}

void loop() {
    // Repeatedly fetch and print data every 10 minutes
    
}

void getStockDataMonth(String symbol) 
{
    // Create an HTTP client object
    HTTPClient http;
    int dataCnt = 0;
    String data[22];
    String stmPacket = "";

    String alphaVantageUrl = 
        String(alphaVantageApiUrl) + "?function=TIME_SERIES_DAILY&symbol=" + String(symbol) + "&apikey=" + String(alphaVantageApiKey);
    Serial.println("Stock URL: " + alphaVantageUrl);    

    // Make the request
    http.begin(alphaVantageUrl.c_str());

    // Check for the response code
    int httpCode = http.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            // Parse JSON response
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            deserializeJson(doc, payload);

            // Extract and print stock information (example for TIME_SERIES_INTRADAY)
            String stockName = doc["Meta Data"]["2. Symbol"];
            JsonObject timeSeries = doc["Time Series (Daily)"];

            for (JsonPair entry : timeSeries) 
            {
                if(dataCnt < 22)
                {
                    String date = entry.key().c_str();
                    double closeValue = entry.value()["4. close"];
                    Serial.print("Date: ");
                    Serial.print(date);
                    Serial.print(", Close: ");
                    Serial.println(closeValue);

                    data[dataCnt] = closeValue; // save data
                }
                else
                {
                    break;
                }

                dataCnt++;
            }

            stmPacket = "*SM^" + String(symbol);
            for(int i = 0; i < 22; i++)
            {
                stmPacket += "," + String(data[i]);
            }
            stmPacket += "\n";

            Serial.print(stmPacket);
        }
        else 
        {
            Serial.println("HTTP request failed with error code: " + String(httpCode));
        }
    }
    else {
        Serial.println("HTTP request failed");
    }

    // Close the connection
    http.end();
}

