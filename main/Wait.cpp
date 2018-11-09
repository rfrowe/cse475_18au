#include "Wait.h"
#include "Neopixel.h"
#include "Debug.h"

constexpr uint8_t Wait::_localWeights[];

uint8_t Wait::getNumRepeats() {
 return 32;
}

State* Wait::transition() {
  Midi::setSound(Midi::getSound() ? 0 : 1);
  Neopixel::setLight(Neopixel::getLight() ? 0 : 1);
  return this;
}

uint8_t Wait::getId() {
  return 0;
}

void Wait::loop(uint32_t dt) {
//  dprintln(F("Waiting..."));
}

const uint8_t* Wait::getLocalWeights() {
  return this->_localWeights;
}

float Wait::getStartleFactor() {
  return 9999999999;
}

bool Wait::rxStartle(int8_t rssi, uint8_t len, uint8_t* payload) {}

void Wait::PIR() {
  dprintln("PIR triggered!");
}

void Wait::startled() {}
