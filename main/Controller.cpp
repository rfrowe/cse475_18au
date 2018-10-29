#include "Controller.h"
#include "Debug.h"

constexpr uint8_t Controller::_localWeights[];

uint8_t Controller::getNumRepeats() {
 return 255;
}

State* Controller::transition() {
 return this;
}

uint8_t Controller::getId() {
  return 0;
}

void Controller::loop(uint32_t dt) {
  txGlobals();
  txStates();
}

bool Controller::txStates() {
  int size = _creature.GLOBALS.NUM_CREATURES + 1;

  bool res = _creature.tx(5, 0xFF, size, _creature.getCreatureStates());
}

bool Controller::txGlobals() {
  int size = sizeof(_creature.GLOBALS);

  bool res = _creature.tx(0, 0xFF, size, (uint8_t*)&_creature.GLOBALS);
}

const uint8_t* Controller::getLocalWeights() {
  return this->_localWeights;
}

float Controller::getStartleFactor() {
  return 9999999999;
}

bool Controller::rxStartle(uint8_t len, uint8_t* payload) {}

void Controller::PIR() {}

void Controller::startled() {}
