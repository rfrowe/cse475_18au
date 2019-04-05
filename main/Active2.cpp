#include "Active2.h"
#include "Midi.h"
#include "Neopixel.h"
#include "Audio.h"

// Bells

constexpr uint8_t Active2::_localWeights[];

uint8_t Active2::getNumRepeats() {
	return random(4, 9); // 4 - 8
}

void Active2::loop(uint32_t dt) {
  // Sounds
  if (random(0, 10) > 6) {
    Neopixel::setLight(0x00);
  } else {
    Audio::setMidi(_creature, random(0x21, 0x25));
    // Effects
    uint16_t effects[] = {0x04, 0x13};
    uint16_t effectSize = 0x02; 
    Neopixel::setLight(effects[random(0x00, effectSize)]);
  }
}

const uint8_t *Active2::getLocalWeights() {
	return this->_localWeights;
}

float Active2::getStartleFactor() {
	return 0.0005f;
}
