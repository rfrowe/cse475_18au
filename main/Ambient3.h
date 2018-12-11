#ifndef _AMBIENT3_H_
#define _AMBIENT3_H_

#include "Creature.h"
#include "State.h"

class Ambient3 : public State {
 public:
  Ambient3(Creature& creature) : State(creature, "Ambient 3", 5) {}
  uint8_t getNumRepeats() override;
  void loop(uint32_t dt) override;
  const uint8_t* getLocalWeights() override;
  float getStartleFactor() override;
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = {10,  8, 10,  2, 20, 1};
};

#endif  // _AMBIENT3_H_
