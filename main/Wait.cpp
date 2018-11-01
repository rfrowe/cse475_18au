#include "Wait.h"
#include "Debug.h"
//#include "Midi.h"

constexpr uint8_t Wait::_localWeights[];

uint8_t Wait::getNumRepeats() {
 return 255;
}

State* Wait::transition() {
 return this;
}

uint8_t Wait::getId() {
  return 0;
}

void Wait::loop(uint32_t dt) {
  Serial.println(F("Waiting..."));
}

const uint8_t* Wait::getLocalWeights() {
  return this->_localWeights;
}

float Wait::getStartleFactor() {
  return 9999999999;
}

bool Wait::rxStartle(uint8_t len, uint8_t* payload) {}

void Wait::PIR() {
  dprintln("PIR triggered!");
}

void Wait::startled() {}
