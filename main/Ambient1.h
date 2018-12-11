#ifndef _AMBIENT1_H_
#define _AMBIENT1_H_

#include "Creature.h"
#include "State.h"

class Ambient1 : public State {
 public:
  Ambient1(Creature& creature) : State(creature, "Ambient 1", 1) {}
  uint8_t getNumRepeats() override;
  void loop(uint32_t dt) override;
  const uint8_t* getLocalWeights() override;
  float getStartleFactor() override;
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = {10,  15,  10,  2, 9, 1};
};

#endif  // _AMBIENT1_H_
