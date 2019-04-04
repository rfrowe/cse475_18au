#include "Active3.h"
#include "Debug.h"
#include "Midi.h"
#include "Neopixel.h"

// Melody

constexpr uint8_t Active3::_localWeights[];

uint8_t Active3::getNumRepeats() {
 return random(4, 10);
}

void Active3::loop(uint32_t dt) {
  // Sounds
  if (random(0, 10) <= 6) {
    _creature.setMidiMode(Midi::setSound(_creature.getMidiMode(), random(0x19, 0x1F)));
  }

  // Effects
  uint16_t effects[] = {0x05};
  uint16_t effectSize = 1; 
  Neopixel::setLight(effects[random(0, effectSize)]);
}

const uint8_t* Active3::getLocalWeights() {
  return this->_localWeights;
}

float Active3::getStartleFactor() {
  return 0.0005f;
}
