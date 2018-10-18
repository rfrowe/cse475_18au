#ifndef _CREATURE_H_
#define _CREATURE_H_

#include <cinttypes>

#include <Adafruit_FeatherOLED.h>
#include <Adafruit_NeoPixel_ZeroDMA.h>
#include <RH_RF69.h>
#include <Wire.h>
#include "State.h"

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

  void setup();

  RH_RF69 _rf69 = RH_RF69(RFM69_CS, RFM69_INT);
  Adafruit_FeatherOLED _oled = Adafruit_FeatherOLED();
  Adafruit_NeoPixel_ZeroDMA _strip = Adafruit_NeoPixel_ZeroDMA(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW);

  uint8_t get_addr() {
    return _addr;
  }

  uint8_t get_last_startle_id() {
    return _last_startle_id;
  }

  void set_last_startle_id(uint8_t startle_id) {
    _last_startle_id = startle_id;
  }

  uint32_t get_last_startle() {
    return _last_startle;
  }

  void set_last_startle(uint32_t last_startle) {
    _last_startle = last_startle;
  }

  uint8_t* get_creature_states() {
    return _creature_states;
  }

  int8_t* get_creature_distances() {
    return _creature_distances;
  }
 private:
  // State& _current;
  uint8_t _kit_num, _addr, _last_startle_id;
  uint32_t _last_startle, _last_loop;
  uint8_t* _creature_states;
  int8_t* _creature_distances;
};

#endif  // _CREATURE_H_
