#ifndef _SOUND_H_
#define _SOUND_H_

#include "inttypes.h"

class Sound {
 public:
  Sound(const uint8_t bank,
        const uint8_t instrument,
        const uint8_t volume,
        const uint8_t len,
        const uint8_t* const notes,
        const uint16_t* const durations,
        const uint8_t transpose = 0,
        const uint8_t duration_offset = 0) :
      bank(bank), instrument(instrument), volume(volume), len(len),
      transpose(transpose), duration_offset(duration_offset),
      notes(notes), durations(durations) { }

  Sound(const Sound &) = delete;
  Sound &operator=(Sound const &) = delete;

  const uint8_t bank, instrument, volume, len, transpose, duration_offset;
  const uint8_t* const notes;
  const uint16_t* const durations;
};

#endif  // _SOUND_H_
