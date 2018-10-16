#include <cinttypes>

class State {
 public:
  // TODO: call set_globals with defaults in impl.
  State(uint8_t kit_num) : _kit_num(kit_num) {}
  State(const State&) = delete;
  State& operator=(State const&) = delete;
 protected:
  void loop(uint32_t dt);
  inline uint8_t addr() final { return this->_addr; }
  void rx();
  bool tx();

  // Packet receivers
  void rx_set_globals();
  void rx_stop();
  void rx_start();
  void rx_spook(uint8_t strength, spookID);
  void rx_entered_state();
  void rx_play_sound();
  void rx_play_effect();
  void rx_report_local_topology();
  void rx_broadcast_topology();

  // Called when PIR pin goes from LOW to HIGH.
  void PIR();

  // Called when this creature is spookd. Should set
  void spookd();

  // Called when a sound should be played
  void playSound(uint8_t sound_idx, bool);
 private:
  void _tx();
  void _rx();
  void _loop(uint32_t dt);
  uint8_t _kit_num, _addr, _last_spook_id;
  uint32_t _last_spook, _last_loop;
  State _next;
}
