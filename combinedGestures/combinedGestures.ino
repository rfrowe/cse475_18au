/*
 * Combined Light and Sound Gestures for each state.
 * Author: Akhil Avula, Kheng Wei Ang, Nick Good, Varun Venkatesh
 * EE 475 Autumn 2018
 */

 #include <Adafruit_FeatherOLED.h>
 #include <soundGestures.ino>
 #include <lightGestures.ino>


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
  // put your main code here, to run repeatedly:

}
