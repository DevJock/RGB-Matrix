#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN 3
#define COLOR_ORDER GRB
#define CHIPSET WS2812B
#define BRIGHTNESS 32

const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;

#define NUM_LEDS (kMatrixWidth * kMatrixHeight)

CRGB leds_plus_safety_pixel[ NUM_LEDS + 1];
CRGB* const leds( leds_plus_safety_pixel + 1);

// Fucntion to Correct for Serpant Layout
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( y & 0x01) {   
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  return i;
}

void setLed(int x, int y, uint16_t r, uint16_t g, uint16_t b){
  leds[NUM_LEDS - XY(x,y) - 1] = CRGB(r,g,b);
  FastLED.show();
}

void Clear(){
    for( int y = 0; y < kMatrixHeight; y++) {  
    for( int x = 0; x < kMatrixWidth; x++) {
      leds[ XY(x, y)] = 000000;
    }
  }
  FastLED.show();
}

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
  Serial.println("Initialized");
  Serial.flush();
}

void loop()
{
  if(Serial.available()){
    delay(20);
    String data = Serial.readStringUntil('\n');
    uint8_t x = data.substring(0,1).toInt();
    uint8_t y = data.substring(1,2).toInt();

    uint16_t number = strtol( &data[2], NULL, 16);

    // Split them up into r, g, b values
    uint16_t r = number >> 16;
    uint16_t g = number >> 8 & 0xFF;
    uint16_t b = number & 0xFF;
    setLed(x,y,r,g,b);
    if(Serial.available()){
      Serial.read();
    }
  }
}