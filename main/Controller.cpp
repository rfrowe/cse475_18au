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

int counter = 0;
void Controller::loop(uint32_t dt) {
  
  if (!(counter % 10)) {
    txGlobals();
  }
  txStates();
  counter++;
}

bool Controller::txStates() {
  return _creature.tx(5, _creature.getAddr(), 0xFF, _creature.GLOBALS.NUM_CREATURES, _creature.getCreatureStates() + 1);
}

bool Controller::txGlobals() {
  return _creature.tx(0, _creature.getAddr(), 0xFF, sizeof(_creature.GLOBALS), (uint8_t*)&_creature.GLOBALS);
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
