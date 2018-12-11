#include "Startle.h"
#include "Midi.h"
#include "Neopixel.h"

constexpr uint8_t Startle::_localWeights[];

uint8_t Startle::getNumRepeats() {
  return rand() % 4 + 1; // 1 - 4
}

void Startle::loop(uint32_t dt) {
  Midi::setSound(0x27, false);
  Neopixel::setLight(0x03);
}

const uint8_t* Startle::getLocalWeights() {
  return this->_localWeights;
}

float Startle::getStartleFactor() {
  return 9999999999;
}

void Startle::startled(uint8_t strength, uint8_t id) {}
