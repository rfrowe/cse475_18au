#include <cinttypes>

#include <Adafruit_FeatherOLED.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <RH_RF69.h>
#include <Wire.h>

#define VREF 3.3

#define ID_PIN A0
#define VBAT_PIN A7
#define LED_PIN 13

#define NEOPIXEL_PIN 19
#define NEOPIXEL_COUNT 16

#define RFM69_FREQ 915.0
#define RFM69_CS 8
#define RFM69_INT 3
#define RFM69_RST 4

class Creature {
 public:
   Creature(uint8_t kit_num);
   Creature(const Creature&) = delete;
   Creature& operator=(Creature const&) = delete;

   RH_RF69 rf69 = new RH_RF69(RFM69_CS, RFM69_INT);
   Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();
   Adafruit_NeoPixel_ZeroDMA strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW);
 private:
   State* _current;
   uint8_t _kit_num, _addr;
   uint8_t _last_spook, _last_loop;
}
