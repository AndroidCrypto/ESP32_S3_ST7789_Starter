/*
  This sketch shows how to work with the ESP32-S3 Supermini development board.
  Attached is an 2.0 inch TFT display that runs a ST7789 driver.
  The display has a size of 240x320 pixels.
  The library for the display is TFT_eSPI in a modified version, because the original
  version (2.5.4) can't run on ESP32 SDK 3.x so far and this SDK version is needed to
  select ESP32-S3 boards.

  Purpose of the sketch:
  - print data on the TFT display in different colors and sizes
  - get system informations about the ESP32 chip on the board
  - use the onboard RGB LED
  - the onboard (red) LED is not used in the sketch separately as it is connected to
    the same GPIO pin as the RGB LED

  This is tested with ESP32 Board version 3.2.0 with SDK 5.4.1 on Arduino IDE 2.3.6
  TFT_eSPI version: 2.5.43
  FastLED version:  3.9.15
*/

/*
Version Management
08.05.2025 V01 Initial programming Important: runs 40 MHz SPI speed only, the S3 Supermini runs on 80 MHz
*/

/*
TFT 240 x 320 pixels 2.0 inch ST7789 display wiring to an ESP32-S3 Supermini
Terminals on display's pcb from left to right

TFT   ESP32-S3
GND   GND
VDD   3.3V 
SCL   13
SDA   12 (= "MOSI")
RST   11
DC    10
CS    9
BLK   8 *1)

Note *1) If you don't need a dimming you can connect BLK with 3.3V
Note *2) The display does not have a MISO ("output") terminal, so it is not wired
*/

// -------------------------------------------------------------------------------
// Sketch and Board information
const char *PROGRAM_VERSION = "ESP32-S3 Supermini ST7789 Starter V01";
const char *PROGRAM_VERSION_SHORT = "ST7789 Starter V01";
const char *DEVICE_NAME = "ESP32-S3 Supermini  2.0 in. 240 x 320 px";
const char *DIVIDER = "--------------------";

// -------------------------------------------------------------------------------
// TFT Display
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI();

#define TFT_BL_PIN 8               // backlight brightness control, needs to be a PWM pin
#define TFT_BRIGHTNESS_PERCENT 90  // avoids overheating of the device
#define TFT_TEXT_SIZE 2
#define TFT_WIDTH 240
#define TFT_LED_TEXT_START 240
#define TFT_AUTHOR_TEXT_START 290

// -------------------------------------------------------------------------------
// RGB LED
#include <FastLED.h>  // https://github.com/FastLED/FastLED
#define RGB_LED_PIN 48
#define RGB_NUM_LEDS 1
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define RGB_LED_BRIGHTNESS_PERCENT 30
CRGB leds[RGB_NUM_LEDS];
#define FRAMES_PER_SECOND 120  // rainbow effect
uint8_t gHue = 0;              // rotating "base color" used by many of the patterns

// -------------------------------------------------------------------------------
// Onboard LED
// The Onboard LED is internally wired to the same GPIO as the RGB LED, so it is not
// useful to put it on or off
//#define ONBOARD_LED_PIN 48  // HIGH = LED on, LOW = LED off

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(PROGRAM_VERSION);

  // init the display
  tft.begin();

  // set the brightness to 90% to avoid heating of the device
  pinMode(TFT_BL_PIN, OUTPUT);
  analogWrite(TFT_BL_PIN, 255 * TFT_BRIGHTNESS_PERCENT / 100);
  delay(10);

  // setup the RGB LED
  CFastLED::addLeds<LED_TYPE, RGB_LED_PIN, COLOR_ORDER>(leds, RGB_NUM_LEDS);
  FastLED.setBrightness(RGB_LED_BRIGHTNESS_PERCENT);

  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(TFT_TEXT_SIZE);
  tft.setRotation(0);  // portrait

  tft.setTextSize(TFT_TEXT_SIZE);
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(0, 0);
  tft.println(DEVICE_NAME);
  tft.setTextColor(TFT_RED);
  tft.println(PROGRAM_VERSION_SHORT);

  tft.setTextColor(TFT_WHITE);
  tft.println(DIVIDER);

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Processor: "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getChipModel());

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Revision:  "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getChipRevision());

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Cores:     "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getChipCores());

  tft.setTextColor(TFT_BLUE);
  tft.print(F("CPU Freq:  "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getCpuFreqMHz());
  tft.println(F(" MHz"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("Flash:     "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getFlashChipSize() / (1024.0 * 1024));
  tft.println(F(" mb"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("PSRAM:     "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getPsramSize() / (1024.0 * 1024));
  tft.println(F(" mb"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("HEAP:      "));
  tft.setTextColor(TFT_YELLOW);
  tft.print(ESP.getHeapSize() / 1024.0);
  tft.println(F(" kb"));

  tft.setTextColor(TFT_BLUE);
  tft.print(F("SDK: "));
  tft.setTextColor(TFT_YELLOW);
  tft.println(ESP.getSdkVersion());

  tft.setTextColor(TFT_WHITE);
  tft.println(DIVIDER);

  tft.setTextSize(2);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.drawCentreString(F(" RED   LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, 2);
  leds[0] = CRGB::Red;
  FastLED.delay(500);
  delay(1000);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawCentreString(F(" GREEN LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, 2);
  leds[0] = CRGB::Green;
  FastLED.delay(500);
  delay(1000);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.drawCentreString(F(" BLUE  LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, 2);
  leds[0] = CRGB::Blue;
  FastLED.delay(500);
  delay(1000);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawCentreString(F(" WHITE LED "), TFT_WIDTH / 2, TFT_LED_TEXT_START, 2);
  leds[0] = CRGB::White;
  FastLED.delay(500);
  delay(1000);
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.drawCentreString(F("RAINBOW LED"), TFT_WIDTH / 2, TFT_LED_TEXT_START, 2);
  leds[0] = CRGB::White;
  fill_rainbow(&(leds[0]), RGB_NUM_LEDS, gHue);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(TFT_TEXT_SIZE);
  tft.drawCentreString(F("by AndroidCrypto"), TFT_WIDTH / 2, TFT_AUTHOR_TEXT_START, 1);
}

void loop() {
  // Rainbow effect
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) {
    gHue++;
  }  // slowly cycle the "base color" through the rainbow
  fill_rainbow(&(leds[0]), RGB_NUM_LEDS, gHue);
}
