#include "Midi.h"
#include "Debug.h"

#include <Adafruit_GFX.h>

void noteOn(uint8_t, uint8_t, uint8_t);
void noteOff(uint8_t, uint8_t, uint8_t);
void setInstrument(uint8_t, uint8_t);
void setVolume(uint8_t, uint8_t);
void setBank(uint8_t, uint8_t);

constexpr Sound* Midi::SOUNDS[];

// Set default sound to none.
volatile uint8_t Midi::_currentIdx = 0;
Sound* volatile Midi::_current = Midi::SOUNDS[Midi::_currentIdx];

volatile bool Midi::_loop = false;
volatile bool Midi::_retrograde = false;

volatile bool gestFlag = false;
volatile bool noteFlag = false;

volatile uint8_t Midi::_instrument = 0;
volatile uint8_t Midi::_transpose = 0;
volatile uint16_t Midi::_duration_offset = 0;

volatile uint8_t noteIdx = 0;
volatile uint16_t duration = 0;

void noteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void noteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void allNoteOff(uint chan) {
  if (chan > 15) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_NOTE_ALL_OFF_1);
  VS1053_MIDI.write(MIDI_NOTE_ALL_OFF_1);
}

void setInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);
  VS1053_MIDI.write(inst);
}

void setVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void setBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t) MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

//Configures the TC to generate output events at the sample frequency.
//Configures the TC in Frequency Generation mode, with an event output once
//each time the audio sample frequency period expires.

void Midi::tcConfigure(int sampleRate)  {
 // Enable GCLK for TCC2 and TC5 (timer counter input clock)
 GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ;
 while (GCLK->STATUS.bit.SYNCBUSY);

 tcReset(); //reset TC5

 // Set Timer counter Mode to 16 bits
 TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
 // Set TC5 mode as match frequency
 TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
 //set prescaler and enable TC5
 TC5->COUNT16.CTRLA.reg |= TIMER_PRESCALER_DIV | TC_CTRLA_ENABLE;
 //set TC5 timer counter based off of the system clock and the user defined sample rate of waveform
 TC5->COUNT16.CC[0].reg = (uint16_t) (CPU_HZ / sampleRate - 1);
 while (tcIsSyncing());

 // Configure interrupt request
 NVIC_DisableIRQ(TC5_IRQn);
 NVIC_ClearPendingIRQ(TC5_IRQn);
 NVIC_SetPriority(TC5_IRQn, 0);
 NVIC_EnableIRQ(TC5_IRQn);

 // Enable the TC5 interrupt request
 TC5->COUNT16.INTENSET.bit.MC0 = 1;
 while (tcIsSyncing()); //wait until TC5 is done syncing
}

//Function that is used to check if TC5 is done syncing
//returns true when it is done syncing
bool tcIsSyncing() {
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

//This function enables TC5 and waits for it to be ready
void Midi::tcStartCounter() {
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
  while (tcIsSyncing()); //wait until snyc'd
}


void Midi::setSound(uint8_t soundIdx, bool loop, uint8_t transpose, uint16_t duration_offset, bool retrograde, int16_t instrument) {
  noInterrupts();

  // Constrain soundIdx. Anything outside of the bounds of the array is 0.
  if (soundIdx >= sizeof(SOUNDS) / sizeof(void*)) {
    soundIdx = 0;
  }

  _loop = loop;
  _transpose = transpose;
  _duration_offset = duration_offset;
  _retrograde = retrograde;
  _instrument = instrument;

  if (_current != SOUNDS[soundIdx]) {
    dprint(F("Next sound: "));
    dprint(soundIdx);
    dprint(loop ? F(", looped") : F(""));
    dprintln(retrograde ? F(", reversed") : F(""));

    if (_current != nullptr && noteFlag) {
      allNoteOff(0);
    }

    _currentIdx = soundIdx;
    _current = SOUNDS[soundIdx];

    noteFlag = false;
    gestFlag = false;

    if (_current != nullptr && instrument < 0) {
      _instrument = _current->instrument;
    }
  }

  interrupts();
}

uint8_t Midi::getSoundIdx() {
  return _currentIdx;
}

Sound* Midi::getSound(){
  return _current;
}

bool Midi::loop() {
  return _loop;
}

bool Midi::retrograde() {
  return _retrograde;
}

uint8_t Midi::instrument() {
  return _instrument;
}

uint8_t Midi::transpose() {
  return (_current == nullptr ? 0 : _current->transpose) + _transpose;
}

uint16_t Midi::duration_offset() {
  return (_current == nullptr ? 0 : _current->duration_offset) + _duration_offset;
}

void Midi::setup() {
  VS1053_MIDI.begin(31250);
  Midi::tcConfigure(1000);  // Hz
  Midi::tcStartCounter();
  allNoteOff(0);
}

//Reset TC5
void tcReset() {
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing());
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

//disable TC5
void tcDisable() {
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}

void TC5_Handler(void) {
  Sound* current = Midi::getSound();
  bool retrograde = Midi::retrograde();

  if (current != nullptr) {
    if (!gestFlag) {
      setVolume(0, current->volume);
      setBank(0, current->bank);
      setInstrument(0, Midi::instrument());
      noteIdx = 0;
      gestFlag = true;
    }

    if (!noteFlag) {
      if (noteIdx >= current->len) {
        if (!Midi::loop()) {
          Midi::setSound(0);
          return;
        }

        noteIdx = 0;
      }

      duration = current->durations[retrograde ? current->len - noteIdx - 1 : noteIdx] + Midi::duration_offset();
      noteOn(0, current->notes[retrograde ? current->len - noteIdx - 1 : noteIdx] + Midi::transpose(), current->volume);
      noteFlag = true;
    }

    if (!--duration) {
      noteOff(0, current->notes[retrograde ? current->len - noteIdx - 1 : noteIdx] + Midi::transpose(), current->volume);
      noteFlag = false;
      noteIdx++;
    }
  }
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}
