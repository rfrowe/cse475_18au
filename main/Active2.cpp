#include "Active2.h"
#include "Debug.h"

constexpr uint8_t Active2::_localWeights[];

uint8_t Active2::getNumRepeats() {
 return 255;
}

State* Active2::transition() {
 State::transition();
// return this;
}

uint8_t Active2::getId() {
  return 0;
}

void Active2::loop(uint32_t dt) {
  Serial.println(F("Active..."));
}

const uint8_t* Active2::getLocalWeights() {
  return this->_localWeights;
}

float Active2::getStartleFactor() {
  return 9999999999;
}

bool Active2::rxStartle(uint8_t len, uint8_t* payload) {}

void Active2::PIR() {
  dprintln("PIR triggered!");
}

void Active2::startled() {}
