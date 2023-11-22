#include <Adafruit_NeoPixel.h>  // Neopixel을 사용하기 위해서 라이브러리를 불러옵니다.
#ifdef __AVR__
 #include <avr/power.h>         // Required for 16 MHz Adafruit Trinket
#endif

#define PIN 7                   // 디지털 입력 핀 설정
#define NUMPIXELS 2             // Neopixel LED 소자 수 (LED가 24개라면 , 24로 작성)
#define BRIGHTNESS 180          // 밝기 설정 0(어둡게) ~ 255(밝게) 까지 임의로 설정 가능
 
#define BUTTON 0
#define IRsensor A0 //1번핀

int button_val = 0;

float sensorValue, cm;
const int numReadings = 30;    // 평균을 취할 데이터 갯수
long readings[numReadings];    // 신호값저장할 배열 지정
int readIndex = 0;             // 현재 신호값 index 변수 선언
long total = 0;
long average = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setLED(){
  digitalWrite(RGB_BUILTIN, HIGH);    // Turn the RGB LED white
  delay(500);
  digitalWrite(RGB_BUILTIN, LOW);     // Turn the RGB LED off

  strip.setBrightness(BRIGHTNESS);    //  BRIGHTNESS 만큼 밝기 설정 
  strip.begin();                      //  Neopixel 제어를 시작
  strip.show();                       //  Neopixel 동작 초기화 합니다

}

void setBUTTON(){
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), conLED, RISING);

}

void setIRSENSOR(){
    for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
    }
}

void startstrip(){
  strip.begin();
  strip.setPixelColor(0, 255, 255, 255);          //  Neopixel 색상 설정
  strip.setPixelColor(1, 255, 255, 255);          //  ( 소자위치 , (Red) , (Green) , (Blue) ) 3가지 색을 다 킨다면 White가 켜짐
  strip.show();                               //  LED가 켜지는 동작을 하게 합니다

}

//---------------------------------------------------------------//

void conLED(){
  button_val++;
  switch(button_val%4){

  case 1:
    neopixelWrite(RGB_BUILTIN,RGB_BRIGHTNESS,0,0); // Red
    Serial.printf("conled_val_1: %d,  ",button_val);
    break;


  case 2:
   neopixelWrite(RGB_BUILTIN,0,RGB_BRIGHTNESS,0); // Green
   Serial.printf("conled_val_2: %d,  ",button_val);
   break;

  case 3:
    neopixelWrite(RGB_BUILTIN,0,0,RGB_BRIGHTNESS); // Blue
    Serial.printf("conled_val_3: %d,  ",button_val);
   break;

  case 4:
   neopixelWrite(RGB_BUILTIN,0,0,0); // Off / black
   Serial.printf("conled_val_4: %d,  ",button_val);
   break;

  default:
   neopixelWrite(RGB_BUILTIN,0,0,0); // Off / black
   Serial.printf("conled_val_0: %d,  ",button_val);
   break;
  }

}

long getIR_Distance(){
  while(!(readIndex==numReadings)){

  sensorValue = analogRead(IRsensor);

  cm = 3*10650.08 * pow(sensorValue,-0.935);

  total = total - readings[readIndex]; // readIndex가 가르키는 값을 제외
  readings[readIndex] = cm; // 센서값을 읽어 배열에 저장
  total = total + readings[readIndex]; // 현재값을 Total에 저장
  readIndex = readIndex + 1; // 다음 배열에 저장하기 위해 readindex를 증가

  }

 if (readIndex >= numReadings)
   readIndex = 0;
  
  average = total / numReadings; // 평균치 계산

  Serial.print("Distance(cm) = ");
  Serial.println(average);

  delay(150);
  return average;
}

void colorWipe(uint32_t c, uint8_t wait){    //  loop에서 사용할 colorWipe 함수를 만든 공식
  for(uint16_t i=0; i<strip.numPixels(); i++){
    strip.setPixelColor(i,c);
    strip.show();
    delay(wait);
    }

}

void lightNEO(){
  colorWipe(strip.Color(0,0,0),250);  //  strip 색상을 ( 255 (Red) , 255 (Green) , 255 (Blue) , 100)
  delay(500);
  colorWipe(strip.Color(255,255,255),250);  //  strip 색상을 ( 255 (Red) , 255 (Green) , 255 (Blue) , 100)
  delay(500);

}

//---------------------------------------------------------------//

void setup(){

  Serial.begin(115200);
  setLED();
  setBUTTON();
  setIRSENSOR();
  startstrip();

}

void loop(){

  lightNEO();
  getIR_Distance();

}