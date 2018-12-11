#ifndef _ACTIVE2_H_
#define _ACTIVE2_H_

#include "Creature.h"
#include "State.h"

class Active2 : public State {
 public:
  Active2(Creature& creature) : State(creature, "Active 2", 4) {}
  uint8_t getNumRepeats() override;
  void loop(uint32_t dt) override;
  const uint8_t* getLocalWeights() override;
  float getStartleFactor() override;
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = {10,  10,  8, 4, 6, 2};
};

#endif  // _ACTIVE2_H_
