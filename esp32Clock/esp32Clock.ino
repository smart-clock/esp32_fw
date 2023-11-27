/* USER CODE BEGIN Includes */
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <ESPAsyncWebSrv.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>

#include "webpage.h"
/* USER CODE END Includes */


/* USER CODE BEGIN PD */
#define STOCK_NAME_SIZE 32
#define WEATHER_INFO_SIZE 32
#define TRANSPORTATION_INFO_SIZE 32

#define STM32_UART_TX_PIN   (5)
#define STM32_UART_RX_PIN   (4)

#define USER_BUTION_PIN     (0) // 0 : dev board, 17 : our board
#define BUZZER_PIN          (8)
#define RGB_LED_PIN         (9)

#define RGB_LED_NUM         (2) // TODO
#define BRIGHTNESS          (180) //  밝기 설정 0(어둡게) ~ 255(밝게) 까지 임의로 설정 가능
/* USER CODE END PD */


/* USER CODE BEGIN PV */
const char* ssid = "smartclock"; // RATS_2.4G
const char* password = "rats8005"; // rats8005

// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 32400;  // +9:00 (9hours*60mins*60seconds)
const int daylightOffset_sec = 0;

//const char* weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=yongin,kr&appid=1a485d0da9911216d18dd97c782eb32f";
const char* weatherApiKey = "1a485d0da9911216d18dd97c782eb32f";

const char* calendarApiKey = "AIzaSyBzuoDpuWBwncCQy0sgaZxkeWBB7CCgVGE";
const char* calendarId = "385956556797-c8ps53eei1tcb836qmhetc3o294noqki.apps.googleusercontent.com";  // Calendar ID for the calendar you want to access

const char* finnhubApiKey = "cletue1r01qnc24enns0cletue1r01qnc24ennsg";

const char* alphaVantageApiKey = "90SJ6Y9ECCZU8VNC"; 
const char* alphaVantageApiUrl = "https://www.alphavantage.co/query";

AsyncWebServer server(80);

int valueToDisplay = 0;
String stockName = "No Stock";
String weatherInfo = "No Weather Info";
String transportationInfo = "No Transportation Info";

String esp2stmPacket = "";

Adafruit_NeoPixel strip = Adafruit_NeoPixel(RGB_LED_NUM, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t period = 0;
uint32_t periodPrev[3] = {0};
/* USER CODE END PV */


/* USER CODE BEGIN PFP */
void setupUART();
void setupWiFi();
void setupServer();
void setUserButton();
void setRgbLed();
void startstrip();

void colorWipe(uint32_t c, uint8_t wait);
void lightNEO();

void saveToEEPROM();
void loadFromEEPROM();

void checkAndPrintStockName();
void checkAndPrintWeather();
void checkAndPrintTransportation();

void getBatteryState(); // TODO
void getDateTime();
void getKoreaWeather();
void getCurrentStockData(String symbol); // Finnhub API
void getMonthStockData(String symbol); // Alpha Vantage API

void sendUserButtonPacket();

void sendPacketToStm(String packet);
void receivePacketFromStm();
/* USER CODE END PFP */


/* USER CODE BEGIN SETUP, LOOP */
void setup() 
{
    setupUART();
    setupWiFi();
    loadFromEEPROM(); // Load data from EEPROM
    setupServer();
    setUserButton();
    setRgbLed();
    startstrip();

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    getDateTime();
    getKoreaWeather();
    getCurrentStockData(stockName);
    getMonthStockData(stockName);
}

void loop()
{
    checkAndPrintStockName();
    checkAndPrintWeather();
    checkAndPrintTransportation();

    period = millis();

    if((period - periodPrev[0]) > 10000) // 10초
    {
        getKoreaWeather();
        periodPrev[0] = period;
    }

    if((period - periodPrev[1]) > 30000)
    {
        getCurrentStockData(stockName);
        periodPrev[1] = period;
    }
}
/* USER CODE BEGIN SETUP, LOOP */


/* USER CODE BEGIN PFP DEFINITION */
void setupUART()
{
    // UART0 : ESP to COM
    Serial.begin(115200);

    // UART1 : ESP to STM
    delay(100);
    Serial1.begin(115200, SERIAL_8N1, STM32_UART_RX_PIN, STM32_UART_TX_PIN); // RX, TX
    Serial1.println();
    Serial1.println("ESP to STM...");
    delay(100);
}

void setupWiFi() 
{
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    Serial.print("Wifi IP: ");
    Serial.println(WiFi.localIP());

    esp2stmPacket = "*WF^" + String(ssid);
    sendPacketToStm(esp2stmPacket);
    delay(100);

    esp2stmPacket = "*IP^" + WiFi.localIP().toString();
    sendPacketToStm(esp2stmPacket);
    delay(100);

    // Set up the time zone
    configTime(9 * 3600, 0, "pool.ntp.org", "time.nist.gov");
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
      weatherInfo = city + "," + country; // Implement your logic to get weather info
    }

    // Update transportation information
    if (request->hasParam("platform") && request->hasParam("bus")) {
      String platform = request->getParam("platform")->value();
      String bus = request->getParam("bus")->value();
      transportationInfo = platform + ", " + bus; // Implement your logic to get transportation info
    }

    // Save to EEPROM
    saveToEEPROM();

    request->send(200, "text/plain", "All information updated and saved to EEPROM");
  });

  server.begin();
}

void setUserButton()
{
    pinMode(USER_BUTION_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(USER_BUTION_PIN), sendUserButtonPacket, RISING);
}

void setRgbLed()
{
    strip.setBrightness(BRIGHTNESS);    //  BRIGHTNESS 만큼 밝기 설정 
    strip.begin();                      //  Neopixel 제어를 시작
    strip.show();                       //  Neopixel 동작 초기화 합니다
}

void startstrip()
{
  strip.begin();
  strip.setPixelColor(0, 255, 255, 255);          //  Neopixel 색상 설정
  strip.setPixelColor(1, 255, 255, 255);          //  ( 소자위치 , (Red) , (Green) , (Blue) ) 3가지 색을 다 킨다면 White가 켜짐
  strip.show();                               //  LED가 켜지는 동작을 하게 합니다
}

void saveToEEPROM() {
  EEPROM.begin(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE);

  // Write each variable to EEPROM
  EEPROM.writeString(0, stockName);
  EEPROM.writeString(STOCK_NAME_SIZE, weatherInfo);
  EEPROM.writeString(STOCK_NAME_SIZE + WEATHER_INFO_SIZE, transportationInfo);

  EEPROM.commit();
  EEPROM.end();
}

void loadFromEEPROM() {
  EEPROM.begin(STOCK_NAME_SIZE + WEATHER_INFO_SIZE + TRANSPORTATION_INFO_SIZE);

  // Read each variable from EEPROM
  stockName = EEPROM.readString(0);
  weatherInfo = EEPROM.readString(STOCK_NAME_SIZE);
  transportationInfo = EEPROM.readString(STOCK_NAME_SIZE + WEATHER_INFO_SIZE);

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

void getBatteryState()
{

}

void sendPacketToStm(String packet)
{
    Serial1.println(packet);
}

void receivePacketFromStm()
{

}

void getDateTime() 
{
    String date = "";
    String day = "";

    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) 
    {
        Serial.println("[ERROR] getDateTime()");
        delay(1000);
        return;
    }

    switch (timeinfo.tm_wday)
    {
        case 0 :
            day = "Sun";
            break;
        case 1 :
            day = "Mon";
            break;
        case 2 : 
            day = "Tue";
            break;
        case 3 :
            day = "Wed";
            break;
        case 4 :
            day = "Thu";
            break;
        case 5 : 
            day = "Fri";
            break;
        case 6 :
            day = "Sat";
            break;
        default:
            break;
    }

    date = String(timeinfo.tm_year + 1900) + "-" + String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_mday) + ","
            + String(day) + "," 
            + String(timeinfo.tm_hour) + ":" + String(timeinfo.tm_min) + ":" + String(timeinfo.tm_sec);

    Serial.println("Date : " + date);

    esp2stmPacket = "*DA^" + String(date);
    sendPacketToStm(esp2stmPacket);
}

void getKoreaWeather() 
{
    // Create an HTTP client object
    HTTPClient http;

    String weatherApiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + String(weatherInfo) + "&appid=" + String(weatherApiKey);

    // Make the request
    http.begin(weatherApiUrl.c_str());

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
            String main = doc["weather"][0]["main"];
            int temperature = doc["main"]["temp"];
            temperature -= 273;

            Serial.println("City: " + city);
            Serial.println("Main: " + main); // use main cuz description is too long
            Serial.println("Temperature: " + String(temperature) + "°C");

            // Send Packet from ESP to STM
            esp2stmPacket = "*WT^" + String(city) + "," + String(main) + "," + String(temperature);
            sendPacketToStm(esp2stmPacket);
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

void getCurrentStockData(String stockName) 
{
    // Create an HTTP client object
    HTTPClient http;

    // Build the Finnhub API request URL with API key and correct endpoint for getting stock quotes
    String finnhubApiUrl = "https://finnhub.io/api/v1/quote?symbol=" + String(stockName) + "&token=" + String(finnhubApiKey); // Example symbol (AAPL)
    String currentPrice = "";

    // Make the request
    http.begin(finnhubApiUrl.c_str());

    // Check for the response code
    int httpCode = http.GET();
    if (httpCode > 0) 
    {
        if (httpCode == HTTP_CODE_OK) 
        {
            // Parse JSON response
            String payload = http.getString();
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            // Check if the response contains the necessary keys
            if (doc.containsKey("c") && doc.containsKey("pc") && doc.containsKey("t")) 
            {
                // Extract and print stock data
                Serial.println("Stock Data from Finnhub:");
                Serial.println("Symbol: " + String(stockName));

                currentPrice = String(doc["c"].as<float>());
                Serial.println("Current Price: " + currentPrice);
                // Serial.println("Previous Close: " + String(doc["pc"].as<float>()));

                // Extract and print UNIX timestamp in milliseconds
                // unsigned long timestamp = doc["t"].as<unsigned long>();
                // Serial.print("Timestamp (UNIX milliseconds): " + String(timestamp));

                // Convert UNIX timestamp to a common time zone (e.g., UTC)
                // struct tm timeinfo;
                // if (!getLocalTime(&timeinfo)) {
                //   Serial.println("Failed to obtain time");
                //   return;
                // }

                // Serial.printf(", Converted Time: %04d-%02d-%02d %02d:%02d:%02d",
                //               timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                //               timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

                Serial.println();

                // Send Packet from ESP to STM
                esp2stmPacket = "*SC^" + String(stockName) + "," + String(currentPrice);
                sendPacketToStm(esp2stmPacket);
            } 
            else 
            {
                Serial.println("Error: Required keys not found in Finnhub API response.");
                Serial.println("Payload: " + payload);
            }
        } 
        else 
        {
            Serial.println("Finnhub HTTP request failed with error code: " + String(httpCode));
        }
    }
    else 
    {
        Serial.println("Finnhub HTTP Request Failed");  
    }

    // Close the connection
    http.end();
}

void getMonthStockData(String stockName) 
{
    // Create an HTTP client object
    HTTPClient http;
    int dataCnt = 0;
    String data[22];
    String stmPacket = "";

    String alphaVantageUrl = 
        String(alphaVantageApiUrl) + "?function=TIME_SERIES_DAILY&symbol=" + String(stockName) + "&apikey=" + String(alphaVantageApiKey);
    Serial.println("Stock URL: " + alphaVantageUrl);    

    // Make the request
    http.begin(alphaVantageUrl.c_str());

    // Check for the response code
    int httpCode = http.GET();
    if (httpCode > 0) 
    {
        if (httpCode == HTTP_CODE_OK) 
        {
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

            esp2stmPacket = "*SM^" + String(stockName);
            for(int i = 0; i < 22; i++)
            {
                esp2stmPacket += "," + String(data[i]);
            }
            sendPacketToStm(esp2stmPacket);
        }
        else
        {
            Serial.println("Alpha HTTP request failed with error code: " + String(httpCode));
        }
    }
    else 
    {
        Serial.println("Alpha HTTP request failed");
    }

    // Close the connection
    http.end();
}

void sendUserButtonPacket()
{
    esp2stmPacket = "*BU^1";
    sendPacketToStm(esp2stmPacket);
}
/* USER CODE BEGIN PFP DEFINITION */