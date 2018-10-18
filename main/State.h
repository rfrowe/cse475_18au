#include <cinttypes>

class State {
 public:
  // TODO: call set_globals with defaults in impl.
  State(uint8_t addr) : _addr(addr) {}
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
  void rx_startle(uint8_t strength, startleID);
  void rx_entered_state();
  void rx_play_sound();
  void rx_play_effect();

  // Called when PIR pin goes from LOW to HIGH.
  void PIR();

  // Called when this creature is startled. Should set
  void startled();

  // Called when a sound should be played
  void playSound(uint8_t sound_idx, bool);
 private:
  // Transmit
  void _tx();
  // Receive
  void _rx();

  // dt = Difference in time
  void _loop(uint32_t dt);

  uint8_t _addr, _last_startle_id;

  uint32_t _last_startle, _last_loop;

  State _next;
}
