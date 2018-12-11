#ifndef _ACTIVE3_H_
#define _ACTIVE3_H_

#include "Creature.h"
#include "State.h"

class Active3 : public State {
 public:
  Active3(Creature& creature) : State(creature, "Active 3", 6) {}
  uint8_t getNumRepeats();
  void loop(uint32_t dt);
  const uint8_t* getLocalWeights();
  float getStartleFactor();
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = {9,  12,  7, 4, 5, 3};
};

#endif  // _ACTIVE3_H_
