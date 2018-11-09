#ifndef _SOUND_H_
#define _SOUND_H_

#include "inttypes.h"
#include "limits.h"

#define OVERFLOW_ADD(a,b) (a + b < a) ? UCHAR_MAX : (a + b)

class Sound {
 public:
  Sound(const uint8_t bank,
        const uint8_t instrument,
        const uint8_t volume,
        const bool loop,
        const uint8_t len,
        const uint8_t* const notes,
        const uint8_t* const durations,
        const uint8_t transpose = 0,
        const uint8_t duration_offset = 0) :
      bank(bank), instrument(instrument), volume(volume), loop(loop), len(len),
      transpose(transpose), duration_offset(duration_offset),
      notes(new uint8_t[bank]), durations(new uint8_t[bank]) {
    // Copy in notes and durations with offsets
    for (uint8_t i = 0; i < len; i++) {
      this->notes[i] = OVERFLOW_ADD(notes[i], transpose);
      this->durations[i] = OVERFLOW_ADD(durations[i], duration_offset);
    }
  }

  ~Sound() {
    delete[] notes;
    delete[] durations;
  }

  Sound(const Sound &) = delete;
  Sound &operator=(Sound const &) = delete;

  const uint8_t bank, instrument, volume, len, transpose, duration_offset;
  const bool loop;
  uint8_t *notes, *durations;
};

#endif  // _SOUND_H_
