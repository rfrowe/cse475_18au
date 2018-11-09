#include "Active2.h"
#include "Debug.h"

constexpr uint8_t Active3::_localWeights[];

uint8_t Active3::getNumRepeats() {
 return rand()%4 + 4;
}

uint8_t Active3::getId() {
  return 6;
}

void Active3::loop(uint32_t dt) {
  Serial.println(F("Active..."));
}

const uint8_t* Active3::getLocalWeights() {
  return this->_localWeights;
}

float Active3::getStartleFactor() {
  return 9999999999;
}

bool Active3::rxStartle(uint8_t len, uint8_t* payload) {}

void Active3::PIR() {
  dprintln("PIR triggered!");
}

void Active3::startled() {}
