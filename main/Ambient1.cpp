#include "Ambient1.h"
#include "Debug.h"

constexpr uint8_t Ambient1::_localWeights[];

uint8_t Ambient1::getNumRepeats() {
 return rand()%4 + 4;
}

uint8_t Ambient1::getId() {
  return 1;
}

void Ambient1::loop(uint32_t dt) {
  Serial.println(F("Ambient..."));
}

const uint8_t* Ambient1::getLocalWeights() {
  return this->_localWeights;
}

float Ambient1::getStartleFactor() {
  return 9999999999;
}

bool Ambient1::rxStartle(uint8_t len, uint8_t* payload) {}

void Ambient1::PIR() {
  dprintln("PIR triggered!");
}

void Ambient1::startled() {}
