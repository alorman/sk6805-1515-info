// A test pogram to have neopixels move on user input.

#include <Adafruit_NeoPixel.h>

//Define LED Pin
#define LED_PIN  14

//Define LED Count
#define LED_COUNT 147

//Establish the neopixle object
Adafruit_NeoPixel TestStrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Define LED Variables
uint8_t RGBcolor [] = {0,0,0};
uint32_t TestColorRGB = TestStrip.Color(RGBcolor[0],RGBcolor[1],RGBcolor[2]);

uint16_t LEDColorH = 100;
uint8_t LEDColorS = 100;
uint8_t LEDColorV = 0;

uint32_t LEDColorHSV = 0;
uint32_t ConvertedHSVtoRGB = 0;

//Define touch variables
uint8_t touchValue = 0;

void setup() {
  Serial.begin(115200); //establish serial port
  TestStrip.begin();
  TestStrip.setBrightness(255); //using this to have the values we input be fully deterministic
  TestStrip.clear();
  TestStrip.show();
  
  Serial.println("Setup Complete...");
  Serial.print("Waiting for button press...");
  while (touchRead(T7) > 54) {
    Serial.print(".");
    delay(500);
    //do nothing
  }
  Serial.println("Starting..."); //nice confirmation that it's actually working
}

void loop() {
  ConvertedHSVtoRGB = TestStrip.gamma32(TestStrip.ColorHSV(LEDColorH, LEDColorS, LEDColorV));
  TestStrip.fill(ConvertedHSVtoRGB);
  uint8_t ColorR = ConvertedHSVtoRGB >> 16;  //take apart the station red and the track red uin 32s so we can do math on them
  uint8_t ColorG = ConvertedHSVtoRGB >> 8;
  uint8_t ColorB = ConvertedHSVtoRGB;
  Serial.println((String)"LED Value is H:" + LEDColorH + " S:" + LEDColorS + " V:" + LEDColorV);
  Serial.println((String)"Converted RGB is:" + ConvertedHSVtoRGB + " R:" + ColorR + " G:" + ColorB + " B:" + ColorB);
  TestStrip.show();
  delay(50);
  Serial.println("Great! Next!");
  LEDColorV++;
}
