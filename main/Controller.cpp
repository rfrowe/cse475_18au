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
  uint8_t * states = _creature.getCreatureStates();
  for (int i = 0; i < _creature.GLOBALS.NUM_CREATURES + 1; i++) {
    Serial.print(i == 0 ? "" : ", ");
    Serial.print(states[i]);
  }
  Serial.println();



  
  //dprintln(F("Controlling..."));
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
