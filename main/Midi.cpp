#include "Midi.h"

#include <Adafruit_GFX.h>

uint8_t scale_notes[8] = {64, 66, 68, 70, 72, 74, 76, 78};
uint8_t scale_durations[8] = {50, 50, 50, 50, 50, 50, 50, 50};
SoundGesture Midi::SCALE = {
  VS1053_BANK_MELODY,
  43,
  127,
  true,
  8,
  scale_notes,
  scale_durations
};

SoundGesture* Midi::current = nullptr;

bool gestFlag = false;
bool noteFlag = false;

uint8_t noteIdx = 0;
uint8_t duration = 0;

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);
  VS1053_MIDI.write(inst);
}

void midiSetVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetBank(uint8_t chan, uint8_t bank) {
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

void Midi::setCurrent(SoundGesture* gesture) {
  noInterrupts();
  if (Midi::current != nullptr && noteFlag) {
    midiNoteOff(0, Midi::current->notes[noteIdx], Midi::current->volume);
  }

  Midi::current = gesture;

  if (gesture == nullptr) {
    noteFlag = false;
    gestFlag = false;
  }
  interrupts();
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

void TC5_Handler (void) {
  if (Midi::current != nullptr) {
    if (!gestFlag) {
      midiSetVolume(0, Midi::current->volume);
      midiSetBank(0, Midi::current->bank);
      midiSetInstrument(0, Midi::current->instrument);
      noteIdx = 0;
      gestFlag = true;
    }

    if (!noteFlag) {
      if (noteIdx == Midi::current->len) {
        if (!Midi::current->loop) {
          gestFlag = false;
          Midi::current = nullptr;
          return;
        }

        noteIdx = 0;
      }

      duration = Midi::current->durations[noteIdx];
      midiNoteOn(0, Midi::current->notes[noteIdx], Midi::current->volume);
      noteFlag = true;
    }

    if (!--duration) {
      midiNoteOff(0, Midi::current->notes[noteIdx], Midi::current->volume);
      noteFlag = false;
      noteIdx++;
    }
  }
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}
