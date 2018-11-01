
// Solder closed jumper on bottom!
#include <SPI.h>
#include <Adafruit_FeatherOLED.h>
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1000

#define GESTURE 3 // 0 1 2 3 4 5 6 

int sampleRate = 1000; //sample rate of the square wave in Hertz, how many times per second the TC5_Handler() function gets called per second basically

void tcStartCounter(); //starts the timer
void TC5_Handler();

bool playFlag = false;
bool gestFlag = false;
bool noteFlag = false;
uint16_t noteNum = 64;
uint16_t gNum = 255;
uint16_t gCnt = 0;
uint16_t noteCnt = 7;
uint16_t noteLength = 50;
uint16_t duration = 50;
uint16_t tempo = 8;
uint16_t volume = 50;//127;
uint16_t chord[4];
uint16_t isChord = 0;
uint16_t channel = 127;
//uint16_t velocity = 50;

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
#define VS1053_GM1_OCARINA 80

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0

#define VS1053_MIDI Serial1

// define note length
#define sixteenthNote 12
#define eighthNote 25
#define quarterNote 50
#define halfNote 100
#define wholeNote 200

// define voices
# define synthVoice 54
# define glockenspiel 10
Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();

uint8_t gesture1[8] = 
   {54,1,
   volume,64,68,71,76,wholeNote};

uint8_t gesture2[27] = 
   {101,3,
   volume,70,wholeNote, volume,72,wholeNote, volume,74,wholeNote};

uint8_t gesture3[5][5] = {
   {1,1,
   volume,26,wholeNote},
   {1,1,
   volume,45,wholeNote},
   {1,1,
   volume,38,wholeNote},
   {1,1,
   volume,40,wholeNote},
   {1,1,
   volume,26,wholeNote}
   };

void setup() {
  delay(100);
  
  Serial.begin(115200);
  delay(100);

  oled.begin(); //Initialize the oled display
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setCursor(0, 0);
  if (0 == GESTURE) {
    oled.print("Startle Gesture");
  } else if (1 == GESTURE) { // ambient
    oled.print("Ambient Gesture 1");
  } else if (2 == GESTURE) { // ambient
    oled.print("Ambient Gesture 2");
  } else if (3 == GESTURE) { // ambient
    oled.print("Ambient Gesture 3");
  } else if (4 == GESTURE) { // active
    oled.print("Active Gesture 1");
  } else if (5 == GESTURE) { // active
    oled.print("Active Gesture 2");
  } else if (6 == GESTURE) { // active
    oled.print("Active Gesture 3");
  }
  oled.display();

  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate' 
  midiSetChannelBank(0, VS1053_BANK_MELODY);
  midiSetChannelVolume(0, 127);

  tcConfigure(sampleRate); //configure the timer to run at <sampleRate>Hertz
  tcStartCounter(); //starts the timer
}

void loop() {
  //handle sound
  if (!playFlag) {
    //set Gesture 0-15
    //if(++gNum >7) gNum = 0;
   // set voice 0-14
   if(1 == GESTURE) {
    midiSetInstrument(0, gesture1[0]); 
   } else if (2 == GESTURE) {
    midiSetInstrument(0, gesture2[0]); 
   } else if (3 == GESTURE) {
     if(++gNum >7) gNum = 0;
     // set voice 0-14
     midiSetInstrument(0, gesture3[gNum][0]);
   }
    noInterrupts();
    // critical, time-sensitive code here
    playFlag = true;

    //set Tempo?? 10-9
    if(--tempo >0) tempo = 9;
    interrupts();
  }
     
}

void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);  
  delay(10);
  VS1053_MIDI.write(inst);
  delay(10);
}


void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;
  
  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

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

//Configures the TC to generate output events at the sample frequency.
//Configures the TC in Frequency Generation mode, with an event output once
//each time the audio sample frequency period expires.

void tcConfigure(int sampleRate)
{
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
bool tcIsSyncing()
{
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

//This function enables TC5 and waits for it to be ready
void tcStartCounter()
{
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
  while (tcIsSyncing()); //wait until snyc'd
}

//Reset TC5 
void tcReset()
{
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing());
  while (TC5->COUNT16.CTRLA.bit.SWRST);
}

//disable TC5
void tcDisable()
{
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing());
}


//this function gets called by the interrupt at <sampleRate>Hertz
void TC5_Handler (void) {
  if(1 == GESTURE){
      if(playFlag){
        if(!gestFlag){
          // start Gesture
          gestFlag = true;
          //set Voice
          gCnt = 1;
          //set notecnt
          noteCnt = gesture1[gCnt++];
          
        }
        if(!noteFlag){ // sets characteristics of the note

          volatile int i;
          for (i=0; i<sizeof(chord); i++) {
            chord[i] = gesture1[gCnt++];
          }
          //set duration
          duration = gesture1[gCnt++]-tempo;
          
          //start note
          for (i=0; i<sizeof(chord); i++) {
            noteNum = chord[i];
            midiNoteOn(0, noteNum, volume); // plays the note
          }
          
          noteFlag = true;
       }
        duration -= 1; // sustains the note until duration = 0
        if(duration==0){
          noteCnt -= 1;
          //midiNoteOff(0, noteNum, 0);
          if(noteCnt==0){
            //clear all flags
            playFlag = false;
            gestFlag = false;
          }
          noteFlag = false;
        }
      }  // END OF YOUR CODE
    TC5->COUNT16.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code
  } else if (2 == GESTURE) {
      if(playFlag){
        if(!gestFlag){
          // start Gesture
          gestFlag = true;
          //set Voice
          gCnt = 1;
          //set notecnt
          noteCnt = gesture2[gCnt++];
          
        }
        if(!noteFlag){ // sets characteristics of the note

          //set volume
          volume = gesture2[gCnt++];
          //set noteNum
          noteNum = gesture2[gCnt++];
          //set duration
          duration = gesture2[gCnt++]-tempo;
  
         //start note
          midiNoteOn(0, noteNum, volume);

//          volatile int i;
//          for (i=0; i<sizeof(chord); i++) {
//            chord[i] = gesture2[gCnt++];
//          }
//          //set duration
//          duration = gesture2[gCnt++]-tempo;
//          
//          //start note
//          for (i=0; i<sizeof(chord); i++) {
//            noteNum = chord[i];
//            midiNoteOn(0, noteNum, volume); // plays the note
//          }
          
          noteFlag = true;
       }
        duration -= 1; // sustains the note until duration = 0
        if(duration==0){
          noteCnt -= 1;
          //midiNoteOff(0, noteNum, 0);
          if(noteCnt==0){
            //clear all flags
            playFlag = false;
            gestFlag = false;
          }
          noteFlag = false;
        }
      }  // END OF YOUR CODE
    TC5->COUNT16.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code
  } else if (3 == GESTURE) {
    if(playFlag){
        if(!gestFlag){
          // start Gesture
          gestFlag = true;
          //set Voice
          gCnt = 1;
          //set notecnt
          noteCnt = gesture3[gNum][gCnt++];
          
        }
        if(!noteFlag){ // sets characteristics of the note
          
          //set volume
          volume = gesture3[gNum][gCnt++];
          //set noteNum
          noteNum = gesture3[gNum][gCnt++];
          //set duration
          duration = gesture3[gNum][gCnt++]-tempo;
  
         //start note
          midiNoteOn(0, noteNum, volume);
          
          noteFlag = true;
       }
        duration -= 1; // sustains the note until duration = 0
        if(duration==0){
          noteCnt -= 1;
          //midiNoteOff(0, noteNum, 0);
          if(noteCnt==0){
            //clear all flags
            playFlag = false;
            gestFlag = false;
          }
          noteFlag = false;
        }
      }  // END OF YOUR CODE
    TC5->COUNT16.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code
  }
}

