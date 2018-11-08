#ifndef _MIDI_H_
#define _MIDI_H_

#include "stdbool.h"
#include <cinttypes>

#include "Sound.h"

#define VS1053_MIDI Serial1

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// Add any extra instruments that you may need here.
// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80
#define VS1053_GM1_SLAP_BASS_1 36
#define VS1053_GM1_CELLO 43

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1000

void tcReset();
bool tcIsSyncing();

class Midi {
 public:
  // More to come, add your sounds here!
  static Sound SCALE;

  /** Array of all sounds, in a fixed order, used to assign an index to each. Add your sounds to this! */
  static Sound* SOUNDS[2];

  /**
   * Use this to set the current sound gesture.
   *
   * @param soundIdx Index of new sound to play in Midi::SOUNDS.
   */
  static void setSound(uint8_t soundIdx);

  /** Current sound being played. Please do not change this value directly. Use Midi::setSound(int16_t) */
  static Sound* current;
  static uint8_t currentIdx;
  
  static void tcStartCounter();
  static void tcConfigure(int frequencyHz);
};

#endif  // _MIDI_H_
