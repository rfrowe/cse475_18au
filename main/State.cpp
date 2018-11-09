#include "Creature.h"
#include "State.h"

State::State(Creature& creature, char* const name) : _creature(creature) {
  strncpy(_name, name, MAX_NAME_LEN);
};

char* State::getName() {
  return _name;
}

void State::playSound(uint8_t sound_idx) {
  // TODO: implement
}

void State::playEffect(uint8_t effect_idx) {
  // TODO: implement
}

bool State::rxPlaySound(uint8_t len, uint8_t* payload) {
  // TODO: implement
}

bool State::rxPlayEffect(uint8_t len, uint8_t* payload) {
  // TODO: implement
}

bool State::rxStartle(uint8_t len, uint8_t* payload) {
  // TODO: implement
}

void State::txStartle(uint8_t strength, uint8_t id) {  
  // TODO: implement
}

int[] State::calProb() {
  // TODO: Implement math to replace this
  int probabilities [7] = [5, 7, 12, 3, 2, 9, 23];
  return probabilities;
}

State* State::getNewState(int stateNum) {
  switch(stateNum) {
    // TODO: return states to be filled
    case 1:
      // return wait state
      break;
    case 2:
      // return active1
      break;
    case 3:
      // return active2
      break;
    case 4:
      // return active3
      break;
    case 5:
      // return ambient1
      break;
    case 6:
      // return ambient2
      break;
    case 7:
      // return ambient3
      break;
  }
}

State* State::transition() {
  int probabilities [7] = calProb();
  int cumProbabilities [7];
  int stateNum = 0;
  cumProbabilities = probabilities;
  for (int i = 1; i < probabilities.length(); i++) {
    cumProbabilities[i] = cumProbabilities[i - 1] + probabilities[i];
  }
  int num = rand(0,cumProbabilities[6]);
  for (int i = cumProbilities.length() - 1; i >= 0; i--) {
    if (num <= cumProbabilities[i]) {
      stateNum = i;
    }
  }
  return State::getNewState(stateNum);
//  // boundary numbers
//  for (int i = 0; i < cumProbabilities.length(); i++) {
//    if (num == cumProbabilities[i]) {
//      stateNum = i;
//    }
//  }
//  // otherwise round up to the next index
//  for (int i = 0; i < cumProbabilities.length() - 1; i++) {
//    if (num >= cumProbabilities[i] && num <= cumProbabilities[i+1]) {
//      stateNum = i+1;
//    }
//  }
  // TODO: implement
}

uint8_t* State::getGlobalWeights() {
  return _globalWeights;
}
