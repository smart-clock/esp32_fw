/*
    Go to thingspeak.com and create an account if you don't have one already.
    After logging in, click on the "New Channel" button to create a new channel for your data. This is where your data will be stored and displayed.
    Fill in the Name, Description, and other fields for your channel as desired, then click the "Save Channel" button.
    Take note of the "Write API Key" located in the "API keys" tab, this is the key you will use to send data to your channel.
    Replace the channelID from tab "Channel Settings" and privateKey with "Read API Keys" from "API Keys" tab.
    Replace the host variable with the thingspeak server hostname "api.thingspeak.com"
    Upload the sketch to your ESP32 board and make sure that the board is connected to the internet. The ESP32 should now send data to your Thingspeak channel at the intervals specified by the loop function.
    Go to the channel view page on thingspeak and check the "Field1" for the new incoming data.
    You can use the data visualization and analysis tools provided by Thingspeak to display and process your data in various ways.
    Please note, that Thingspeak accepts only integer values.

    You can later check the values at https://thingspeak.com/channels/2005329
    Please note that this public channel can be accessed by anyone and it is possible that more people will write their values.
 */

#include <WiFi.h>

const char* ssid     = "SK_WiFiGIGA701C"; // Change this to your WiFi SSID
const char* password = "2004078319"; // Change this to your WiFi password

const char* host = "apis.data.go.kr"; // This should not be changed
const int httpPort = 80; // This should not be changed
const String nodeid   = "GGB228000779"; // Change this to your channel ID
const String nodenm   = "시청.용인대역";
const String serviceKey = "sWGTGWYKqpdlcuyYS8CIoal8B6PI7MdA9J91AYatDLLVXY7HCNcqaku7%2F9raxJeF7jN%2BZP0LNzoEvksW3Q3vGg%3D%3D"; // Change this to your Write API key
const String readApiKey = "sWGTGWYKqpdlcuyYS8CIoal8B6PI7MdA9J91AYatDLLVXY7HCNcqaku7/9raxJeF7jN+ZP0LNzoEvksW3Q3vGg=="; // Change this to your Read API key

// The default example accepts one data filed named "field1"
// For your own server you can ofcourse create more of them.
int citycode = 31190;
char nodeNm[]="명지대";
char cityname[100]=" ";

int numberOfResults = 3; // Number of results to be read
int nodeno = 1; // Field number which will be read out

void setup()
{
    Serial.begin(115200);
    while(!Serial){delay(100);}

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void readResponse(WiFiClient *client){
  unsigned long timeout = millis();
  while(client->available() == 0){
    if(millis() - timeout > 5000){
      Serial.println(">>> Client Timeout !");
      client->stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client->available()) {
    String line = client->readStringUntil('\r');
    Serial.print(line);
  }

  Serial.printf("\nClosing connection\n\n");
}

void loop(){
  WiFiClient client;
  String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "/1613000/BusSttnInfoInqireService/getCtyCodeList"+ "\r\n" + "Connection: close\r\n\r\n";
  Serial.println("be write");
  // WRITE --------------------------------------------------------------------------------------------
  if (!client.connect(host, httpPort)) {
    Serial.println("pass1 x");
    return;
  }
  Serial.println("pass1 o");
  client.print("https://apis.data.go.kr/1613000/BusSttnInfoInqireService/getCtyCodeList?serviceKey=sWGTGWYKqpdlcuyYS8CIoal8B6PI7MdA9J91AYatDLLVXY7HCNcqaku7%2F9raxJeF7jN%2BZP0LNzoEvksW3Q3vGg%3D%3D&");
  readResponse(&client);
  Serial.println("do write");
  // READ --------------------------------------------------------------------------------------------

  String readRequest = "GET /citycode/" + citycode + "/cityname/" +  + ".json?results=" + numberOfResults + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Connection: close\r\n\r\n";
  Serial.println("do read");
  if (!client.connect(host, httpPort)) {
    Serial.println("pass2 o");
    return;
  }

  client.print(readRequest);
  readResponse(&client);

  // -------------------------------------------------------------------------------------------------

  delay(10000);
}