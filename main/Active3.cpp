#include "Active3.h"
#include "Debug.h"
#include "Midi.h"

constexpr uint8_t Active3::_localWeights[];

uint8_t Active3::getNumRepeats() {
 return rand() % 5 + 4; // 4 - 8 repeats
}

void Active3::loop(uint32_t dt) {
  Midi::setSound(3);
}

const uint8_t* Active3::getLocalWeights() {
  return this->_localWeights;
}

float Active3::getStartleFactor() {
  return 0.001f;
}
