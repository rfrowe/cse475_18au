/*
 * Combined Light and Sound Gestures for each state.
 * Author: Akhil Avula, Kheng Wei Ang, Nick Good, Varun Venkatesh
 * EE 475 Autumn 2018
 */

#include <Adafruit_FeatherOLED.h>
#include <SPI.h>
#define CPU_HZ 48000000
#define TIMER_PRESCALER_DIV 1000
#include <Adafruit_NeoPixel_ZeroDMA.h>

// DMA NeoPixels work ONLY on SPECIFIC PINS.
// On Circuit Playground Express: 8, A2 and A7 (TX) are valid.
// On Feather M0, Arduino Zero, etc.: 5, 11, A5 and 23 (SPI MOSI).
// On GEMMA M0: pin 0.
// On Trinket M0: pin 4.
// On Metro M4: 3, 6, 8, 11, A3 and MOSI
#define PIN         A5
#define NUM_PIXELS 16
#define GESTURE 0 // 0: startle gesture, 1 : ambient gesture, 2: active gesture
/****CHANGE THE GESTURE NUMBER TO TEST OUT EACH GESTURE****/

Adafruit_FeatherOLED oled = Adafruit_FeatherOLED();
Adafruit_NeoPixel_ZeroDMA strip(NUM_PIXELS, PIN, NEO_GRBW);

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
uint16_t volume = 127;
uint16_t chord3[3];
uint16_t chord4[4];
uint16_t chord6[6];
uint16_t isChord = 0;
uint16_t channel = 127;
//uint16_t velocity = 50;

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

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
#define dottedEighthNote 38
#define quarterNote 50
#define dottedQuarterNote 75
#define halfNote 100
#define wholeNote 200
#define fourTiedWholeNotes 800

// define voices
# define synthVoice 54
# define fifthsLead 87
# define brightness 101
# define timpani 48
# define synthBass1 39


uint8_t startleSound[14] =
  {89, 4,
  volume,88,quarterNote, volume,88,halfNote, volume,76,quarterNote, volume,76,halfNote};

uint8_t ambientSound[27] = 
   {brightness,3,
   volume,70,wholeNote, volume,72,wholeNote, volume,74,wholeNote};

uint8_t activeSound[23] = 
   {104,7,
   volume,52,eighthNote, volume,52,eighthNote, volume,64,eighthNote,
   volume,52,eighthNote, volume,52,eighthNote, volume,52,eighthNote, volume,59,eighthNote};

void setup() {
  delay(100);
  
  Serial.begin(115200);
  delay(100);

  oled.begin(); //Initialize the oled display
  oled.display();
  oled.clearDisplay();
  oled.init();
  oled.setCursor(0, 0); // 0: startle, 1: ambient, 2: active
  if (0 == GESTURE) {
    oled.print("Startle Gesture");
  } else if (1 == GESTURE) { // ambient
    oled.print("Ambient Gesture");
  } else if (2 == GESTURE) { // ambient
    oled.print("Active Gesture");
  } 
  oled.display();

  // for lightGestures
  strip.begin(); // Initialize the LEDs
  strip.setBrightness(32);
  strip.show();

  // for soundGestures
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
     if(0 == GESTURE) {
      midiSetInstrument(0, startleSound[0]);
     }else if(1 == GESTURE) {
      midiSetInstrument(0, ambientSound[0]);
     } else if (2 == GESTURE) {
      midiSetInstrument(0, activeSound[0]);
     } 
   noInterrupts();
   // critical, time-sensitive code here
   playFlag = true;
   
   //set Tempo?? 10-9
   if(--tempo >0) tempo = 9;
   interrupts();
   if(0 == GESTURE) {
      startleLight();
   } else if(1 == GESTURE) {
      ambientLight();
   } else if (2 == GESTURE) {
      activeLight();
   }
  }
}

// FROM lightGestures.ino

void startleLight(void) {
  uint16_t i;
  uint32_t temp;
  strip.setBrightness(13);
    for(i=0; i<=strip.numPixels(); i++) {
      strip.setPixelColor(i, 0xFF0000 << 8);
      strip.show();
      delay(50);
    }
    for(i=0; i<=strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
    strip.show();
    delay(500);
    for (int j = 0; j < 2; j++) {
      for(i=0; i<=strip.numPixels(); i++) {
        strip.setPixelColor(i,0xFF0000 << 8);
      }
      strip.show();
      delay(500);
      for(i=0; i<=strip.numPixels(); i++) {
        strip.setPixelColor(i, 0);
      }
      strip.show();
      delay(500);
    }
}

void ambientLight(void) {
  uint16_t i;
  uint32_t temp;
  uint32_t elapsed, t, startTime = micros();
  for (;;) {
    t       = micros();
    elapsed = t - startTime;
    if(elapsed > 5000000) break; // Run for 5 seconds
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((uint8_t)(
        (elapsed * 256 / 1000000) + i * 256 / strip.numPixels())));
    }
    bounceBrightness(8,33,50);
  }
}

void bounceBrightness(int low, int high, int delTime) {
  for (int j = low; j < high; j++) {
    strip.setBrightness(j);
    strip.show();
    delay(delTime);
  }
  for (int k = high - 1; k >= low; k--) {
    strip.setBrightness(k);
    strip.show();
    delay(delTime);
  }
}

void activeLight(void) {
  uint16_t i;
  uint32_t temp;
  for(uint32_t c = 0xFF0000; c; c >>= 8) { // Red, green, blue
    for(i=0; i<=strip.numPixels()/2; i++) {
      strip.setPixelColor(i, c);
      strip.setPixelColor(strip.numPixels() - i, c >> 8);
      strip.show();
      delay(50);
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}



// from soundGestures.ino

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
  if(0 == GESTURE) {
    if(playFlag){
        if(!gestFlag){
          // start Gesture
          gestFlag = true;
          //set Voice
          gCnt = 1;
          //set notecnt
          noteCnt = startleSound[gCnt++];
          
        }
        if(!noteFlag){ // sets characteristics of the note

          //set volume
          volume = startleSound[gCnt++];
          //set noteNum
          noteNum = startleSound[gCnt++];
          //set duration
          duration = startleSound[gCnt++]-tempo;
  
         //start note
          midiNoteOn(0, noteNum, volume);
          
          noteFlag = true;
       }
        duration -= 1; // sustains the note until duration = 0
        if(duration==0){
          noteCnt -= 1;
          midiNoteOff(0, noteNum, 127);
          if(noteCnt==0){
            //clear all flags
            playFlag = false;
            gestFlag = false;
          }
          noteFlag = false;
        }
      }  // END OF YOUR CODE
    TC5->COUNT16.INTFLAG.bit.MC0 = 1; //don't change this, it's part of the timer code 
  } else if (1 == GESTURE) {
      if(playFlag){
        if(!gestFlag){
          // start Gesture
          gestFlag = true;
          //set Voice
          gCnt = 1;
          //set notecnt
          noteCnt = ambientSound[gCnt++];
          
        }
        if(!noteFlag){ // sets characteristics of the note

          //set volume
          volume = ambientSound[gCnt++];
          //set noteNum
          noteNum = ambientSound[gCnt++];
          //set duration
          duration = ambientSound[gCnt++]-tempo;
  
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
  } else if(2 == GESTURE){
    if(playFlag){
        if(!gestFlag){
          // start Gesture
          gestFlag = true;
          //set Voice
          gCnt = 1;
          //set notecnt
          noteCnt = activeSound[gCnt++];
          
        }
        if(!noteFlag){ // sets characteristics of the note

          //set volume
          volume = activeSound[gCnt++];
          //set noteNum
          noteNum = activeSound[gCnt++];
          //set duration
          duration = activeSound[gCnt++]-tempo;
  
         //start note
          midiNoteOn(0, noteNum, volume);
          
          noteFlag = true;
       }
        duration -= 1; // sustains the note until duration = 0
        if(duration==0){
          noteCnt -= 1;
          midiNoteOff(0, noteNum, 127);
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
     
