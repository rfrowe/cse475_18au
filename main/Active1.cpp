#include "Active1.h"
#include "Debug.h"
#include "Midi.h"

constexpr uint8_t Active1::_localWeights[];

uint8_t Active1::getNumRepeats() {
 return rand() % 4 + 1; // 1 - 4
}

void Active1::loop(uint32_t dt) {
  Midi::setSound(15);
}

const uint8_t* Active1::getLocalWeights() {
  return this->_localWeights;
}

float Active1::getStartleFactor() {
  return 0.001f;
}
