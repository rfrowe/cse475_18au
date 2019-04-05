#include "Ambient1.h"
#include "Debug.h"
#include "Midi.h"
#include "Neopixel.h"
#include "Audio.h"

// Weather

constexpr uint8_t Ambient1::_localWeights[];

uint8_t Ambient1::getNumRepeats() {
 return random(10, 15);
}

void Ambient1::loop(uint32_t dt) {
  // Sounds
  if (random(0, 10) <= 3) {
    Audio::setMidi(_creature, random(0x13, 0x18), true);
  }

  // Effects
  uint16_t effects[] = {0x07, 0x0B, 0x0F};
  uint16_t effectSize = 3; 
  Neopixel::setLight(effects[random(0, effectSize)]);
}

const uint8_t* Ambient1::getLocalWeights() {
  return this->_localWeights;
}

float Ambient1::getStartleFactor() {
  return 0.001f;
}
