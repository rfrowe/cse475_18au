#include "Creature.h"
#include "State.h"
#include "Startle.h"
#include "Midi.h"
#include "Neopixel.h"
#include "Debug.h"
#include "Audio.h"

State::State(Creature& creature, char* const name, const uint8_t id) : _creature(creature), _id(id) {
  strncpy(_name, name, MAX_NAME_LEN);
  _name[MAX_NAME_LEN] = 0;
};

uint8_t State::getId() {
  return _id;
}

char* State::getName() {
  return _name;
}

bool State::rxPlaySound(uint8_t len, uint8_t* payload) {
  if (len < 1) {
    return false;
  }
  if (payload[0] == 0) {
    Audio::setMP3(_creature, 0, false, 1);
  } else {
    Audio::setMidi(_creature, payload[0]);
  }
  return true;
}

bool State::rxPlayEffect(uint8_t len, uint8_t* payload) {
  if (len < 1) {
    return false;
  }
  Neopixel::setLight(payload[0]);
  return true;
}

bool State::rxStartle(int8_t rssi, uint8_t len, uint8_t* payload) {
  if (len != 2) {
    return false;
  }
  uint8_t strength = payload[0];
  uint8_t id = payload[1];

  uint8_t decayStrength = strength;
  float decay = (1.f / (1.f + exp((-_creature.GLOBALS.STARTLE_DECAY - rssi) / (float) _creature.GLOBALS.STARTLE_DECAY))) * getStartleFactor();
  strength = (uint8_t) round(decay * strength);

  startled(strength, id);
  return true;
}

void State::txStartle(uint8_t strength, uint8_t id) {
  uint8_t pld[2];
  pld[0] = strength;
  pld[1] = id;
  _creature.tx(6, 255, 2, pld);
}

void State::startled(uint8_t strength, uint8_t id) {
  uint8_t last = _creature.getLastStartleId();
  if (id != last) {
    _creature.updateThreshold();
    if (strength >= _creature.getStartleThreshold()) {
      _creature.setNextState(new Startle(_creature));
      txStartle(strength, id);
      _creature.setLastStartleId(id);
      _creature.setStartleThreshold(255);
    }
  }
}

State* State::transition() {
  // Get total number of active creatures (i.e. they've recently communicated & are not in Wait or Startle)
  // Get the total number of creatures in each state
  // Get the total sum of the inverse absolute value of the RSSI
  uint8_t numActiveCreature = 0;
  uint8_t stateSums[ACTIVE_STATES + AMBIENT_STATES] = { 0 };
  float distanceStateSums[ACTIVE_STATES + AMBIENT_STATES] = { 0 };
  for (uint8_t i = 1; i < _creature.GLOBALS.NUM_CREATURES + 1; i++) {
    if (_creature.getCreatureStates()[i] > 0 && _creature.getCreatureStates()[i] <= (ACTIVE_STATES + AMBIENT_STATES)) {
      numActiveCreature += 1;
      float creatureInverseDistance = _creature.getCreatureDistances()[i] ? -1.f / _creature.getCreatureDistances()[i] : 0;
      stateSums[_creature.getCreatureStates()[i] - 1] += 1;
      distanceStateSums[_creature.getCreatureStates()[i] - 1] += creatureInverseDistance;
    }
  }

  // Calculate the global scalar values taking into account the states of other creatures
  float stateGlobalScalars[ACTIVE_STATES + AMBIENT_STATES] = { 0 };
  for (uint8_t i = 0; i < ACTIVE_STATES + AMBIENT_STATES; i++) {
    stateGlobalScalars[i] = numActiveCreature ? _globalWeights[i] * ((numActiveCreature - stateSums[i]) / (float) numActiveCreature) : 0;
  }

  float stateLikelihoods[ACTIVE_STATES + AMBIENT_STATES] = { 0 };
  for (uint8_t i = 0; i < ACTIVE_STATES + AMBIENT_STATES; i++) {
    stateLikelihoods[i] = getLocalWeights()[i] + stateGlobalScalars[i] * distanceStateSums[i];
  }

  dprintln(F("State transition weights:"));
  dprint(stateLikelihoods[0]);
  dprint(F("\t"));
  for (uint8_t i = 1; i < ACTIVE_STATES + AMBIENT_STATES; i++) {
    stateLikelihoods[i] += stateLikelihoods[i - 1];
    dprint(stateLikelihoods[i]);
    dprint(F("\t"));
  }
  dprintln();

  float randomVal = static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX / (stateLikelihoods[ACTIVE_STATES + AMBIENT_STATES - 1])));

  uint8_t stateID = 0;
  for (uint8_t i = 0; i < ACTIVE_STATES + AMBIENT_STATES; i++) {
    if (randomVal < stateLikelihoods[i]) {
      stateID = i + 1;
      break;
    }
  }

  dprint(F("Generated "));
  dprint(randomVal);
  dprint(F(" -->  "));
  dprintln(stateID);

  return _creature.getState(stateID);
}

int8_t* State::getGlobalWeights() {
  return _globalWeights;
}
