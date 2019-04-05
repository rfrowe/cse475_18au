#include "Ambient3.h"
#include "Debug.h"
#include "Midi.h"
#include "Neopixel.h"
#include "Audio.h"

// Wind

constexpr uint8_t Ambient3::_localWeights[];

uint8_t Ambient3::getNumRepeats() {
 return random(4, 12);
}

void Ambient3::loop(uint32_t dt) {
  Audio::setMidi(_creature, 0x15, true);
  
  // Effects
  uint16_t effects[] = {0x01, 0x0A};
  uint16_t effectSize = 2; 
  Neopixel::setLight(effects[random(0, effectSize)]);
}

const uint8_t* Ambient3::getLocalWeights() {
  return this->_localWeights;
}

float Ambient3::getStartleFactor() {
  return 0.001f;
}
