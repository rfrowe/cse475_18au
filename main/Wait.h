#ifndef _WAIT_H_
#define _WAIT_H_

#include "Creature.h"
#include "State.h"

class Wait : public State {
 public:
  Wait(Creature& creature) : State(creature, "Wait", WAIT) {}
  uint8_t getNumRepeats() override;
  State* transition() override;
  void loop(uint32_t dt) override;
  const uint8_t* getLocalWeights() override;
  float getStartleFactor() override;
protected:
  void startled(uint8_t strength, uint8_t id) override;
private:
  static constexpr uint8_t _localWeights[ACTIVE_STATES + AMBIENT_STATES] = { 0 };
};

#endif  // _WAIT_H_
