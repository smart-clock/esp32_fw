#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebSrv.h>
#include <Time.h>


int curmil = 0;
int premil[3] = {0};

const char* ssid = "MJU_WIFI";
const char* password = "mjuwlan!";
//MJU_WIFI , mjuwlan!

// BUS ARIVAL API
const char* alphaVantageApiKey = "sWGTGWYKqpdlcuyYS8CIoal8B6PI7MdA9J91AYatDLLVXY7HCNcqaku7%2F9raxJeF7jN%2BZP0LNzoEvksW3Q3vGg%3D%3D";
const char* busArivalApiUrl = "http://apis.data.go.kr/1613000/ArvlInfoInqireService";

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

    busArival();
}

void loop() {
    // Repeatedly fetch and print data every 10 minutes

    curmil = millis();

    if(curmil-premil[0]>1000){
      Serial.println(getTime());
      premil[0]=curmil;
    }
    else if(curmil-premil[1]>30000){
      API_CALL();
      premil[1]=curmil;
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
    busArival();
}

void busArival() {
    // Create an HTTP client object
    HTTPClient http;

    String busArivalUrl = String(busArivalApiUrl) + "/getSttnAcctoSpcifyRouteBusArvlPrearngeInfoList?serviceKey=" + String(alphaVantageApiKey) + "&pageNo=1" + "&numOfRows=10" + "&_type=json" + "&cityCode=31190" + "&nodeId=GGB228002959" + "&routeId=GGB228000175";

    Serial.println(busArivalUrl);
    // Make the request
    http.begin(busArivalUrl.c_str());

    // Check for the response code
    int httpCode = http.GET();
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            // Parse JSON response
            String payload = http.getString();
            DynamicJsonDocument doc(4096);
            deserializeJson(doc, payload);

            // 경로 설정
            String item = doc["response"]["body"]["items"]["item"][0];

              String nodenm             = doc["response"]["body"]["items"]["item"][0]["nodenm"];
              String routeno            = doc["response"]["body"]["items"]["item"][0]["routeno"];
              String arrprevstationcnt  = doc["response"]["body"]["items"]["item"][0]["arrprevstationcnt"];
              String arrtime0           = doc["response"]["body"]["items"]["item"][0]["arrtime"];
              String arrtime1           = doc["response"]["body"]["items"]["item"][1]["arrtime"];
    /*
              Serial.println(" item : " + item);
              Serial.println("정류소 이름 : " + nodenm);
              Serial.println("버스 번호 : " + routeno);
              Serial.println("1번째 버스 도착 시간 [s] : " + arrtime0);
              Serial.println("2번째 버스 도착 시간 [s] : " + arrtime1);
    */
              String BUSsand = "*BS^" + String(routeno) + "," + String(nodenm) + "," + String(arrtime0) + "," + String(arrtime0);
            Serial.println(BUSsand);

        } else {
            Serial.println("HTTP request failed with error code: " + String(httpCode));
        }
    } else {
        Serial.println("HTTP request failed");
    }

    // Close the connection
    http.end();
}

String getTime() {
  WiFiClient client;
  while (!!!client.connect("google.com", 80)) { //  google.com 연결 안되면, !!! 또는 ! 는 부울
  Serial.println("connection failed, retrying..."); 
  }
  
  client.print("HEAD / HTTP/1.1\r\n\r\n"); // google.com 에 접속해서 헤더파일 받아오는 명령어?

  while(!!!client.available()) {} // !!! 또는 ! 는 부울

  while(client.available()){
    if (client.read() == '\n') {   
      if (client.read() == 'D') {   
        if (client.read() == 'a') {   
          if (client.read() == 't') {   
            if (client.read() == 'e') {   
              if (client.read() == ':') {   
                client.read();
                String theDate = client.readStringUntil('\r'); 
                client.stop();
                return theDate;  // Date: 까지 읽고, 맞으면 \r 까지 읽어오기.
              }
            }
          }
        }
      }
    }
  }
}