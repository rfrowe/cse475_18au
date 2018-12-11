#include "Wait.h"
#include "Neopixel.h"
#include "Debug.h"

constexpr uint8_t Wait::_localWeights[];

uint8_t Wait::getNumRepeats() {
 return 255;
}

State* Wait::transition() {
  return this;
}

void Wait::loop(uint32_t dt) {
}

const uint8_t* Wait::getLocalWeights() {
  return this->_localWeights;
}

float Wait::getStartleFactor() {
  return 9999999999;
}

void Wait::startled(uint8_t strength, uint8_t id) {}
