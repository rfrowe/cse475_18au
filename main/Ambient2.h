#ifndef _AMBIENT2_H_
#define _AMBIENT2_H_

#include "Creature.h"
#include "State.h"

class Ambient2 : public State {
 public:
  Ambient2(Creature& creature) : State(creature, "Ambient 2", 3) {}
  uint8_t getNumRepeats();
  void loop(uint32_t dt);
  const uint8_t* getLocalWeights();
  float getStartleFactor();
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = {10,  10,  10,  4, 8, 2};
};

#endif  // _AMBIENT2_H_
