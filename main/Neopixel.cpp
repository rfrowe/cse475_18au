#include "Neopixel.h"
#include "Debug.h"

// Initialize fixed list of light gestures.
constexpr bool (*Neopixel::LIGHTS[])(uint32_t);

uint8_t Neopixel::_currentIdx = 0;
uint32_t Neopixel::_lastLoop = 0;
Adafruit_NeoPixel_ZeroDMA Neopixel::_strip = Adafruit_NeoPixel_ZeroDMA(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRBW);

void Neopixel::setup() {
  _strip.begin();
  _strip.setBrightness(NEOPIXEL_BRIGHTNESS);
  _strip.show();
}

void Neopixel::loop() {
  uint32_t thisLoop = millis();
  uint32_t dt;
  if (_lastLoop) {
    dt = thisLoop - _lastLoop;
  } else {
    dt = 0;
  }

  if (LIGHTS[_currentIdx] != nullptr) {
    if (LIGHTS[_currentIdx](dt) || dt == 0) {
      _lastLoop = thisLoop;
    }
  }
}

void Neopixel::setLight(uint8_t lightIdx) {
  // lightIdx soundIdx. Anything outside of the bounds of the array is 0.
  if (lightIdx >= sizeof(LIGHTS) / sizeof(void*)) {
    lightIdx = 0;
  }

  if (_currentIdx != lightIdx) {
    dprint(F("Next light: "));
    dprintln(lightIdx);

    _strip.setBrightness(NEOPIXEL_BRIGHTNESS);
    _strip.clear();
    _strip.show();

    _currentIdx = lightIdx;
    _lastLoop = 0;
  }
}

uint8_t Neopixel::getLight() {
  return _currentIdx;
}

void Neopixel::_fill(uint32_t c, uint16_t first, uint16_t count) {
  for (uint8_t pix = first; pix < first + count; pix++) {
    _strip.setPixelColor(pix % NEOPIXEL_COUNT, c);
  }
}
