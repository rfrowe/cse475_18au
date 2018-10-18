void Wait::loop() { }

State* Wait::transition() {
  return &this;
}

void Wait::rx_stop() { }
void Wait::rx_start(uint8_t mode) { }
void Wait::rx_play_sound(uint8_t sound_idx) { }
void Wait::rx_play_effect(uint8_t effect_idx) { }
void Wait::rx_broadcast_states(uint8_t* states) { }
void Wait::rx_startle(uint8_t strength, uint8_t id) { }
void Wait::tx_entered_state(uint8_t old_state, uint8_t state) { }
void Wait::tx_startle(uint8_t strength) { }
void Wait::PIR() { }
void Wait::startled() { }
void Wait::playSound(uint8_t sound_idx, bool broadcast) { }
void Wait::playEffect(uint8_t effect_idx, bool broadcast) { }
