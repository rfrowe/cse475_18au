#include "Audio.h"
#include "Creature.h"

void Audio::MidiMode(void) {
  int i = 0;

  while (i<sizeof(sVS1053b_Realtime_MIDI_Plugin)/sizeof(sVS1053b_Realtime_MIDI_Plugin[0])) {
    unsigned short addr, n, val;
    addr = sVS1053b_Realtime_MIDI_Plugin[i++];
    n = sVS1053b_Realtime_MIDI_Plugin[i++];
    while (n--) {
      val = sVS1053b_Realtime_MIDI_Plugin[i++];
      musicPlayer.sciWrite(addr, val);
    }
  }
}

void Audio::MP3Mode(void) {
  musicPlayer.softReset();
  musicPlayer.begin();
  musicPlayer.setVolume(MP3_VOLUME, MP3_VOLUME);
}

void Audio::setMidi(Creature& creature, uint8_t soundIdx, bool loop, uint8_t transpose, uint16_t duration_offset, bool retrograde, int16_t instrument) {
  if (!creature.getMidiMode()) {
    creature.setMidiMode(true);
    MidiMode();
  }
  MidiMode();
  Midi::setSound(soundIdx, loop, transpose, duration_offset, retrograde, instrument);
}

void Audio::setMP3(Creature& creature, uint8_t soundIdx, bool loop, uint8_t volume) {
  if (creature.getMidiMode()) {
    creature.setMidiMode(false);
    MP3Mode();
  }
  MP3Mode();
  musicPlayer.begin();
  musicPlayer.setVolume(MP3_VOLUME, MP3_VOLUME);
  musicPlayer.playFullFile("track001.mp3");
  while (!musicPlayer.stopped());
}

void Audio::init() {
  pinMode(8, INPUT_PULLUP);
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
  }
  
  if (! musicPlayer.begin()) { // initialise the music player
     Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
  }
}
