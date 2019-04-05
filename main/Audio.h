#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <Adafruit_VS1053.h>
#include <SPI.h>
#include <SD.h>

class Creature;

#define MP3_VOLUME 1

#define VS1053_RESET   -1     // VS1053 reset pin (not used!)
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
// DREQ should be an Int pin
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

const unsigned short sVS1053b_Realtime_MIDI_Plugin[28] = { /* Compressed plugin */
  0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, /*    0 */
  0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, /*    8 */
  0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, /*   10 */
  0x0200, 0x000a, 0x0001, 0x0050,
};

static Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

class Audio {
 public:
   Audio();
   Audio(const Audio&) = delete;

   /**
    * Use this to set the current sound gesture.
    *
    * @param soundIdx Index of new sound to play in Midi::SOUNDS.
    * @param loop Whether or not the sound should loop when finished.
    * @param transpose Amount each note should be shifted in the sound.
    * @param duration_offset Amount each note's duration should be shifted in the sound.
    * @param retrograde Whether or not the sound should be played in reverse.
    * @param instrument Instrument override or negative for default sound instrument.
    */
   static void setMidi(Creature& creature, uint8_t soundIdx, bool loop=false, uint8_t transpose=0, uint16_t duration_offset=0, bool retrograde=false, int16_t instrument=-1);

   static void setMP3(Creature& creature, uint8_t soundIdx, bool loop=false, uint8_t volume=1);

   static void init();

 private:
   static void MidiMode(void);
   static void MP3Mode(void);
};
#endif  // _AUDIO_H_
