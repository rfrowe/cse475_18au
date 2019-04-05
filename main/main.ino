#include <Adafruit_VS1053.h>

#include "Creature.h"
#include "Audio.h"

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
