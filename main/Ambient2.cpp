#include "Ambient2.h"
#include "Debug.h"
#include "Midi.h"
#include "Neopixel.h"

// Insects

constexpr uint8_t Ambient2::_localWeights[];

uint8_t Ambient2::getNumRepeats() {
 return random(5, 10);
}

void Ambient2::loop(uint32_t dt) {
  // Sounds
  if (random(0, 10) <= 3) {
    Midi::setSound(random(0, 3) == 0 ? 0x29 : 0x2B);
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
