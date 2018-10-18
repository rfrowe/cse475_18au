#ifndef _STATE_H_
#define _STATE_H_

#include <cinttypes>

#include "Creature.h"

class Creature;

class State {
 public:
  // TODO: call set_globals with defaults in impl.

  State(Creature& creature) : _creature(creature) {};
  State(const State&) = delete;
  State& operator=(State const&) = delete;
 protected:
//   void loop(uint32_t dt);
//   void rx();
//   bool tx();

  // Packet receivers
  void rx_set_globals(uint8_t* payload);
  void rx_stop();
  void rx_start(uint8_t mode);
  void rx_play_sound(uint8_t sound_idx);
  void rx_play_effect(uint8_t effect_idx);
  void rx_broadcast_states(uint8_t* states);
  void rx_startle(uint8_t strength, uint8_t id);

  // Packet transmitters
  void tx_entered_state(uint8_t old_state, uint8_t state);
  void tx_startle(uint8_t strength);

  // Event handlers
  // Called when PIR pin goes from LOW to HIGH.
  void PIR();
  // Called when this creature is startled. Should set
  void startled();
  // Called when a sound should be played
  void playSound(uint8_t sound_idx, bool broadcast);
  // Called when an effect should be displayed
  void playEffect(uint8_t effect_idx, bool broadcast);
 private:
//   static uint8_t* _localWeights;
//   static uint8_t* _globalWeights;
  static uint8_t _startleDecay;
  uint8_t _repeat_state;

  Creature& _creature;
};

#endif  // _STATE_H_
