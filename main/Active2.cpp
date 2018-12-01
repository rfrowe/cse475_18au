#include "Active2.h"
#include "Debug.h"
#include "Midi.h"

constexpr uint8_t Active2::_localWeights[];

uint8_t Active2::getNumRepeats() {
 return rand() % 5 + 4; // 4 - 8 repeats
}

void Active2::loop(uint32_t dt) {
  Midi::setSound(2);
}

const uint8_t* Active2::getLocalWeights() {
  return this->_localWeights;
}

float Active2::getStartleFactor() {
  return 0.001f;
}
