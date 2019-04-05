#include "Creature.h"
#include <Adafruit_VS1053.h>
#include "Audio.h"


#define MP3_VOLUME 1

#define VS1053_RESET   -1     // VS1053 reset pin (not used!)
#define VS1053_CS       6     // VS1053 chip select pin (output)
#define VS1053_DCS     10     // VS1053 Data/command select pin (output)
#define CARDCS          5     // Card chip select pin
// DREQ should be an Int pin
#define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

Creature c;

void setup() {
  Serial.begin(115200);
  delay(3000);
  Audio::init();


  Serial.println(F("Initializing creature"));
  c.setup();
}

void loop() {
  c.loop();
}
