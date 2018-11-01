#ifndef _MIDI_H_
#define _MIDI_H_

#include "stdbool.h"

#include <cinttypes>

#define VS1053_MIDI Serial1

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80
#define VS1053_GM1_SLAP_BASS_1 36

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1000

typedef struct SoundGesture {
  const uint8_t bank;
  const uint8_t instrument;
  const uint8_t volume;
  bool loop;
  uint8_t len;
  uint8_t* notes;
  uint8_t* durations;
};

void tcReset();
bool tcIsSyncing();

class Midi {
 public:
  // More to come
  static SoundGesture SCALE;

  static void tcStartCounter();

  static void tcConfigure(int frequencyHz);

  /**
   * Use this to set the current sound gesture. Do not set
   * current directly as this can cause notes to get stuck.
   *
   * @param gesture New SoundGesture to start playing
   */
  static void setCurrent(SoundGesture* gesture);

  static SoundGesture* current;
};

#endif  // _MIDI_H_
