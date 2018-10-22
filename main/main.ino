#include "Creature.h"

Creature c;

void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println(F("Initializing creature"));
  c.setup();
}

void loop() {
  c.loop();
}
