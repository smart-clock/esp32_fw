#include <Adafruit_NeoPixel.h>

#define PIN_WS2812B     (16)   // ESP32 pin that connects to WS2812B
#define NUM_PIXELS      (17)  // The number of LEDs (pixels) on WS2812B

Adafruit_NeoPixel ws2812b(NUM_PIXELS, PIN_WS2812B, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
    ws2812b.begin();    
    ws2812b.setBrightness(50);

    // turn on all pixels to red at the same time for two seconds
    for (int pixel = 0; pixel < NUM_PIXELS; pixel++) {         // for each pixel
    ws2812b.setPixelColor(pixel, ws2812b.Color(255, 153, 0));  // it only takes effect if pixels.show() is called
    }
    ws2812b.show();  // update to the WS2812B Led Strip
    delay(1000);     // 1 second on time
}

void loop() {
  // put your main code here, to run repeatedly:

}
