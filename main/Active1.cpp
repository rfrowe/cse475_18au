#include "Active1.h"
#include "Debug.h"
#include "Midi.h"
#include "Neopixel.h"

// Birds

constexpr uint8_t Active1::_localWeights[];

uint8_t Active1::getNumRepeats() {
 return random(10, 15);
}

void Active1::loop(uint32_t dt) {
  // Sounds
  if (random(0, 10) <= 6) {
    Midi::setSound(random(0x01,0x12));
  } 

  // Effects
  uint16_t effects[] = {0x10, 0x11};
  uint16_t effectSize = 0x02; 
  Neopixel::setLight(effects[random(0x00, effectSize)]);
}

const uint8_t* Active1::getLocalWeights() {
  return this->_localWeights;
}

float Active1::getStartleFactor() {
  return 0.001f;
}
