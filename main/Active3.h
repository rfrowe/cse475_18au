#ifndef _AVTIVE3_H_
#define _ACTIVE3_H_

#include "Creature.h"
#include "State.h"

class Active3 : public State {
 public:
  Active3(Creature& creature) : State(creature, "Active") {}
  uint8_t getNumRepeats();
  State* transition() override;
  void loop(uint32_t dt);
  uint8_t getId();
  const uint8_t* getLocalWeights();
  float getStartleFactor();
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = { 0 };
};

#endif  // _ACTIVE_H_
