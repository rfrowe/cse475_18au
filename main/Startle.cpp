// Startle State file
// Akhil Avula

#include "Startle.h"
#include "Debug.h"

constexpr uint8_t Startle::_localWeights[];

uint8_t Startle::getNumRepeats() {
 return 255;
}

State* Startle::transition() {
 State::transition();
// return this;
}

uint8_t Startle::getId() {
  return 0;
}

void Startle::loop(uint32_t dt) {
  Serial.println(F("Startling..."));
}

const uint8_t* Startle::getLocalWeights() {
  return this->_localWeights;
}

float Startle::getStartleFactor() {
  return 9999999999;
}

bool Startle::rxStartle(uint8_t len, uint8_t* payload) {}

void Startle::PIR() {
  dprintln("PIR triggered!");
}

void Startle::startled() {}
