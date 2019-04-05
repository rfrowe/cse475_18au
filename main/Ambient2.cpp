#include "Ambient2.h"
#include "Debug.h"
#include "Midi.h"
#include "Neopixel.h"
#include "Audio.h"

// Insects

constexpr uint8_t Ambient2::_localWeights[];

uint8_t Ambient2::getNumRepeats() {
 return random(5, 10);
}

void Ambient2::loop(uint32_t dt) {
  static uint8_t reps = 0;

  // Sounds
  if (random(0, 10) <= 3) {
    Audio::setMidi(_creature, random(0, 3) == 0 ? 0x29 : 0x2B, true);
    reps = random(3, 6);
  } else if (reps > 0) {
    reps--;
  } else {
    Audio::setMidi(_creature, 0);
  }

  // Effects
  uint16_t effects[] = {0x06, 0x0C};
  uint16_t effectSize = 2; 
  Neopixel::setLight(effects[random(0, effectSize)]);
}

const uint8_t* Ambient2::getLocalWeights() {
  return this->_localWeights;
}

float Ambient2::getStartleFactor() {
  return 0.001f;
}
